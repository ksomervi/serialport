// Simple class for communicating with a serial port.
// Copyright (C) 2105  Kevin Somervill
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
// USA.
// Filename: communication_manager.h
// Author: Kevin Somervill <kevin@somervill.org>
// Date: Thu Apr  9 15:07:52 EDT 2015

#include <cstdio>       /* Standard input/output definitions */
#include <cstdlib>
#include <cstring>      /* String function definitions */
#include <unistd.h>     /* UNIX standard function definitions */

#include <fcntl.h>      /* File control definitions */
#include <cerrno>       /* Error number definitions */

#ifdef __linux__
#include <termios.h>    /* POSIX terminal control definitions */
#include <sys/ioctl.h>

#else

#include <windows.h>
  // If usleep needs to be replaced, here's a gist for it:
  //   https://gist.github.com/ngryman/6482577
#endif

#include "communication_manager.h"

communication_manager::communication_manager() {
  _sp = NULL;
  _max_retries = 1000; //hack
}

communication_manager::~communication_manager() {
  if (_sp) {
    delete _sp;
    _sp = NULL;
  }
  _max_retries = 1000; //hack
}

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns true on success or false on error
bool communication_manager::open_session(const char* serialport, int baud) {
  _sp = new serial_port();

  if (_sp->open(serialport, baud)) {
    return true;
  }

  delete _sp;
  _sp = NULL;
  return false;
}

void communication_manager::close_session() {
#if defined (CM_APP_DEBUG)
  printf("close_session: closing port %s\n", _sp->name());
#endif
  _sp->close();
  delete _sp;
  _sp = NULL;
}

int communication_manager::baudrate() {
  if (_sp) {
    return _sp->baudrate();
  }
  return 0;
}

char * communication_manager::port_name() {
  if (_sp) {
    return _sp->name();
  }
  return NULL;
}

bool communication_manager::session_is_open(void) {
  if (_sp) {
    return (_sp->is_open());
  }
  return false;
}

int communication_manager::send(const uint8_t* str, int len) {
  int n = _sp->send((unsigned char*)str, len);
  if (n!=len) {
    return -1;
  }
  return 0;
}

int communication_manager::receive(uint8_t* buf, int len, bool dbg) {
  unsigned char b;
  int i = 0;
  int tries = 0;

  while (i<len && tries<_max_retries) {
    int n = _sp->poll(&b, 1);  // read a char at a time
    if (n==-1) {
      printf("Can't read port in receive()\n");
      return -1;    // couldn't read
    }
    else if (n==0) {
      tries++;
      usleep( 10 * 1000 ); // wait 10 msec try again
    }
    else {
      if (dbg) {
        printf ("    read: 0x%X\n", b);
      }
      buf[i] = b;
      i++;
    }
  }

  if (tries == _max_retries) {
    printf("too many tries in receive()\n");
  }
  //buf[i] = 0;  // null terminate the string
  return (i);
}

int communication_manager::poll(uint8_t* buf, bool dbg) {
  return(_sp->poll((unsigned char*)buf, 1));
}

port_list communication_manager::get_available_ports() {
  return _sp->available_ports();
}//end get_available_ports()


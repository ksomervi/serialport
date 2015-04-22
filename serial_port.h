// Kevin Somervill
// Minor modifications to work by:
//   Original author: Teunis van Beelen
//
// Copyright (C) 2005, 2006, 2007, 2008, 2009 Teunis van Beelen
//
// teuniz@gmail.com
//
//**************************************************************************
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//
// This version of GPL is at 
//       http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
//

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <cstdio>
#include <cstring>

#ifdef __linux__

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#define RS232_MIN_PORT_INDEX  0
#define RS232_MAX_PORT_INDEX  25
#define INVALID_PORT          -1

#else

#include <windows.h>

#define RS232_MIN_PORT_INDEX  1
#define RS232_MAX_PORT_INDEX  16

#endif

#define MAX_READ_SIZE 4096

class serial_port {
  private:
    int _baudrate;
    char * _port_name;

#ifdef __linux__
    int _port_fd;
    struct termios new_port_settings;
    struct termios old_port_settings;
#else
    HANDLE _port_handle;
    char _baud_str[64];
    int _port_number;
#endif
    int _error;

  public:
    serial_port();

    void name(const char *);
    char * name();
    int baudrate();
    void baudrate(int);
    bool open(int, int);
    bool open(const char *, int);
    bool is_open();
    int poll(unsigned char *, int);
    int send(unsigned char);
    int send(unsigned char *, int);
    void close();
    //void cprintf(int, const char *);
    //int IsCTSEnabled(int);

    bool is_valid_port(const char *);
    int get_port_index(const char *);

};

#endif //!defined(SERIAL_PORT_H)


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
#ifndef COMM_MANAGER_H
#define COMM_MANAGER_H

#include<stdint.h>
#include "serial_port.h"

#ifndef INVALID_PORT
#define INVALID_PORT -1
#endif

#define CM_APP_DEBUG 1

//TODO:
// - Add poll() function that tests for available characters, but does leaves
//   them in the buffer.
// - Add flush() function that clears the selected buffer

class communication_manager {

  private:
    serial_port * _sp;

  public:
    communication_manager();
    ~communication_manager();

    int baudrate();
    char * port_name();

    bool open_session(const char* serialport, int baud);
    bool session_is_open();
    void close_session();

    //int send(const uint8_t b);
    int send(const uint8_t* buf, int len);
    int receive(uint8_t* buf, int len, bool dbg=false);

};

#endif // !defined (COMM_MANAGER_H)


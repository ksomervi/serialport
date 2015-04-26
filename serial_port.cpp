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

#include "serial_port.h"

#ifdef __linux__   /* Linux */

static char comports[RS232_MAX_PORT_INDEX+1][13] = {
  "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3",
  "/dev/ttyS4", "/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7",
  "/dev/ttyS8", "/dev/ttyS9", "/dev/ttyS10", "/dev/ttyS11",
  "/dev/ttyS12", "/dev/ttyS13", "/dev/ttyS14", "/dev/ttyS15",
  "/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2", "/dev/ttyUSB3",
  "/dev/ttyUSB4", "/dev/ttyUSB5",
  "/dev/ttyACM0", "/dev/ttyACM1", "/dev/ttyACM2", "/dev/ttyACM3"
};

/*
int serial_port::GetPortIndex (const char *name) {
  // get index into comports
  for (int i=0; i<RS232_MAX_PORT_INDEX; i++) {
    if (strcmp(name, comports[i]) == 0) {
      return (i);
    }
  }
  return (-1);
}//end 
*/

serial_port::serial_port() {
  _port_fd = INVALID_PORT;
  _port_name = NULL;
}

void serial_port::name(const char * name) {
  if (is_valid_port(name)) {
    _port_name = comports[get_port_index(name)];
  }
  else {
    _port_name = NULL;
  }
}

void serial_port::baudrate(int rate) {
  _baudrate = rate;
}

bool serial_port::open(int comport_number, int baud) {
  if((comport_number>RS232_MAX_PORT_INDEX)||(comport_number<RS232_MIN_PORT_INDEX)) {
    printf("illegal comport number\n");
    return(false);
  }

  _port_name = comports[comport_number];
  return open(_port_name, baud);
}

bool serial_port::open(const char * name, int baud) {
  int _baudr = B9600;
  this->name(name);
  if (_port_name == NULL) {
    printf("invalid port name\n");
    return(false);
  }

  switch(baud) {
    case      50 : _baudr = B50;
                   break;
    case      75 : _baudr = B75;
                   break;
    case     110 : _baudr = B110;
                   break;
    case     134 : _baudr = B134;
                   break;
    case     150 : _baudr = B150;
                   break;
    case     200 : _baudr = B200;
                   break;
    case     300 : _baudr = B300;
                   break;
    case     600 : _baudr = B600;
                   break;
    case    1200 : _baudr = B1200;
                   break;
    case    1800 : _baudr = B1800;
                   break;
    case    2400 : _baudr = B2400;
                   break;
    case    4800 : _baudr = B4800;
                   break;
    case    9600 : _baudr = B9600;
                   break;
    case   19200 : _baudr = B19200;
                   break;
    case   38400 : _baudr = B38400;
                   break;
    case   57600 : _baudr = B57600;
                   break;
    case  115200 : _baudr = B115200;
                   break;
    case  230400 : _baudr = B230400;
                   break;
    case  460800 : _baudr = B460800;
                   break;
    case  500000 : _baudr = B500000;
                   break;
    case  576000 : _baudr = B576000;
                   break;
    case  921600 : _baudr = B921600;
                   break;
    case 1000000 : _baudr = B1000000;
                   break;
    default      : printf("invalid baudrate\n");
                   return(false);
                   break;
  }
  _baudrate = baud;

  _port_fd = ::open(_port_name, O_RDWR | O_NOCTTY | O_NDELAY);
  if(_port_fd == INVALID_PORT) {
    perror("unable to open comport ");
    return(false);
  }

  _error = tcgetattr(_port_fd, &old_port_settings);
  if(_error == -1) {
    perror("unable to read portsettings ");
    if(::close(_port_fd) == -1) {
      perror("unable to close port ");
    }
    return(false);
  }
  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

  new_port_settings.c_cflag = _baudr | CS8 | CLOCAL | CREAD;
  new_port_settings.c_iflag = IGNPAR;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block until n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block until a timer expires (n * 100 mSec.) */
  _error = tcsetattr(_port_fd, TCSANOW, &new_port_settings);
  if(_error==-1) {
    perror("unable to adjust portsettings ");
    if (::close(_port_fd) == -1) {
      perror("unable to close serial port");
    }
    _port_fd = INVALID_PORT;
    return(false);
  }

  return(true);
}

bool serial_port::is_open() {
  return (_port_fd != INVALID_PORT);
}

int serial_port::poll(unsigned char *buf, int size) {
  if(size>MAX_READ_SIZE) {
    size = (int)MAX_READ_SIZE;
  }

  return(read(_port_fd, buf, size));
}

int serial_port::send(unsigned char byte) {
  int n = write(_port_fd, &byte, 1);

  if(n<0) {
    return(1);
  }

  return(0);
}

int serial_port::send(unsigned char *buf, int size) {
  return(write(_port_fd, buf, size));
}

void serial_port::close() {
  tcsetattr(_port_fd, TCSANOW, &old_port_settings);
  if (::close(_port_fd) == -1) {
    perror("unable to close serial port");
    return;
  }
  _port_fd = INVALID_PORT;
}

/*
Constant  Description
TIOCM_LE  DSR (data set ready/line enable)
TIOCM_DTR DTR (data terminal ready)
TIOCM_RTS RTS (request to send)
TIOCM_ST  Secondary TXD (transmit)
TIOCM_SR  Secondary RXD (receive)
TIOCM_CTS CTS (clear to send)
TIOCM_CAR DCD (data carrier detect)
TIOCM_CD  Synonym for TIOCM_CAR
TIOCM_RNG RNG (ring)
TIOCM_RI  Synonym for TIOCM_RNG
TIOCM_DSR DSR (data set ready)

int IsCTSEnabled() {
  int status = ioctl(_port_fd, TIOCMGET, &status);

  if(status&TIOCM_CTS) {
    return(1);
  }
  return(0);
}
*/

#else         /* windows */

char comports[RS232_MAX_PORT_INDEX+1][10] = {
  "\\\\.\\COM0",  /* INVALID */
  "\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",  "\\\\.\\COM4",
  "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",  "\\\\.\\COM8",
  "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11", "\\\\.\\COM12",
  "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15", "\\\\.\\COM16"
};

serial_port::serial_port() {
  _port_handle = INVALID_HANDLE_VALUE;
  _port_name = NULL;
}

bool serial_port::open(const char * name, int baud) {
  sscanf(name, "COM%d", &_port_number);
  return (this->open(_port_number, baud));
}

bool serial_port::open(int comport_number, int baud) {
  if((comport_number>15)||(comport_number<0)) {
    printf("illegal comport number\n");
    return(false);
  }

  sprintf(_baud_str, "baud=%d data=8 parity=N stop=1", baud);
  _baudrate = baud;

  _port_handle = CreateFileA(comports[comport_number],
                      GENERIC_READ|GENERIC_WRITE,
                      0,                          /* no share  */
                      NULL,                       /* no security */
                      OPEN_EXISTING,
                      0,                          /* no threads */
                      NULL);                      /* no templates */

  if(_port_handle == INVALID_HANDLE_VALUE) {
    printf("unable to open comport\n");
    return(false);
  }

  DCB port_settings;
  memset(&port_settings, 0, sizeof(port_settings));  /* clear the new struct  */
  port_settings.DCBlength = sizeof(port_settings);

  if(!BuildCommDCBA(_baud_str, &port_settings)) {
    printf("unable to set comport dcb settings\n");
    CloseHandle(_port_handle);
    _port_handle = INVALID_HANDLE_VALUE;
    return(false);
  }

  if(!SetCommState(_port_handle, &port_settings)) {
    printf("unable to set comport cfg settings\n");
    CloseHandle(_port_handle);
    _port_handle = INVALID_HANDLE_VALUE; //not sure this is required
    return(false);
  }

  COMMTIMEOUTS Cptimeouts;

  Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
  Cptimeouts.ReadTotalTimeoutConstant    = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant   = 0;

  if(!SetCommTimeouts(_port_handle, &Cptimeouts)) {
    printf("unable to set comport time-out settings\n");
    CloseHandle(_port_handle);
    return(false);
  }

  _port_name = comports[_port_number];
  return(true);
}


int serial_port::poll(unsigned char *buf, int size) {
  int n;

  if (MAX_READ_SIZE < size) {
    size = MAX_READ_SIZE;
  }

/* added the void pointer cast, otherwise gcc will complain about */
/* "warning: dereferencing type-punned pointer will break strict aliasing rules" */

  ReadFile(_port_handle, buf, size, (LPDWORD)((void *)&n), NULL);

  return(n);
}


int serial_port::send(unsigned char byte) {
  int n;

  WriteFile(_port_handle, &byte, 1, (LPDWORD)((void *)&n), NULL);

  if (n<0) {
    return(-1);
  }

  return(n);
}


int serial_port::send(unsigned char *buf, int size) {
  int n;

  if (WriteFile(_port_handle, buf, size, (LPDWORD)((void *)&n), NULL)) {
    return(n);
  }

  return(-1);
}


void serial_port::close() {
  CloseHandle(_port_handle);
  _port_handle = INVALID_HANDLE_VALUE;
  _port_name = NULL;
}

bool serial_port::is_open() {
  return (_port_handle != INVALID_HANDLE_VALUE);
}


/*
int IsCTSEnabled(int comport_number) {
  int status;

  GetCommModemStatus(_port_handle, (LPDWORD)((void *)&status));

  if (status&MS_CTS_ON) {
    return(1);
  }

  return(0);
}
*/

#endif

bool serial_port::is_valid_port (const char *name) {
  // get index into comports
  for (int i=RS232_MIN_PORT_INDEX; i<=RS232_MAX_PORT_INDEX; i++) {
    if (strcmp(name, comports[i]) == 0) {
      return (true);
    }
  }
  return (false);
}//end 

int serial_port::get_port_index (const char *name) {
  // get index into comports
  for (int i=RS232_MIN_PORT_INDEX; i<=RS232_MAX_PORT_INDEX; i++) {
    if (strcmp(name, comports[i]) == 0) {
      return (i);
    }
  }
  return (-1);
}//end 

char * serial_port::name() {
  return _port_name;
}

int serial_port::baudrate() {
  return _baudrate;
}

port_list serial_port::available_ports() {
  port_list available;
#ifdef _WIN32
  // Needed for QueryDosDevice
  char path[128];
#endif
  int n = 0;
 
  // List the available serial ports
  for (int p=RS232_MIN_PORT_INDEX; p<=RS232_MAX_PORT_INDEX; p++) {
#ifdef _WIN32
    n = QueryDosDevice(comports[p], path, sizeof(path));
#else
    n = access(comports[p], F_OK) + 1;
#endif

    if (n>0) {
      available.push_back(comports[p]);
    }
  }//end for(p)

  return available;
}



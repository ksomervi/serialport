# serialport
Simple serial port class (in C++).

This was derived from the rs232 source code by Teunis van Beelen and partially from the example at  https://123a321.wordpress.com/2010/02/01/serial-port-with-mingw/.
 
This package includes two primary classes:

    class serial_port {
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

        bool is_valid_port(const char *);
        int get_port_index(const char *);

    };


    class communication_manager {
      public:
        communication_manager();
        ~communication_manager();

        int baudrate();
        char * port_name();

        bool open_session(const char* serialport, int baud);
        bool session_is_open();
        void close_session();

        int send(const uint8_t* buf, int len);
        int receive(uint8_t* buf, int len, bool dbg=false);
        int poll(uint8_t* buf, bool dbg=false);

    };

The communication_manager class provides a simplified interface to use serial
ports.


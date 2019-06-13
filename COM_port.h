#include <iostream>
#include <fstream>
#include <stdexcept>

#define RS232_BUFFERSIZE 64

class COM_class{
public:
    enum com_status {COM_ERROR, COM_OK};
    bool IsPresent[16];
    char TxBuffer[RS232_BUFFERSIZE];

    int Open(int port_number);
    int Close(int port_number);
    int List(void);
    int Shake(void);
};

extern COM_class COM;

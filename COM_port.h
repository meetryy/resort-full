#include <iostream>
#include <fstream>
#include <stdexcept>

#define RS232_BUFFERSIZE 64

extern bool COM_present[16];
extern char TxBuffer[RS232_BUFFERSIZE];

int open_COM(int port_number);
int close_COM(int port_number);
int list_COM(void);
int handshake_COM(void);

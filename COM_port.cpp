#include <iostream>
#include <fstream>
#include <stdexcept>

#include "rs232.h"
#include "COM_port.h"
#include "variables1.h"
#include "UI.h"
#include "rs232.h"
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

char TxBuffer[RS232_BUFFERSIZE];

enum com_status {COM_ERROR, COM_OK};

int open_COM(int port_number);
int close_COM(int port_number);

bool COM_present[16] = {0};

int open_COM(int port_number){
        //port_number=0,         /* /dev/ttyS0 (COM1 on windows) */
        //com_speed=9600;       /* 9600 baud */
        char mode[]={'8','N','1',0};
        if(RS232_OpenComport(port_number, com_speed, mode)){
            char buffer[64];
            sprintf(buffer, "error opening COM%d @ %u", port_number+1, com_speed);

        return(COM_ERROR);
      }
      else {
            com_connected = 1;
            return COM_OK;}

}


int close_COM(int port_number){
    RS232_CloseComport(port_number);
    com_connected = 0;
}

int list_COM(void){
    char mode[]={'8','N','1',0};
    for (int k = 0; k<16; k++){
        if (RS232_OpenComport(k, 9600, mode) == 0){
                close_COM(k);
                COM_present[k] = 1;
        }
        else {
            COM_present[k] = 0;
        }
    }
    return 0;
}

int handshake_COM(void){
    RS232_cputs(com_port_number, "HANDSHAKE QUERY");
    cv::waitKey(100);

    unsigned char RXbuf[64];
    int  n = RS232_PollComport(com_port_number, RXbuf, 63);
    if(n > 0)
    {
      RXbuf[n] = 0;
      for(int i = 0; i < n; i++)
      {
        if(RXbuf[i] < 32)
        {
          RXbuf[i] = '.';
        }
      }
     if (RXbuf == (unsigned char*)"HANDSHAKE ANSWER") {return 0;}
     //else{ShowError("handshake failed!");}
    }
    return 0;

    //else{ShowError("handshake failed!");}
}



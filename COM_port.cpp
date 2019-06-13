#include <iostream>
#include <fstream>
#include <stdexcept>

#include "rs232.h"
#include "COM_port.h"
#include "variables1.h"
#include "newGUI.h"
#include "rs232.h"

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

COM_class COM;

int COM_class::Open(int port_number){
        //port_number=0,         /* /dev/ttyS0 (COM1 on windows) */
        //V.ComPort.Speed=9600;       /* 9600 baud */
        char mode[]={'8','N','1',0};
        if(RS232_OpenComport(port_number, V.ComPort.Speed, mode)){
            //char buffer[64];
            //sprintf(buffer, "error opening COM%d @ %u", port_number+1, V.ComPort.Speed);
            GUI.ConsoleOut(u8"ќЎ»Ѕ ј: Ќевозможно открыть порт!");

        return(COM_ERROR);
      }
      else {
            V.ComPort.Connected = 1;
             GUI.ConsoleOut(u8"—¬я«№: ѕорт открыт");
            return COM_OK;}
}

int COM_class::Close(int port_number){
    RS232_CloseComport(port_number);
    V.ComPort.Connected = 0;
    GUI.ConsoleOut(u8"—¬я«№: ѕорт закрыт");
}

int COM_class::List(void){
    char mode[]={'8','N','1',0};
    for (int k = 0; k<16; k++){
        if (RS232_OpenComport(k, 9600, mode) == 0){
                Close(k);
                IsPresent[k] = 1;
        }
        else {
            IsPresent[k] = 0;
        }
    }
    GUI.ConsoleOut(u8"—¬я«№: —н€т список портов");
    return 0;
}

int COM_class::Shake(void){
    RS232_cputs(V.ComPort.Number, "HANDSHAKE QUERY");
    cv::waitKey(100);

    unsigned char RXbuf[64];
    int  n = RS232_PollComport(V.ComPort.Number, RXbuf, 63);
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



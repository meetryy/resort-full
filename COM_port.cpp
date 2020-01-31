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

char CRC8(unsigned char*data, int length){
   unsigned char crc = 0x00;
   unsigned char extract;
   unsigned char sum;
   for(int i=0;i<length;i++){
      extract = *data;
      for (unsigned char tempI = 8; tempI; tempI--) {
         sum = (crc ^ extract) & 0x01;
         crc >>= 1;
         if (sum) crc ^= 0x8C;
         extract >>= 1;
      }
      data++;
   }
   return crc;
}

int COM_class::Open(int port_number){
    //port_number=0,         /* /dev/ttyS0 (COM1 on windows) */
    //V.ComPort.Speed=9600;       /* 9600 baud */
    char mode[]={'8','N','1',0};
    if(RS232_OpenComport(port_number, V.ComPort.Speed, mode)){
        GUI.ConsoleOut(u8"ќЎ»Ѕ ј: Ќевозможно открыть порт!");
        return(COM_ERROR);
    }
      else {
            V.ComPort.Connected = 1;
            V.ComPort.Number = port_number;
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
        else IsPresent[k] = 0;
    }
    GUI.ConsoleOut(u8"—¬я«№: ѕолучен список портов");
    return 0;
}

int COM_class::Test(void){
    RS232_cputs(V.ComPort.Number, "0");
    V.comTest = !V.comTest;
};

int COM_class::setHwState(uint8_t hwType, uint8_t hwID, uint8_t hwAction, uint8_t parameter){

    unsigned char dataOut[3] = {0};

    dataOut[0] = 0x80;
    dataOut[0] = dataOut[0] | (hwType << 5);
    dataOut[0] = dataOut[0] | (hwID << 3);
    dataOut[0] = dataOut[0] | (hwAction & 0x07);
    dataOut[1] = parameter;

    dataOut[2] = CRC8(&dataOut[0], 2);

    std::cout << "TX: ";

    std::printf("%02X ", static_cast<unsigned>(dataOut[0]));
    std::printf("%02X ", static_cast<unsigned>(dataOut[1]));
    std::printf("%02X\n", static_cast<unsigned>(dataOut[2]));


    RS232_SendBuf(V.ComPort.Number, dataOut, 3);

    V.comTest = !V.comTest;
    return 0;
};

int setZoneState(int* zoneArray, int arrLen){

}

void COM_class::listen(void){
    unsigned char buf[128] = {0};
    int n = RS232_PollComport(V.ComPort.Number, buf, 128);

    if(n > 0)
    {
      buf[n] = 0;   /* always put a "null" at the end of a string! */

  //    for(int i=0; i < n; i++)
   //   {
       // if(buf[i] < 32)  /* replace unreadable control-codes by dots */
       // {
       //   buf[i] = '.';
      //  }
    //  }
        std::string str(buf, buf + sizeof buf / sizeof buf[0]);
        std::string out = "--> ";
        out.append(str);
        GUI.ConsoleOut(out);
      //printf("%s\n",(char *)buf);
    }

}

int COM_class::Shake(void){
    /*
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
    */

    std::string out = "<-- preved";
    GUI.ConsoleOut(out);
    RS232_cputs(V.ComPort.Number, "preved");


}



#include <iostream>
#include <fstream>
#include <stdexcept>

#include "rs232.h"
#include "COM_port.h"
#include "variables1.h"
#include "newGUI.h"
#include "rs232.h"
#include "belt_processor.h"

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
        GUI.ConsoleOut(u8"ОШИБКА: Невозможно открыть порт COM%u!", port_number+1);
        GUI.popupError(u8"Невозможно открыть порт!");
        //GUI.popupError(u8"Невозможно открыть порт!");
        return(COM_ERROR);
    }
    else {
        RS232_flushRX(port_number);
        isOpen = 1;
        connectionOk = 0;
        V.ComPort.Number = port_number;

        GUI.ConsoleOut(u8"СВЯЗЬ: Порт COM%u открыт на %u", port_number+1, V.ComPort.Speed);
        return COM_OK;}
}

int COM_class::Close(int port_number){
    RS232_CloseComport(port_number);
    isOpen = 0;
    connectionOk = 0;
    GUI.ConsoleOut(u8"СВЯЗЬ: Порт закрыт");
}


int COM_class::closeCurrent(void){
    RS232_CloseComport(V.ComPort.Number);
    isOpen = 0;
    connectionOk = 0;
    GUI.ConsoleOut(u8"СВЯЗЬ: Порт COM%u закрыт", V.ComPort.Number+1);
}

int COM_class::List(void){
    char mode[]={'8','N','1',0};
    for (int k = 0; k<16; k++){
        if (RS232_OpenComport(k, 9600, mode) == 0){
            RS232_CloseComport(k);
            IsPresent[k] = 1;
        }
        else IsPresent[k] = 0;
    }
    GUI.ConsoleOut(u8"СВЯЗЬ: Получен список портов");
    return 0;
}


#define MSG_LEN 4

enum {  CMD_EJECT,
        CMD_BELT_RUN, CMD_BELT_SPEED, CMD_BELT_ACCEL, CMD_BELT_DIR,
        CMD_AGIT_SPEED, CMD_AGIT_ACCEL, CMD_AGIT_RUN, CMD_AGIT_DIR, CMD_AGIT_SWING, CMD_AGIT_SWING_LEN,
        CMD_VIB_RUN, CMD_VIB_SPEED,
        CMD_SCREEN_HOR_R, CMD_SCREEN_HOR_G, CMD_SCREEN_HOR_B, CMD_SCREEN_HOR_ON, CMD_SCREEN_HOR_BRT,
        CMD_SCREEN_VER_R, CMD_SCREEN_VER_G, CMD_SCREEN_VER_B, CMD_SCREEN_VER_ON, CMD_SCREEN_VER_BRT,
        CMD_ESTOP, CMD_RESET,
        CMD_BYE,
        CMD_NR};

int COM_class::sendCmd(uint8_t hwType, int16_t prarmValue){
    unsigned char dataOut[MSG_LEN] = {0};

    dataOut[0] = hwType;
    dataOut[1] |= (int8_t)prarmValue & 0xFF;
    dataOut[2] |= (int8_t)(prarmValue >> 8);
    dataOut[3] = CRC8(&dataOut[0], MSG_LEN-1);

    RS232_SendBuf(V.ComPort.Number, dataOut, 4);
    //V.comTest = !V.comTest;
    return 0;
};


void COM_class::sendZoneState(void){
    unsigned char dataOut[MSG_LEN] = {0};

    dataOut[0] = CMD_EJECT;
    for (int i=0; i<8; i++)
        dataOut[1] |= (BeltProcessor.Ejector.Zone[i].State << i);

    for (int i=8; i<15; i++)
        dataOut[2] |= (BeltProcessor.Ejector.Zone[i].State << (i - 8));

    dataOut[3] = CRC8(&dataOut[0], MSG_LEN-1);

    RS232_SendBuf(V.ComPort.Number, dataOut, 4);

    //V.comTest = !V.comTest;
}

#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

void COM_class::startThread(void){
    std::thread com_t(&COM_class::comThread, this);
    com_t.detach();
}

void COM_class::comThread(void){
    std::cout << "com thread started" << std::endl;
    while(1){
        if (isOpen) {
            COM.listen();
            if (tryShake) {
                Shake();
                tryShake = 0;
            }
        }
        else sleep_for(nanoseconds(200));
    }
}

void COM_class::listen(void){
    unsigned char buf[128] = {0};
    int n = RS232_PollComport(V.ComPort.Number, buf, 128);

    if(n > 0){
      buf[n] = 0;   /* always put a "null" at the end of a string! */
        std::string str(buf, buf + sizeof buf / sizeof buf[0]);
        GUI.ConsoleOut(std::string("Rx: " + str));

        if (!connectionOk && str.compare("startup")) tryShake = 1;

      //printf("%s\n",(char *)buf);
      RS232_flushRX(V.ComPort.Number);
    }
}

int COM_class::Shake(void){
    if (isOpen){
        GUI.ConsoleOut("Tx: " + V.ComPort.shakeQuery);
        RS232_cputs(V.ComPort.Number, V.ComPort.shakeQuery.c_str());

        sleep_for(milliseconds(V.ComPort.shakeTimeout));

        unsigned char rxBuf[32] = {0};
        int n = RS232_PollComport(V.ComPort.Number, rxBuf, 32);

        if(n > 0){
            rxBuf[n] = 0;
            std::string str(rxBuf, rxBuf + sizeof rxBuf / sizeof rxBuf[0]);

            if (str.find(V.ComPort.shakeAnswer) != std::string::npos) {
                 GUI.ConsoleOut("Handshake OK");
                 connectionOk = 1;
            }
            else {
                GUI.ConsoleOut("Handshake failed! (wrong answer)");
                GUI.popupError("Handshake failed! (wrong answer)");
                //GUI.popupError("Handshake failed! (wrong answer)");
            }

            GUI.ConsoleOut("Rx: " + str);
        }
        else {
                GUI.ConsoleOut("Handshake failed! (no Rx)");
                GUI.popupError("Handshake failed! (no Rx)");
                //GUI.popupError("Handshake failed! (no Rx)");
        }
    }
    else GUI.ConsoleOut(u8"ОШИБКА: Порт же закрыт!");
}


void COM_class::tryConnect(void){

};

void COM_class::tryGoodbye(void){
    if (isOpen){
        sendCmd(CMD_BYE, 0xBEEF);

        sleep_for(milliseconds(V.ComPort.shakeTimeout));

        unsigned char rxBuf[32] = {0};
        int n = RS232_PollComport(V.ComPort.Number, rxBuf, 32);

        if(n > 0){
            rxBuf[n] = 0;
            std::string str(rxBuf, rxBuf + sizeof rxBuf / sizeof rxBuf[0]);

            if (str.find("goodbye") != std::string::npos) {
                 GUI.ConsoleOut("Goodbye OK");
                 connectionOk = 0;

            }
            else {
                GUI.ConsoleOut("Goodbye failed! (wrong answer)");
                GUI.popupError("Goodbye failed! (wrong answer)");
                connectionOk = 0;
                //GUI.popupError("Handshake failed! (wrong answer)");
            }


            GUI.ConsoleOut("Rx: " + str);
        }
        else {
            GUI.ConsoleOut("Goodbye failed! (no Rx)");
            GUI.popupError("Goodbye failed! (no Rx)");
            connectionOk = 0;
            //GUI.popupError("Handshake failed! (no Rx)");
        }

        //GUI.ConsoleOut("Port closed");
        closeCurrent();


    }
};




#include <iostream>
#include <fstream>
#include <stdexcept>

#define RS232_BUFFERSIZE 64

enum sysStates {STATE_STARTUP, STATE_WAITING, STATE_READY, STATE_ERROR};
enum hwTypeMask {MASK_HWTYPE_MOTOR, MASK_HWTYPE_LAMP, MASK_HWTYPE_LED, MASK_HWTYPE_MISC}; // 4 max!
enum motActMask {MASK_ACT_MOTORRUN, MASK_ACT_MOTORACCEL, MASK_ACT_MOTORSPD, MASK_ACT_MOTORDIR}; // 4 max!
enum lampActMask {MASK_ACT_LAMPPWM, MASK_ACT_LAMPONOFF}; // 4 max!
enum ledActMask {MASK_ACT_LEDR, MASK_ACT_LEDG, MASK_ACT_LEDB, MASK_ACT_LEDONOFF}; // 4 max!
enum miscActMask {MASK_ACT_PING}; // 4 max!

class COM_class{
public:
    enum com_status {COM_ERROR, COM_OK};
    bool IsPresent[16];
    char TxBuffer[RS232_BUFFERSIZE];

    int Open(int port_number);
    int Close(int port_number);
    int List(void);
    int Shake(void);
    int Test(void);
    int setHwState(uint8_t hwType, uint8_t hwID, uint8_t hwAction, uint8_t parameter);
    int setZoneState(int* zoneArray, int arrLen);
    void listen(void);

};

extern COM_class COM;



typedef struct hw_t{
    struct {
        struct {
            bool run = 0;
            int microstepDivider = 1;
            float pulleyDiamMm = 33.0f;
            float speedMmS = 0.0f;
            int speedSteps = 0.0f;
        } transp;

        struct {
            bool run = 0;
            bool dir = 0;
            int microstepDivider = 1;
            float speedRevMin = 0.0f;
            int speedSteps = 0;
        } hopper;

        struct {
            bool run = 0;
            int speedPWM = 0;
            float speedRevMin = 0.0f;
        } feeder;

        struct {
            bool state[16] = {0};
        } nozzles;
    } State;


    void setMotorSpeed(int motor, long speedSteps);
    void setMotorRun(int motor, bool run);

};

extern hw_t hw;

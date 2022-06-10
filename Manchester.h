
#ifndef Manchester_H
#define Manchester_H

#include "mbed.h"
#include "DigitalIn.h"
#include "Timeout.h"
#include "InterruptIn.h"
#include "ThisThread.h"
#include "Thread.h"




struct DataFrame{

    uint8_t buffer[32];
    int rPointer;
    int wPointer;
    uint8_t dataByte;

    volatile struct {
        bool isBusy:1;
        unsigned int dataLen=8;
    } state;
};

class Manchester {

    private:

        unsigned int boudRateUs = 86; 

        Timeout rxClock;
        Timeout txClock;

        int Bit=0;

        DigitalOut tx;
        DigitalIn rx;
        InterruptIn rxInterrupt;

        struct DataFrame rxFrame;
        struct DataFrame txFrame;

        void initRx();
        void readRiseEdge();
        void readFallEdge();
        void readBit();
        void sendBit();
        void resetRead();

        public:
            Manchester(PinName rxPin, PinName txPin,int);
            int read();
            int write(uint8_t chr);

        


};

#endif

#include "Manchester.h"


void Manchester::sync(){
    t.stop();
    boudRateUsRx = 2 * (t.elapsed_time()).count();
    t.reset();
    readBit();
};

void Manchester::readRiseEdge(){

    Bit = 1;

    if(rxFrame.state.dataLen == 8){
        sync();
    }
}

void Manchester::readFallEdge(){
 
    Bit = 0;

    if(rxFrame.state.dataLen == 8){
        sync();
    } 

}

void Manchester::readBit() {
    
    if (rxFrame.state.dataLen != 0) {
        rxFrame.state.dataLen--;
        rxFrame.dataByte >>= 1;
        rxFrame.dataByte |= Bit ? 0x80 : 0x00;
    
    }  
    else{
            
            uint8_t wPointer = rxFrame.wPointer++;

            rxFrame.wPointer = rxFrame.wPointer < sizeof(rxFrame.buffer) ? rxFrame.wPointer : 0;
            
            if (rxFrame.wPointer == rxFrame.rPointer) {
                rxFrame.wPointer = wPointer;

            }
            rxFrame.buffer[wPointer] = rxFrame.dataByte;
            resetRead();           
            return;
        
    }
   

    rxClock.attach_us(callback(this, &Manchester::readBit),boudRateUsRx);    
    
    return;

};

int Manchester::read() {
 
    if (rxFrame.rPointer == rxFrame.wPointer) {
            return -1;
        }

    int c = rxFrame.buffer[rxFrame.rPointer++];
    rxFrame.rPointer = rxFrame.rPointer < sizeof(rxFrame.buffer) ? rxFrame.rPointer : 0;
    
    return c;
};

void Manchester::resetRead(){
    
    rxFrame.state.isBusy = 0;
    rxFrame.state.dataLen   = 8;

    rxInterrupt.fall(callback(this,&Manchester::initRx));
    rxInterrupt.rise(callback(this,&Manchester::initRx));
    
}


void Manchester::initRx(){

    if (rxFrame.state.isBusy == 0) {
      
        rxFrame.state.isBusy = 1;
        rxFrame.state.dataLen   = 8;

        rxInterrupt.fall(callback(this,&Manchester::readFallEdge));
        rxInterrupt.rise(callback(this,&Manchester::readRiseEdge));

        t.start();

    }

};

void Manchester::sendBit() {

    int dataBit;

    if (txFrame.state.dataLen) {
        
        dataBit = txFrame.dataByte & 0x01;

        if(txFrame.state.dataLen%2 == 0 ){
            tx.write(!dataBit);
        }else{
            tx.write(dataBit);
            txFrame.dataByte >>= 1;


        }

        txFrame.state.dataLen--;
    }
    else {

        if (txFrame.rPointer == txFrame.wPointer) {
            txFrame.state.isBusy = 0;

            return;
        }

        txFrame.dataByte = txFrame.buffer[txFrame.rPointer++];
        txFrame.rPointer = txFrame.rPointer < sizeof(txFrame.buffer) ? txFrame.rPointer : 0;
        
        
        dataBit = 0;
    }
   
    txClock.attach_us(callback(this,&Manchester::sendBit),(boudRateUsTx-5)/2);
    
};

int Manchester::write(uint8_t c, boudRate) {

    uint8_t wPointer = txFrame.wPointer++;

    boudRateUsTx = floor(1000000/boudRate)-21;


    
    txFrame.wPointer = txFrame.wPointer < sizeof(txFrame.buffer) ? txFrame.wPointer : 0;
    
    if (txFrame.wPointer == txFrame.rPointer) {
        txFrame.wPointer = wPointer;

    }
    txFrame.buffer[wPointer] = c;



    if (txFrame.state.isBusy == 0) {
        txFrame.state.isBusy = 1;
        txFrame.state.dataLen = 16;
        tx.write(!tx.read());
        
        sendBit();

        
    }
   
    return c;

};


Manchester::Manchester(PinName rxPin, PinName txPin): rxInterrupt(rxPin),tx(txPin),rx(rxPin)
{
    tx.write(1);
    rxFrame.state.isBusy = 0;
    txFrame.state.isBusy = 0;  
    rxInterrupt.fall(callback(this,&Manchester::initRx));
    rxInterrupt.rise(callback(this,&Manchester::initRx));

};


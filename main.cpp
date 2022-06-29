
#include "ThisThread.h"
#include "mbed.h"

int main()
{
   
   
    printf("sempai");
    static Manchester man(PB_2,PB_3);

    printf("sempai");
    string name = "Vlad Shkapych";

    for(int i = 0; i < name.length(); i++) {
        man.write(name[i],9600);
          
    } 
    ThisThread::sleep_for(1);

    for(int i = 0; i < name.length(); i++) {
        
        printf("%c",man.read());
        
        
    }
        

}


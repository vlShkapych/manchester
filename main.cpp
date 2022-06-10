
#include "mbed.h"
#include "Manchester.h"
#include <cstdio>
#include <string>

int main()
{
   
   

    static Manchester man(PB_2,PB_3,9600);

   
    string name = "Vlad Shkapych";

    for(int i = 0; i < name.length(); i++) {
        man.write(name[i]);
        printf("d");   
    } 
    for(int i = 0; i < name.length(); i++) {
        
        printf("%c",man.read());
        
        
    }
        

}

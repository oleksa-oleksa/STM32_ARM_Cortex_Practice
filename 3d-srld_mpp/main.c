#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

int main(void)
{
    SystemInit();
    InitSysTick();
    start_RTC();

    our_init_board();
    


    while(1){

    }
    return 0; // to make the warning stop
}

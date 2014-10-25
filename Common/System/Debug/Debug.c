#include <stdio.h>
#include "DriverInclude.h"

int fputc(int ch, FILE *f)
{
    uint8 dgbBuffer[16];
    uint32 tmpcnt, i;

    if (ch == '\n')
    {
        UARTWriteByte('\r', 1000);
        
        tmpcnt = SysTickCounter;
        for (i = 0; i < 16; i++)
        {
            dgbBuffer[i] = tmpcnt % 10;
            tmpcnt = tmpcnt / 10;
            if (tmpcnt == 0)
            {
                break;
            }
        }
        if (i < 2)
        {
            i++;
            dgbBuffer[i] = 0;
        }
        
        UARTWriteByte('[', 1000);
        i++;
        while(i != 0)
        {
            i--;
            UARTWriteByte(dgbBuffer[i]+0x30, 1000);
            if (i == 2)
            {
                UARTWriteByte('.', 1000);
            }
        }
        UARTWriteByte(']', 1000);
    }
    UARTWriteByte(ch, 1000);
}


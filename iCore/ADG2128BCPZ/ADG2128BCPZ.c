
#include "ADG2128BCPZ.h"
#include "cmsis_os.h"
#include "i2c.h"
/*
x:x0~x11
y:y0~y7
state:状态 0 Disconnect 1 Connect

adg2128_set(1,1,1) 表示 x1与y1 连接
adg2128_set(2,1,0) 表示 x2与y1 断开
*/

void adg2128_set(unsigned short x,unsigned short y,unsigned short state)
{
    uint8_t xa,yb;
    uint8_t j[2];
    uint8_t add=0x00;

    if(x<12)
    {
        add=0xe0;
                x%=12;
    }else if(x<24)
    {
        add=0xe2;
                x%=12;
    }else if(x<36)
    {
        add=0xe4;
                x%=12;
    }else if(x<48)
    {
        add=0xe6;
                x%=12;
    }else if(x<60)
    {
        add=0xe8;
                x%=12;
    }else if(x<72)
    {
        add=0xeA;
                x%=12;
    }else if(x<84)
    {
        add=0xeB;
                x%=12;
    }else if(x<96)
    {
        add=0xee;
                x%=12;
    }


    if(state==1)
    {
    xa=0x80+(0x08*x);
    yb=xa|y;
    j[0]=yb;
    j[1]=0x01;
    HAL_I2C_Master_Transmit(&hi2c2, add, &j[0], 2, 1000);
    }else if(state==0)
    {
        xa=0x08*x;
        yb=xa|y;
        j[0]=yb;
        j[1]=0x01;
    HAL_I2C_Master_Transmit(&hi2c2, add, &j[0], 2, 1000);
    }
}







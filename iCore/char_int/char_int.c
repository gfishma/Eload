#include "char_int.h"


//单个字符，转换为十六进制
//如 ‘f’ 'A' '5' == 0xf 0xA 0x5 
uint8_t char2Hex(uint8_t data)
{
    if(data >='A' && data <= 'F')
    return (data - 'A' + 0x0A);

    else if (data >= 'a' && data <='f')
    return (data - 'a' + 0x0A);

    else if (data >= '0' && data <='9')
    return (data - '0');

    else
    return 0;
}



//合并两个十六进制数 
//如0xf 0xf 合并成0xff
uint8_t twochar2Hex(uint8_t data1, uint8_t data2)
{
    return (uint8_t)((char2Hex(data1)<<4) + char2Hex(data2));
}




uint8_t charnum(uint8_t data[], uint8_t len)
{
    uint16_t num=0;
    for (size_t i = 0; i < len; i++)
    {
    if(data[i] >= '0' && data[i] <= '9')
    {
        num=num*10+data[i]-'0';  
        return num;
    }
    }
    return 0;
}




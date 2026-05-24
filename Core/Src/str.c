/*
 * str.c
 *
 *  Created on: 2020年5月29日
 *      Author: timecy
 */

#include "str.h"

//
void ustr_substr();

// src			字符串源
// character	待检测的字符
// 返回字符的个数
unsigned short str_CharQty(const char *str, char character)
{
	unsigned short count = 0;
	for(unsigned short i = 0; str[i]; i++)
		if(str[i] == character) count++;
	return count;
}

//// 返回
//char* ustr_split()
//{
//
//}
//----------------------------------------------------------------------------
//pSrc为源字符串
//pEnd 为字符串结束地址
//pResult存放结果变量的指针
//返回值为找到数值后的下一字节，返回NULL为没找到。
char* str_GetHex(char* pSrc, char* pEnd, long* pResult)
{
    char*    pStart;
    unsigned long        Data32 = 0;
    unsigned short        BitCnt = 0;

    if(pSrc == NULL)
    {    return NULL;}

    pStart = pSrc;

    while(pStart < pEnd)
    {
        if((*pStart >='0')&&(*pStart <= '9'))
        {
            Data32 = Data32<<4;
            Data32 |= (((*pStart) - 0x30) & 0x0F);
            BitCnt ++;
        }
        else if((*pStart >= 'A') && (*pStart <= 'F'))
        {
            Data32 = Data32<<4;
            Data32 |= (((*pStart) - 0x37) & 0x0F);
            BitCnt ++;
        }
        else if((*pStart >= 'a') && (*pStart <= 'f'))
        {
            Data32 = Data32<<4;
            Data32 |= (((*pStart) - 0x57) & 0x0F);
            BitCnt ++;
        }
        else if((*pStart == 'X') || (*pStart == 'x'))
        {//
            Data32 = 0;
            BitCnt = 0;
        }
        else//Is not a Hex Code
        {
            if(BitCnt)
            {
                *pResult = Data32;
                return pStart;
            }
            else
            {    return NULL;}
        }
        pStart++;
    }
    if(BitCnt)
    {
        *pResult = Data32;
        return pStart;
    }
    else
    {
        return NULL;
    }
}

//----------------------------------------------------------------------------
//pSrc为源字符串
//pEnd 为字符串结束地址
//pResult存放结果变量的指针
//返回值为找到数值后的下一字节，返回NULL为没找到。
char* str_GetDec(char* pSrc, char* pEnd, long* pResult)
{
    char*    pStart;
    long    Data32 = 0;
    unsigned short       BitCnt = 0;
    unsigned char        Signed = 0;

    if(pSrc == NULL)
    {    return NULL;}

    pStart = pSrc;

    while(pStart < pEnd)
    {
        if((*pStart == '-')&&(BitCnt == 0))
        {
            Signed = 1;
        }
        else if((*pStart >='0')&&(*pStart <= '9'))
        {
            Data32 *= 10;
            Data32 += (((*pStart) - 0x30) & 0x0F);
            BitCnt ++;
        }
        else//Is not a Dec Code
        {
            if(BitCnt)
            {
                if(Signed == 1)
                {    *pResult = -Data32;}
                else
                {    *pResult = Data32;}
                return pStart;
            }
            else
            {
                return NULL;
            }
        }
        pStart++;
    }
    if(BitCnt)
    {
        if(Signed == 1)
        {    *pResult = -Data32;}
        else
        {    *pResult = Data32;}
        return pStart;
    }
    else
    {
        return NULL;
    }
}
//----------------------------------------------------------------------------
//pSrc为源字符串
//pEnd 为字符串结束地址
//pResult存放结果变量的指针
//返回值为找到数值后的下一字节，返回NULL为没找到。
char* str_GetHexDec(char* pSrc, char* pEnd, long* pResult)
{
    char* pStart;
    unsigned char isHex = 0;

    if(pSrc == NULL)
    { return NULL; }

    pStart = pSrc;


    while(pStart < pEnd)
    {
    	if(isCharCode(*pStart))
    	{
			if(*pStart == 'x' || *pStart == 'X')
			{
				isHex = 1;
				break;
			}
    	}
    	else
    	{
    		isHex = 0;
    		break;
    	}
    	pStart++;
    }

	pStart = pSrc;

    if(isHex)
    {
    	return str_GetHex(pStart, pEnd, pResult);
    }
    else
    {
    	return str_GetDec(pStart, pEnd, pResult);
    }
}

void str_deSpace(char *str)
{
	char *p=str;
	int i=0;
	while(*p)
	{
		if(*p!=' ')
			str[i++]=*p;
		p++;
	}
	str[i]='\0';
}

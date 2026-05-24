#include "Tftlcd.h"
#include "spi.h"
#include "gpio.h"
#include "font.h"

#define LCD_TOTAL_BUF_SIZE	(240*240*2)
#define LCD_Buf_Size 1152
static uint8_t lcd_buf[LCD_Buf_Size];

u16	POINT_COLOR = GREEN;	//默认画笔颜色
u16	BACK_COLOR = BLACK;		//默认背景颜色


//LCD批量写数据
static void LCD_SPI_Send(u8 *data, u16 size)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3,  data, size, 1000);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

/**
 * @brief	写指令到LCD
 *
 * @param   cmd		需要发送的指令
 *
 * @return  void
 * 
 * 
 * 
 * 
 */

static u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}


static void LCD_Write_Cmd(u8 cmd)
{
    LCD_WR(0);
    LCD_SPI_Send(&cmd, 1);
}

/**
 * @brief	写数据到LCD
 *
 * @param   data		需要发送的数据
 *
 * @return  void
 */
static void LCD_Write_Data(u8 data)
{
    LCD_WR(1);
    LCD_SPI_Send(&data, 1); 
}

//LCD初始化
void LCD_Init(void)
{
    LCD_PWR(0);
    LCD_RST(0);
    HAL_Delay(150);
    LCD_RST(1);
    HAL_Delay(500);
    LCD_CS(1);
     /* Sleep Out */
    LCD_Write_Cmd(0x11);
    /* wait for power stability */
    HAL_Delay(120);

    /* Memory Data Access Control */
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x00);

    /* RGB 5-6-5-bit  */
    LCD_Write_Cmd(0x3A);
    LCD_Write_Data(0x65);

    /* Porch Setting */
    LCD_Write_Cmd(0xB2);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x33);
    LCD_Write_Data(0x33);

    /*  Gate Control */
    LCD_Write_Cmd(0xB7);
    LCD_Write_Data(0x72);

    /* VCOM Setting */
    LCD_Write_Cmd(0xBB);
    LCD_Write_Data(0x3D);   //Vcom=1.625V

    /* LCM Control */
    LCD_Write_Cmd(0xC0);
    LCD_Write_Data(0x2C);

    /* VDV and VRH Command Enable */
    LCD_Write_Cmd(0xC2);
    LCD_Write_Data(0x01);

    /* VRH Set */
    LCD_Write_Cmd(0xC3);
    LCD_Write_Data(0x19);

    /* VDV Set */
    LCD_Write_Cmd(0xC4);
    LCD_Write_Data(0x20);

    /* Frame Rate Control in Normal Mode */
    LCD_Write_Cmd(0xC6);
    LCD_Write_Data(0x0F);	//60MHZ

    /* Power Control 1 */
    LCD_Write_Cmd(0xD0);
    LCD_Write_Data(0xA4);
    LCD_Write_Data(0xA1);

    /* Positive Voltage Gamma Control */
    LCD_Write_Cmd(0xE0);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2B);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x54);
    LCD_Write_Data(0x4C);
    LCD_Write_Data(0x18);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x0B);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x23);

    /* Negative Voltage Gamma Control */
    LCD_Write_Cmd(0xE1);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2C);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x44);
    LCD_Write_Data(0x51);
    LCD_Write_Data(0x2F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x20);
    LCD_Write_Data(0x23);

    /* Display Inversion On */
    LCD_Write_Cmd(0x21);

    LCD_Write_Cmd(0x29);

    LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    LCD_Clear(BLACK);
    LCD_PWR(1);
}

void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_Write_Cmd(0x2a);
    LCD_Write_Data(x1 >> 8);
    LCD_Write_Data(x1);
    LCD_Write_Data(x2 >> 8);
    LCD_Write_Data(x2);

    LCD_Write_Cmd(0x2b);
    LCD_Write_Data(y1 >> 8);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2 >> 8);
    LCD_Write_Data(y2);

    LCD_Write_Cmd(0x2C);
}

void LCD_Clear(u16 color)
{
    u16 i, j;
    u8 data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    for(j = 0; j < LCD_Buf_Size / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    LCD_WR(1);

    for(i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_Buf_Size); i++)
    {
        LCD_SPI_Send(lcd_buf, (uint16_t)LCD_Buf_Size);
    }
}

void LCD_ShowChar(u16 x, u16 y, char chr, u8 size)
{
    u8 temp, t1, t;
    u8 csize;		//得到字体一个字符对应点阵集所占的字节数
    u16 colortemp;
    u8 sta;

    chr = chr - ' '; //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    if((x > (LCD_Width - size / 2)) || (y > (LCD_Height - size)))	return;

    LCD_Address_Set(x, y, x + size / 2 - 1, y + size - 1);//(x,y,x+8-1,y+16-1)

    if((size == 16) || (size == 32) )	//16和32号字体
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++)
        {
            if(size == 16)temp = asc2_1608[(u16)chr][t];	//调用1608字体
            else if(size == 32)temp = asc2_3216[(u16)chr][t];	//调用3216字体
            else return;			//没有的字库

            for(t1 = 0; t1 < 8; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;
                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

	else if  (size == 12)	//12号字体
	{
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++)
        {
            temp = asc2_1206[(u16)chr][t];

            for(t1 = 0; t1 < 6; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;
                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
	
    else if(size == 24)		//24号字体
    {
        csize = (size * 16) / 8;

        for(t = 0; t < csize; t++)
        {
            temp = asc2_2412[(u16)chr][t];

            if(t % 2 == 0)sta = 8;
            else sta = 4;

            for(t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;
                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
}

void LCD_Write_HalfWord(const u16 da)
{
    u8 data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;

    LCD_WR(1);
    LCD_SPI_Send(data, 2);
}

void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, char *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }

        if(y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size);
        x += size / 2;
        p++;
    }
}

void lcd_show_chinese(uint16_t x, uint16_t y, char ch, uint8_t font_size)
{
    uint16_t i, j;
    uint16_t x_pos, y_pos, size, font_width, font_height;
    uint8_t *font_ptr;
    uint8_t bit_width, temp;
    u16 colortemp;
	
    if((x > (LCD_Width - font_size)) || (y > (LCD_Height - font_size)))	{
        return;
    }
	
    x_pos = x;
    y_pos = y;
    font_height = font_size;
    font_width = font_size;
    bit_width = 8;
    size = (font_width / 8 + ((font_width % 8) ? 1 : 0)) * font_height;
    
	font_ptr = (uint8_t*)&ch_3232[(u16)ch];

    for (i = 0; i < size; i++) {
        temp = *(font_ptr + i);
        for (j = 0; j < bit_width; j++) {
            if(temp & 0x80) colortemp = POINT_COLOR;
            else colortemp = BACK_COLOR;
            LCD_Address_Set(x_pos, y_pos, x_pos, y_pos);
            LCD_Write_HalfWord(colortemp);
            temp <<= 1;
            x_pos++;
        }
        if (x_pos >= (x + font_width)) {
            y_pos++;
            x_pos = x;
        }
    }
}

void lcd_clear_part(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1,uint16_t color)
{
    uint16_t i, j;
    uint16_t x_pos, y_pos, x1_pos, y1_pos;
    u16 colortemp;
    
    colortemp = color;
    x_pos = x;
    y_pos = y;
    x1_pos = x1;
    y1_pos = y1;    

    for (i = 0; i < y1_pos; i++) {
        for (j = 0; j < x1_pos; j++) {
            LCD_Address_Set(x_pos, y_pos, x_pos, y_pos);
            LCD_Write_HalfWord(colortemp);
            x_pos++;
        }
        if (x_pos >= x1_pos) {
            y_pos++;
            x_pos = x;
        }
    }
}

void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode)LCD_ShowChar(x + (size / 2)*t, y, '0', size);
                else
                    LCD_ShowChar(x + (size / 2)*t, y, ' ', size);

                continue;
            }

            else enshow = 1;
        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size);
    }
}

void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size);
                continue;
            }

            else enshow = 1;
        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size);
    }
}


void OLED_ShowFloatNumber(u8 x,u8 y, float num,u8 char_size)
{
    unsigned char zheng_len,xiao_len,polarity_flag;
    int zheng,xiao;
    zheng_len=0;

    if(num>0)

    polarity_flag=1;
    else
    {
        polarity_flag=0;
        num=-num;
    }

    zheng=(int)num;
    xiao=((num-zheng)*100)/1;


    while(zheng)
    {
        zheng_len++;
        zheng/=10;
    }
    xiao_len=2;
    zheng=(int)num;
    if(polarity_flag)
    {
        if(char_size==16)
        {
            LCD_ShowNum(x,y,zheng,zheng_len,char_size);
            LCD_ShowChar(x+zheng_len*8,y,'.',char_size);
            LCD_ShowNum(x+(zheng_len+1)*8,y,xiao,xiao_len,char_size);

        }
        else
        {
            LCD_ShowNum(x,y,zheng,zheng_len,char_size);
            LCD_ShowChar(x+zheng_len*6,y,'.',char_size);
            LCD_ShowNum(x+(zheng_len+1)*6,y,xiao,xiao_len,char_size);

        }
    }
    else
    {
           LCD_ShowChar(x+zheng_len*6,y,'-',char_size);
           if(char_size==16)
           {

            LCD_ShowNum(x+8,y,zheng,zheng_len,char_size);
            LCD_ShowChar(x+(zheng_len+1)*8,y,'.',char_size);
            LCD_ShowNum(x+(zheng_len+2)*8,y,xiao,xiao_len,char_size);

           }else
           {
            LCD_ShowNum(x+6,y,zheng,zheng_len,char_size);
            LCD_ShowChar(x+(zheng_len+1)*6,y,'.',char_size);
            LCD_ShowNum(x+(zheng_len+2)*6,y,xiao,xiao_len,char_size);
           }

    }

}





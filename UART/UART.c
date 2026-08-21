// 2024/12/7 morning modify (clear thinking)
#include <reg51.h>
#include <intrins.h>
#include <string.h>

#define LeftShift(x) _crol_(x, 1) // 重新定義 左旋函式
#define LCD_Data P2
sbit LCD_RS = P1 ^ 0; //定義LCD指令資料接腳
sbit LCD_RW = P1 ^ 1; //定義LCD讀取寫入接腳
sbit LCD_E = P1 ^ 2;  //定義LCD致能接腳

void delay1ms(int times);
void delay(unsigned int t);
void delay_us(unsigned int t);
void write_data_LCD(char WDLCD);
void write_command_LCD(char WCLCD);
void LCD_init(void);
unsigned char getKB(unsigned char var);
void LCD_show(char row, char column, unsigned char *str);

char row0 = 0 , row1 = 0;  
unsigned char boom[] = {05, 11, 12, 15, 25, 33, 43, 50, 57, 60, 67, 70, 74, 75, 77};
unsigned char HIT_goal = 0;

void main(void)
{
    unsigned char kb_buf, column, tmp, value;
    unsigned char trans_flag=0;
		unsigned char real_number;
		int in_flag=1;
	
    LCD_init();  //LCD初始化

    IE = 0x90;   //設定允許串列傳輸中斷
    SCON = 0x50; //設定串列傳輸工作於模式1, 可接收資料
    TMOD = 0x20; //設定TIMER1工作於模式2(autoo reload模式)
    TH1 = 253;   //設定auto reload值為253(在石英震盪器為11.059MHz時, buad rate為9600bps)
    TR1 = 1;     //啟動TIMER1

    while (1)
    {
        kb_buf = 0xFE;
        for (column = 0; column < 4; column++)
        {
            P0 = kb_buf;
            tmp = P0;
            if ((tmp & 0xF0) != 0xF0) 
            {
                delay(1); 
                tmp = P0;
                if ((tmp & 0xF0) != 0xF0)  
                {
									  write_command_LCD( row0 + 0x80 );
										value = getKB(tmp);
										if(value == 15){										// clean
												row0 = 0;
												real_number = 0;
												write_command_LCD(0x01);
												write_command_LCD(0x80);
										}
										else if(value>=0 && value<=7){			//choose number
												if(row0<2){
														write_data_LCD(value + '0');
														if(row0==0){
																real_number  = value *10;
																row0++;
														}
														else if(row0==1){
																real_number +=value;
																row0++;
														}
												}
										}
										else if(value == 14){								// Transfer button
												row0=0;
												SBUF = real_number;
												real_number=0;
										}
                }
								do
								{
										tmp = P0;
								}while ( (tmp & 0xF0) != 0xF0 ); 
            } 
            kb_buf = LeftShift(kb_buf); 
        }
    } 
}

// UART的中斷函式, 使用中斷向量4
void uart_isr(void) interrupt 4
{
		unsigned int a;
		unsigned char MISS_flag=0;
		unsigned char boom_size = sizeof(boom)/sizeof(boom[0]);
		unsigned char in_value;
		unsigned char state;
	
    if (TI == 1) 
    {
        TI = 0; 
    }
    if (RI == 1) 
    {
        RI = 0; 
				in_value = SBUF;
				if(in_value>=0 && in_value<=77){
						write_command_LCD(0xc0);
						write_data_LCD(in_value/10 + '0');
						write_data_LCD(in_value%10 + '0');
						delay1ms(1000);
						write_command_LCD(0xc0);
						write_data_LCD(' ');
						write_data_LCD(' ');
					
						for(a=0;a<boom_size;a++){
								if(boom[a] == in_value){
										state='X';								//state HIT
										HIT_goal++;
										if(HIT_goal==boom_size){
												state='Z';						//state WIN
										}
										break;
								}
								else{
										state='Y';								//state MISS
								}

						}
						SBUF=state;												
				}
				
				if(in_value=='X'){
						write_command_LCD(0x80);
						write_data_LCD(' ');
						write_data_LCD(' ');
						LCD_show(1,0,"HIT");
						delay1ms(1000);
						write_command_LCD(0x01);
				}
				else if(in_value=='Y'){
						write_command_LCD(0x80);
						write_data_LCD(' ');
						write_data_LCD(' ');
						LCD_show(1,0,"MISS");
						delay1ms(1000);
						write_command_LCD(0x01);
				}
				if(in_value=='Z'){
						LCD_show(1,0,"WIN");
						while(1);
				}
    }
}

void LCD_show(char row, char column, unsigned char *str)
{
    char i = 0;
    write_command_LCD((row * 0x40) + 0x80 + column);
    while (str[i] != '\0')
        write_data_LCD(str[i++]);
}

unsigned char getKB(unsigned char var)
{
	unsigned char tmp;

	switch (var)
	{
	//column1	P0.0
	case 0xEE: tmp = 12; break;		//1110 1110 => P0.4 P0.0 => C
	case 0xDE: tmp = 8;  break;		//1101 1110 => P0.5 P0.0 => 8
	case 0xBE: tmp = 4;  break;		//1011 1110 => P0.6 P0.0 => 4
	case 0x7E: tmp = 0;  break;		//0111 1110 => P0.7 P0.0 => 0

	//column2	P0.1
	case 0xED: tmp = 13; break;		//1110 1101 => P0.4 P0.1 => D
	case 0xDD: tmp = 9;  break;		//1101 1101 => P0.5 P0.1 => 9
	case 0xBD: tmp = 5;  break;		//1011 1101 => P0.6 P0.1 => 5
	case 0x7D: tmp = 1;  break;		//0111 1101 => P0.7 P0.1 => 1

	//column3	P0.2
	case 0xEB: tmp = 14; break;		//1110 1011 => P0.4 P0.2 => E
	case 0xDB: tmp = 10; break;		//1101 1011 => P0.5 P0.2 => A
	case 0xBB: tmp = 6;  break;		//1011 1011 => P0.6 P0.2 => 6
	case 0x7B: tmp = 2;  break;		//0111 1011 => P0.7 P0.2 => 2

	//column4	P0.3
	case 0xE7: tmp = 15; break;		//1110 0111 => P0.4 P0.3 => F
	case 0xD7: tmp = 11; break;		//1101 0111 => P0.5 P0.3 => B
	case 0xB7: tmp = 7;  break;		//1011 0111 => P0.6 P0.3 => 7
	case 0x77: tmp = 3;  break;		//0111 0111 => P0.7 P0.3 => 3

	default: tmp = 16; break;
	}
	return tmp;
}

void delay1ms(int times){
		int i,j;
		for(i=0;i<times;i++)
			for(j=0;j<194;j++);
}

void delay(unsigned int t)
{
    unsigned int i;
    while (t--)
    {
        i = 255;
        while (i--);
    }
}

void delay_us(unsigned int t)
{
    while (t--);
}

//寫資料到LCD副程式
void write_data_LCD(char WDLCD)
{
    LCD_Data = WDLCD;
    LCD_RS = 1;  // RS=1 資料暫存器
    LCD_RW = 0;  // RW=0 寫入
    LCD_E = 1;
    delay_us(100);  //延遲等訊號穩定
    LCD_E = 0;
    delay_us(100);  //延遲等訊號處裡完畢
}

//寫指令到LCD副程式
void write_command_LCD(char WCLCD)
{
    LCD_Data = WCLCD;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 1;
    delay_us(100);  //延遲等訊號穩定
    LCD_E = 0;
    delay_us(100);  //延遲等訊號處裡完畢
}

// LCD初始化
void LCD_init(void)
{
    write_command_LCD(0x38);  //設定LCD介面8位元，2行，5*7字型
    write_command_LCD(0x06);  //設定寫資料至LCD時，顯示不要動，游標向右移動
    write_command_LCD(0x0F);  //設定螢幕顯示及游標閃爍
    write_command_LCD(0x01);  //清除顯示器
    delay_us(5000);
}

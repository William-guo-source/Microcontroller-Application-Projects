// 2024/11/30 afternoon
#include <reg51.h>
#include <intrins.h>
#include <string.h>

#define LeftShift(x) _crol_(x,1);    //LeftShift(x) for the scan line 
#define DBPORT P2
sbit RS = P1^0;		// ???? P1.0
sbit RW = P1^1;		// P1.1
sbit E = P1^2;		// P1.2

void adjtime();
void set_timer0(int num);
void delay();
void delay1ms(int times);
unsigned char getIntData(unsigned char var);
void LCD_init();
void LCD_CMD(char cmd);
void LCD_DATA(char data1);

unsigned char run_flag;											 //run time or not
unsigned char buffer[] = {0, 0, 0, 0, 0, 0}; //hh:mm:ss
unsigned char count;                		     //for timer usingg											
unsigned char second, minute, hour;	       	 //been read by interruptt so set it global variable
unsigned char blink;											   //adjtime function, blink it or not
unsigned char first_flag;									   //for first time blink no redundant second
bit time_adjust_flag = 0;

void main(void)
{
		IE = 0x83;		  //1000 0011 open INT0 TIMER0 
		TMOD = 0x00;		//TIMER 0 mode-0(13-bitcounter)
		IP = 0x02;		  //0000 0010 TIMER 0 high order priority
	
		TR0=1;							//start bitt for timer 0
		set_timer0(5000);		//set TH0 TL0 
		count=200;					//repeat 200 times
	
		LCD_init(); //Initial LCD
		
		second=0; minute=0; hour=0;
		first_flag = 1;			//first time blink
		adjtime();					//settime function
		
		LCD_CMD(0x01);
		while (1)
		{
			if(time_adjust_flag){
				run_flag = 0;
				time_adjust_flag = 0;
				adjtime();
			}
			
			buffer[0] = hour / 10;		//instantaneous updating value
			buffer[1] = hour % 10;		//for hour minute second
			buffer[2] = minute / 10;
			buffer[3] = minute % 10;
			buffer[4] = second / 10;
			buffer[5] = second % 10;
			
			 if(second>59){						//time limit
					minute++; second=0;
			 }
			 if(minute>59){
					hour++; minute=0;
			 }
			 if(hour>23){
					second=0; minute=0; hour=0;
			 }
			 
			 LCD_CMD(0x84);						//locate the first place
			 LCD_DATA(0x30+ buffer[0]);
			 LCD_DATA(0x30+ buffer[1]);
			 LCD_DATA(':');
			 
			 LCD_DATA(0x30+ buffer[2]);
			 LCD_DATA(0x30+ buffer[3]);
			 LCD_DATA(':');
			 
			 LCD_DATA(0x30+ buffer[4]);
			 LCD_DATA(0x30+ buffer[5]);
		}
}

void adjtime(){
	int pos;								//left <==> right
	int buffer_edit_mode;		//edit0:hour -> edit1:minute -> edit2:second
	
	buffer_edit_mode=0;			//adjust from clock
	pos=0;									//adjust from tenth digit
	run_flag=0;							//stop the clock

	while(run_flag==0){
		 unsigned char kb_scan, column, tmp;
		 unsigned char i;
		 kb_scan = 0xFE;
		 for (column = 0; column < 4; column++)
		 {
				 P0 = kb_scan;
				 tmp = P0;
				 while ((tmp & 0xF0) != 0xF0)			// P0.7~P0.4 all 1, no press
				 {
						 delay();											//debouce
						 tmp = P0;
						 if ((tmp & 0xF0) != 0xF0)		//confirm again, if it press or not
						 {
								 int realnum = getIntData(tmp);
								 if(realnum>=0 && realnum<=9){		//input number	
										 buffer[pos + buffer_edit_mode*2]=realnum;
										 pos++;
										 if(pos>1) pos=0;
								 }
								 else if(realnum==11){						//clear
										 pos=0;
										 buffer_edit_mode=0;
										 for(i=0;i<6;i++) buffer[i]=0;
								 }
								 else if(realnum==10){						//Enter
										 run_flag=1; first_flag=0;
								 }
								 else if(realnum==12){						//Set
										 pos=0;
										 buffer_edit_mode++;
										 if(buffer_edit_mode>2){			//adjust next to second, start the runflag
												run_flag=1; first_flag=0;	//start time
										 }
								 }
						 }
						 do
						 {
							tmp = P0;
						 } while ((tmp & 0xF0) != 0xF0);     //if it is been released or not
					 }
					 kb_scan = LeftShift(kb_scan);				 //scan change 1110 1101 1011 0111
			 }
		 
			 LCD_CMD(0x84);												//locate to first place
			 if(blink==1){
						if(buffer_edit_mode==0){				//blink for hour
								 LCD_DATA(' ');
								 LCD_DATA(' ');
								 LCD_DATA(':');
							
							   LCD_DATA(0x30+buffer[2]);
								 LCD_DATA(0x30+buffer[3]);
								 LCD_DATA(':');
							
							   LCD_DATA(0x30+buffer[4]);
								 LCD_DATA(0x30+buffer[5]);
						}
						else if(buffer_edit_mode==1){	  //blink for minute
								 LCD_DATA(0x30+buffer[0]);
								 LCD_DATA(0x30+buffer[1]);
								 LCD_DATA(':');
							
							   LCD_DATA(' ');
								 LCD_DATA(' ');
								 LCD_DATA(':');
							
							   LCD_DATA(0x30+buffer[4]);
								 LCD_DATA(0x30+buffer[5]);
						}
						else if(buffer_edit_mode==2){		//blink for second
								 LCD_DATA(0x30+buffer[0]);
								 LCD_DATA(0x30+buffer[1]);
								 LCD_DATA(':');
							
							   LCD_DATA(0x30+buffer[2]);
								 LCD_DATA(0x30+buffer[3]);
								 LCD_DATA(':');
							
							   LCD_DATA(' ');
								 LCD_DATA(' ');
						}
			 }
			 else{																//all appear
					 LCD_DATA(0x30+buffer[0]);
					 LCD_DATA(0x30+buffer[1]);
					 LCD_DATA(':');
							
					 LCD_DATA(0x30+buffer[2]);
					 LCD_DATA(0x30+buffer[3]);
					 LCD_DATA(':');
							
					 LCD_DATA(0x30+buffer[4]);
					 LCD_DATA(0x30+buffer[5]);
			 }
			 
			 if(run_flag == 1){													//set all adjusted time
						second = buffer[4]*10 + buffer[5];		//set the whole second
						minute = buffer[2]*10 + buffer[3];		//set the whole minute
						hour   = buffer[0]*10 + buffer[1];		//set the whole hour
			 }
	}
}

void int0(void) interrupt 0					//INT0 interruptt function
{
	time_adjust_flag = 1;
}

void timer0_isr(void) interrupt 1		//Timer0 interruptt function
{										
	set_timer0(5000);
	if (--count == 0)	  //judge count 200 times or not, 200*5000us=1s           
	{
		count = 200;		//redefine value 200
		LCD_CMD(0x01);		//clear LCD
		LCD_CMD(0x84);
		second++;		    //times up, second plus 1

		if(blink){			//blink control
			blink=0;
			if(first_flag) second=0;
		}
		else{
			blink=1;
			if(first_flag) second=0;
		}
	}
}

void set_timer0(int num){
	TH0 = (8192 - num) / 32;	//for high byte
	TL0 = (8192 - num) % 32;	//for low  byte
}

void delay(){
	int i;
	for(i = 0; i < 100; i++);
}

void delay1ms(int times){
		int i,j;
		for(i=0;i<times;i++)
			for(j=0;j<194;j++);
}

unsigned char getIntData(unsigned char var)
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

void LCD_init(void)
{
		LCD_CMD(0x38); 
		LCD_CMD(0x06); 
		LCD_CMD(0x0F); 
		LCD_CMD(0x01); 
		delay1ms(5);
}

void LCD_CMD(char cmd){
		char i;
		DBPORT = cmd;
		RS = 0;RW = 0; E = 1;
		for(i=0;i<100;i++);
		RS = 0;RW = 0; E = 0;
}

void LCD_DATA(char data1){
		char i;
		DBPORT = data1;
		RS = 1;RW = 0; E = 1;
		for(i=0;i<100;i++);
		RS = 1;RW = 0; E = 0;
}
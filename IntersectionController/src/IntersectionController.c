#include "LCD_comm.h"


//Main 'File' I/O write location:



typedef struct{

	int states;

} app_data;

int main(int argc, char *argv[])
{


	app_data * td = {NULL};

	pthread_t output;
	void *retval;

	pthread_create(&output,NULL,LCD_Control,&td);

	pthread_join(output,&retval);

}


void * LCD_Control(void * data)
{
	//up	0x5E
	//left 0x7F
	//right 0x7E

	char b1[] = "N:______P_E:______P_";
	char b2[] = "S:______P_W:______P_";

	b1[2] = b2[2] = b1[12] = b2[12] = (char)0x7F;
	b1[4] = b2[4] = b1[14] = b2[14] = (char)0x5E;
	b1[6] = b2[6] = b1[16] = b2[16] = (char)0x7E;

	app_data * td = (app_data*) data;


	int LCD_file;
	int success = openLCD(&LCD_file);

	if (success == 0)
	{
		printf("Open Success");
		writeLCD(0,0,b1,&LCD_file);
		writeLCD(1,0,b2,&LCD_file);

	}
}

/* Test program: Waves
 *
 *  * LCD_Main.c
 *
 *  Created on: 24Sep.,2019
 *      Author: Adem Ahmet Karakaya


	char c[] = ".-.   ";
	char d[] = "   '-'";
	char a[] = "0";
	char b[] = "0";

	int j = 0;

	int succ = openLCD(&LCD_file);

	printf("Open Success == %d",succ);

/*	writeLCD(0,0,"RMIT_BBB_v5_03        ", &LCD_file);
	writeLCD(1,0,"192.168.201.50        ", &LCD_file);
*/



/*	while(1)
	{
		a[0] = c[0];
		b[0] = d[0];
		writeLCD(0,0,a,&LCD_file);
		writeLCD(0,0,b,&LCD_file);

		for (j = 1; j < 20;j++)
		{
			a[0] = c[j%7];
			b[0] = d[j%7];
			writeLCD(0,j,a,&LCD_file);
			writeLCD(1,j,b,&LCD_file);
			usleep(100000);

		}
		for (j = 1; j < 20;j++)
		{
			a[0] = c[j%7];
			b[0] = d[j%7];
			writeLCD(0,19-j,a,&LCD_file);
			writeLCD(1,19-j,b,&LCD_file);
			usleep(100000);
		}

	}
*/





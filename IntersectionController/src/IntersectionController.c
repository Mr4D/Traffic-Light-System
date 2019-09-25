/*
 * LCD_Main.c
 *
 *  Created on: 24Sep.,2019
 *      Author: Adem Ahmet Karakaya
 */

#include "LCD_comm.h"

int main(int argc, char *argv[])
{
	//char c[] = ".-.   ";
	//char d[] = "   '-'";
	//char a[] = "0";
	//char b[] = "0";

	//int j = 0;
	int open_success = openLCD();

	printf("Open Success == %d",open_success);

	writeLCD(0,0,"RMIT_BBB_v5_03      ");
	writeLCD(1,0,"192.168.201.50        ");

}
//
//	while(1)
//	{
//		a[0] = c[0];
//		b[0] = d[0];
//		writeLCD(0,0,a);
//		writeLCD(0,0,b);
//
//		for (j = 1; j < 20;j++)
//		{
//			a[0] = c[j%7];
//			b[0] = d[j%7];
//			writeLCD(0,j,a);
//			writeLCD(1,j,b);
//			usleep(100000);
//
//		}
//		for (j = 1; j < 20;j++)
//		{
//			a[0] = c[j%7];
//			b[0] = d[j%7];
//			writeLCD(0,19-j,a);
//			writeLCD(1,19-j,b);
//			usleep(100000);
//		}
//
//	}
//
//	//printf("Write Success == %d",write_success);
//
//}

//issues:
/*
 * can't keep opening file, need to check
 * Screwy when overwriting single character
 *
 *
 */

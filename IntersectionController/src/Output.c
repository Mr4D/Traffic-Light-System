#include "Output.h"

/*
 * Revised I2C LCD writing.
 *
 * A collection of functions for initialising, configuring, reading and writing the LCD screen.
 *
 * Method of Use:
 * 		- openLCD to open the LCD 'file' I/O
 * 		- writeLCD to write to the LCD display
 * 		||||||||||Primitive Functions||||||||||||||||
 * 		- I2cWrite to write configuration data accordingly
 *		- Initialise_LCD initialises the LCD screen with a list of I2C writes
 * 		- SetCursor sets the cursor data.
 */

/*
 * Adem Karakaya || s36031133 || 24.09.2019
 * Personal Notes:
 *
 * 		- 20 Characters Max
 * 		- Can update characters individually
 *
 */


// Writes to I2C
int  I2cWrite_(int *fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData)
{
	/* Writes anything (including control data) to the I2C via packets.
	 *
	 *
	 *
	 *
	 */
	i2c_send_t hdr;
    iov_t sv[2];
    int status, i;

    uint8_t LCDpacket[21] = {};  // limited to 21 characters  (1 control bit + 20 bytes)

    // set the mode for the write (control or data)
    LCDpacket[0] = mode;  // set the mode (data or control)

	// copy data to send to send buffer (after the mode bit)
	for (i=0;i<NbData+1;i++)
		LCDpacket[i+1] = *pBuffer++;

    hdr.slave.addr = Address;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = NbData + 1;  // 1 extra for control (mode) bit
    hdr.stop = 1;

    SETIOV(&sv[0], &hdr, sizeof(hdr));
    SETIOV(&sv[1], &LCDpacket[0], NbData + 1); // 1 extra for control (mode) bit
      // int devctlv(int filedes, int dcmd,int sparts, int rparts, const iov_t *sv, const iov_t *rv, int *dev_info_ptr);
    status = devctlv(*fd,DCMD_I2C_SEND, 2,0,sv,NULL,NULL);

    //if (status != EOK)
    	//printf("status = %s\n", strerror ( status ));

    return status;
}


void SetCursor(int *fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column)
{
	/* Sets cursor
	 * Don't use
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 */
	uint8_t position = 0x80; // SET_DDRAM_CMD (control bit)
	uint8_t rowValue = 0;
	uint8_t	LCDcontrol = 0;
	if (row == 1)
		rowValue = 0x40;     // memory location offset for row 1
	position = (uint8_t)(position + rowValue + column);
	LCDcontrol = position;
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
}


void Initialise_LCD (int *fd, _Uint32t LCDi2cAdd)
{
	/* This will initialise the LCD.
	 * DON'T USE THIS
	 *
	 *
	 *
	 *
	 *
	 *
	 */

	uint8_t	LCDcontrol = 0x00;

	//   Initialise the LCD display via the I2C bus
	LCDcontrol = 0x38;  // data byte for FUNC_SET_TBL1
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x39; // data byte for FUNC_SET_TBL2
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x14; // data byte for Internal OSC frequency
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x79; // data byte for contrast setting
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x50; // data byte for Power/ICON control Contrast set
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x6C; // data byte for Follower control
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x0C; // data byte for Display ON
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x01; // data byte for Clear display
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
}





int openLCD(int *filep)
{
	/* USE THIS
	 * This is to open the LCD 'file' I/O and init.
	 *
	 * int * I/O 'file' pointer.
	 *
	 *
	 *
	 *
	 */

	int error;
	volatile uint8_t LCDi2cAdd = 0x3C;
	_Uint32t speed = 200000; // nice and slow 10000 (will work with 200000)



	// Open I2C resource and set it up
	if ((*filep = open("/dev/i2c1",O_RDWR)) < 0)  // OPEN I2C1
		error = -1;
	else
		error = -1;

	error = devctl(*filep,DCMD_I2C_SET_BUS_SPEED,&(speed),sizeof(speed),NULL);  // Set Bus speed
	if (error)
	{
		//fprintf(stderr, "Error setting the bus speed: %d\n",strerror ( error ));
		exit(EXIT_FAILURE);
	}
	else
		//printf("Bus speed set = %d\n", speed);

	Initialise_LCD(filep, LCDi2cAdd);

	usleep(1);
	return error;
}

int writeLCD(int row, int col, char write[], int* filep)
{
	int werror;
	/* USE THIS
	 * Int of Row Chosen (0,1)
	 * Int of Column Chosen (0-20)
	 * Char [] of what to write
	 * (Lim 20 Char)
	 *
	 * This will open the I2C file, then write to it what you will.
	 * Up: 0x5E Left: 0x7F Right: 0x7E
	 */

	volatile uint8_t LCDi2cAdd = 0x3C;
	// write some Text to the LCD screen

	size_t n = sizeof(write[])/sizeof(write[0]);
	uint8_t	LCDdata[n]= {};
	strcpy(LCDdata,write);



	// set cursor on LCD to first position first line
	SetCursor(filep, LCDi2cAdd,row,col);
	werror = I2cWrite_(filep, LCDi2cAdd, DATA_SEND, &LCDdata[0], sizeof(LCDdata));
	// write new data to I2C

	printf("\nComplete\n");
	sleep(5);
	return werror;
}


void * LCD_Control(void * data) {
	/* RUN THIS AS THREAD */
	int error;
	int *filep;
	char base[21] = {'N', ':', 0x7F, '_', 0x5E, '_', 0x7E, '_', 'P', '_', 'E', ':', 0x7F, '_', 0x5E, '_', 0x7E, '_', 'P', '_'};
	char base2[21] = {'S',':',0x7F,'_',0x5E,'_',0x7E,'_','P','_','W',':',0x7F,'_',0x5E,'_',0x7E,'_','P','_'};

	traffic_state_t trafficOutputState;
	rail_state_t	railOutputState;
	ped_state_t		pedOutputState;

	error = openLCD(filep);
	if (error != 0)
		printf("Fatal Error: LCD could not initialize.");

	writeLCD(0,0,base,filep);
	writeLCD(1,0,base2,filep);

	ic_state_data_t * td = (ic_state_data_t*) data;


	while (1) {
		pthread_mutex_lock(&td->mutex_state);

		while (td->produce_data) {
			pthread_cond_wait(&td->cond_state_changed, &td->mutex_state);
		}

		trafficOutputState = td -> current_state;


		//boom gate and pedestrian states required. Current conditions assume peds = red, boom gate = up.
		switch(outputState)
		{

		case INITIAL:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case ALL_RED:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case FAULT:
			base[3] = 'Y'; base[5] = 'Y'; base[7] = 'Y'; base[9] = 'Y'; base[13] = 'Y'; base[15] = 'Y'; base[17] = 'Y'; base[19] = 'Y';
			writeLCD(0,0,base,filep);
			base2[3] = 'Y'; base2[5] = 'Y'; base2[7] = 'Y'; base2[9] = 'Y'; base2[13] = 'Y'; base2[15] = 'Y'; base2[17] = 'Y'; base2[19] = 'Y';
			writeLCD(1,0,base2,filep);
			break;

		case NS_S_G:
			base[3] = '-'; base[5] = 'G'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = '-'; base2[5] = 'G'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case NS_S_Y:
			base[3] = 'Y'; base[5] = 'Y'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'Y'; base2[5] = 'Y'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case EW_R_G:
			base[3] = 'G'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'G'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'G'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'G'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case EW_R_Y:
			base[3] = 'Y'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'Y'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'Y'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'Y'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case EW_S_G:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = '-'; base[15] = 'G'; base[17] = 'G'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = '-'; base2[15] = 'G'; base2[17] = 'G'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case EW_S_Y:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = 'Y'; base[15] = 'Y'; base[17] = 'G'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'Y'; base2[15] = 'Y'; base2[17] = 'G'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case NS_R_G:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'G'; base[9] = 'R'; base[13] = 'G'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'G'; base2[9] = 'R'; base2[13] = 'G'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		case NS_R_Y:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'Y'; base[9] = 'R'; base[13] = 'Y'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'Y'; base2[9] = 'R'; base2[13] = 'Y'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;

		default:
			base[3] = 'R'; base[5] = 'R'; base[7] = 'R'; base[9] = 'R'; base[13] = 'R'; base[15] = 'R'; base[17] = 'R'; base[19] = 'R';
			writeLCD(0,0,base,filep);
			base2[3] = 'R'; base2[5] = 'R'; base2[7] = 'R'; base2[9] = 'R'; base2[13] = 'R'; base2[15] = 'R'; base2[17] = 'R'; base2[19] = 'R';
			writeLCD(1,0,base2,filep);
			break;
		}

		td->produce_data = true;
		pthread_cond_signal(&td->cond_state_changed);
		pthread_mutex_unlock(&td->mutex_state);

		//printf("Output will show out for state %d\n", outputState);
	}

}


#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include <string.h> 
#include <errno.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <wiringPi.h> 
#include <mysql/mysql.h>


#define FAN 22 // BCM_GPIO 6 
#define LED 26 // BCM_GPIO 12
#define LIGHTSEN_OUT 2 //gpio27 - J13 connect
#define MAXTIMINGS 85

 
#define DBHOST "192.168.0.12" // 라즈베리 파이 IP 주소 
#define DBUSER "root" 
#define DBPASS "root" 
#define DBNAME "project" 


MYSQL *connector; 
MYSQL_RES *result; 
MYSQL_ROW row; 

 
void Bpluspinmodeset(void); 
void act_fan_on(); 
void act_fan_off();
void act_led_on();
void act_led_off();

int ret_temp;
int curlight = 0;
int SMODE = 0; 
int FANMODE = 0; 
int LEDMODE = 0;
int tempcut = 0;
int extra_tempcut = 0;
static int DHTPIN = 11 ;
static int dht22_dat[5] =  {0,0,0,0,0 }; 


int main (void) {

 	if(wiringPicheck()) printf("Fail"); 
	Bpluspinmodeset(); 
	int curtemp = 0;
//MySQL Connection 
	connector = mysql_init(NULL); 
	if (!mysql_real_connect(connector, DBHOST, DBUSER, DBPASS, DBNAME, 3306, NULL, 0)) 
		{ 
			fprintf(stderr, "%s\n", mysql_error(connector)); 
			return 0; 
		} 
	printf("MySQL(rpidb) opened.\n"); 


	while(1) 
		{ 
			char query[1024]; 
//check temp
			curtemp = get_temperature_sensor(); 
			printf("Temp : %d \n", curtemp); 
//check illumination
			if(get_light() == 0) 
				{ 
					printf("light ! \n");
					curlight = 9;
				} 
			else 
				{ 
					printf("dark ! \n"); 
					curlight = 8;
				} 
//mode check
			read_SMODE();
			read_FANMODE();
			read_LEDMODE();
			read_tempcut();
//Write iotfarmDB 
			sprintf(query,"insert into project3 values (now(),%d,%d,%d,%d,%d)", curtemp,curlight,FANMODE,LEDMODE,SMODE); 
			if(mysql_query(connector, query)) 
				{ 
					fprintf(stderr, "%s\n", mysql_error(connector)); 
					printf("Write DB error\n"); 
				} 	
			delay(5000); //5초마다 data 저장
//Auto control
			//AC and LED
			if(SMODE == 3)
				{
					if(curtemp > tempcut)
						{
							act_fan_on();
						}
					if(curtemp < extra_tempcut)
						{
							act_fan_off();
						}
					if(curlight == 8)
						{
							act_led_on();
						}
					else
						{
							act_led_off();
						}	
				}
			//only AC
			if(SMODE == 2)
				{
					if(curtemp > tempcut)
						{
							act_fan_on();
						}
					if(curtemp < extra_tempcut)
						{
							act_fan_off();
						}
				}

			} 

		return 0 ; 

		} 

 

int wiringPicheck(void)
	{ 
		if (wiringPiSetup () == -1) 
			{ 
				fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno)); 
				return 1 ; 
			} 
	} 

void Bpluspinmodeset(void) 
	{ 
		pinMode(LIGHTSEN_OUT, INPUT);
		pinMode (FAN, OUTPUT);
		pinMode (LED, OUTPUT);  
	} 

int get_light() 
	{ 
	// sets up the wiringPi library 
		if (wiringPiSetup () < 0) 
			{ 
				fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)); 
				return 1; 
			} 
		if(digitalRead(LIGHTSEN_OUT)) //day 
		return 1; 
		else //night 
		return 0; 
	}

int read_SMODE()
	{ 
		FILE *f;
		f=fopen("data/smode.txt","r");
		fscanf(f,"%d",&SMODE);
		fclose(f);

		if(SMODE == 3) 
			{ 
				printf("smode on \n"); 
			} 
		if(SMODE == 2) 
			{ 
				printf("sleep on \n"); 
			} 
		if(SMODE == 0) 
			{ 
				printf("smode off \n"); 
			} 
		return 0;
	}

int read_FANMODE()
	{ 
		FILE *f;
		f=fopen("data/fanmode.txt","r");
		fscanf(f,"%d",&FANMODE);
		fclose(f);
		if(FANMODE == 1) 
			{ 
				printf("fan on \n"); 
			} 
		else 
			{ 
				printf("fan off \n"); 
			} 
		return 0;
	}

int read_LEDMODE()
	{ 
		FILE *f;
		f=fopen("data/ledmode.txt","r");
		fscanf(f,"%d",&LEDMODE);
		fclose(f);

		if(LEDMODE == 1) 
			{ 
				printf("led on \n"); 
			} 
		else 
			{ 
				printf("led off \n"); 
			} 

		return 0;
	}

int read_tempcut()
	{ 
		int tempcut_a;
		FILE *f;
		f=fopen("data/tempcut.txt","r");
		fscanf(f,"%d",&tempcut);
		fclose(f);

		extra_tempcut = tempcut-10;
		tempcut_a = tempcut;

		printf("Set temp : %d\n", tempcut_a);
		return 0;
	}

int get_temperature_sensor() 
	{ 
		int received_temp; 
		DHTPIN = 11; 
		if (wiringPiSetup() == -1) 
		exit(EXIT_FAILURE) ; 
		if (setuid(getuid()) < 0) 
			{ 
				perror("Dropping privileges failed\n"); 
				exit(EXIT_FAILURE); 
			} 
		while (read_dht22_dat_temp() == 0) 
			{ 
				delay(500); // wait 1sec to refresh 
			} 
		received_temp = ret_temp ; 
//		printf("Temperature = %d\n", received_temp); 
		return received_temp; 
	}

static uint8_t sizecvt(const int read) 
	{ 
	/* digitalRead() and friends from wiringpi are defined as returning a value 
	< 256. However, they are returned as int() types. This is a safety function */ 
		if (read > 255 || read < 0) 
			{ 
				printf("Invalid data from wiringPi library\n"); 
				exit(EXIT_FAILURE); 
			} 
		return (uint8_t)read; 
	}

int read_dht22_dat_temp() 
	{ 
		uint8_t laststate = HIGH; 
		uint8_t counter = 0; 
		uint8_t j = 0, i; 
		dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] = 0; 
	// pull pin down for 18 milliseconds 
		pinMode(DHTPIN, OUTPUT); 
		digitalWrite(DHTPIN, HIGH); 
		delay(10); 
		digitalWrite(DHTPIN, LOW); 
		delay(18); 
	// then pull it up for 40 microseconds 
		digitalWrite(DHTPIN, HIGH); 
		delayMicroseconds(40); 
	// prepare to read the pin 
		pinMode(DHTPIN, INPUT); 
	// detect change and read data 
		for ( i=0; i< MAXTIMINGS; i++) 
			{ 
				counter = 0; 
				while (sizecvt(digitalRead(DHTPIN)) == laststate) 
					{ 
						counter++; 
						delayMicroseconds(1); 
						if (counter == 255)
							{ 
								break; 
							} 
					} 
				laststate = sizecvt(digitalRead(DHTPIN)); 
				if (counter == 255) break; 
	// ignore first 3 transitions 
				if ((i >= 4) && (i%2 == 0))
					{ 
	// shove each bit into the storage bytes 
						dht22_dat[j/8] <<= 1; 
						if (counter > 16) 
						dht22_dat[j/8] |= 1; 
						j++; 
					} 
			} 
	// check we read 40 bits (8bit x 5 ) + verify checksum in the last byte 
	// print it out if data is good 
		if ((j >= 40) && (dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF)) )  
			{ 
				float t, h; 
//				h = (float)dht22_dat[0] * 256 + (float)dht22_dat[1]; 
//				h /= 10; 
				t = (float)(dht22_dat[2] & 0x7F)* 256 + (float)dht22_dat[3]; 
				t /= 10.0; 
				if ((dht22_dat[2] & 0x80) != 0) t *= -1; 
//				ret_humid = (int)h; 
				ret_temp = (int)t; 
				//printf("Temperature = %.2f *C \n", t ); 
				return ret_temp; 
			} 
		else 
			{ 
				printf("Data not good, skip\n"); 
				return 0; 
			} 
	}

void act_fan_on() 
	{ 
		if(wiringPicheck()) printf("Fail"); 
		pinMode (FAN, OUTPUT); 
		digitalWrite (FAN, 1) ; // On 
	}

void act_fan_off() 
	{ 
		if(wiringPicheck()) printf("Fail"); 
		pinMode (FAN, OUTPUT); 
		digitalWrite (FAN, 0) ; // On 
	}

void act_led_on() 
	{ 
		if(wiringPicheck()) printf("Fail"); 
		pinMode (LED, OUTPUT); 
		digitalWrite (LED, 1) ; // On 
	}

void act_led_off()
	{ 
		if(wiringPicheck()) printf("Fail"); 
		pinMode (LED, OUTPUT); 
		digitalWrite (LED, 0) ; // On 
	}
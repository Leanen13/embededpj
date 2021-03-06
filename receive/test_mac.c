#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>
#include <linux/poll.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define	KEY_FILE_NAME	"/dev/key_driver"
#define	KEY_FILE_NAME2	"/dev/key_driver2"
#define	KEY_FILE_NAME3	"/dev/key_driver3"
#define	KEY_FILE_NAME4	"/dev/key_driver4"
#define DOT_FILE_NAME   "/dev/dot_driver"
#define	SENS_FILE_NAME	"/dev/sens_driver"
#define	SENS_FILE_NAME2	"/dev/sens_driver2"
#define	SENS_FILE_NAME3	"/dev/sens_driver3"
#define MOTOR_FILE_NAME "/dev/mot_driver"

void *thread_sens1(void *arg);
void *thread_sens2(void *arg);
void *thread_sens3(void *arg);

int		mac_state = 0;
int		EA500 = 0, EA100 = 0, EA50 = 0;
int		key_fd, key2_fd, key3_fd, key4_fd;
int		key_state=0;
int main()
{
	int		key1_ret, key2_ret,key3_ret,key4_ret;
	char		key,key2,key3,key4;
	struct pollfd	key_ev[1],key2_ev[1],key3_ev[1],key4_ev[1];

	int		j = 0;

	// dot matrix
	int     dot_fd;
	char    data, val;
	int     number, len, i, a, b, k;
	int 	size = 0, size2=0, size3=0, n = 0, m = 0, time = 0;
	clock_t start, end;
	char    dot_values[10][8] = {
		{0xFF, 0xFF, 0x00, 0x7E, 0x7E, 0x00, 0xFF, 0xFF},               //0
		{0xFF, 0xFF, 0xFF, 0xBE, 0x00, 0xFE, 0xFF, 0xFF},               //1
		{0xFF, 0xFF, 0x60, 0x66, 0x66, 0x06, 0xFF, 0xFF},               //2
		{0xFF, 0xFF, 0x66, 0x66, 0x66, 0x00, 0xFF, 0xFF},               //3             
		{0xFF, 0xFF, 0x07, 0xF7, 0x00, 0xF7, 0xFF, 0xFF},               //4
		{0xFF, 0xFF, 0x06, 0x66, 0x66, 0x60, 0xFF, 0xFF},               //5
		{0xFF, 0xFF, 0x00, 0x76, 0x76, 0x70, 0xFF, 0xFF},               //6
		{0xFF, 0xFF, 0x10, 0xE0, 0xE0, 0x00, 0xFF, 0xFF},               //7
		{0xFF, 0xFF, 0x00, 0x6E, 0x6E, 0x00, 0xFF, 0xFF},               //8
		{0xFF, 0xFF, 0x0E, 0x6E, 0x6E, 0x00, 0xFF, 0xFF}                //9
	};
	char    dot_char[5][8] = {
		{0xFF, 0xFF, 0x7F, 0x7F, 0x00, 0x7F, 0x7F, 0xFF},               //t
		{0xFF, 0xFF, 0x00, 0x7E, 0x7E, 0x7E, 0xFF, 0xFF},               //c
		{0xFF, 0xFF, 0xFF, 0xFF, 0x99, 0xFF, 0xFF, 0xFF},               //:
		{0xFF, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0xFF, 0xFF},               //W
		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}                //NULL
	};

	// mot
	int     mot_fd;
	char    mot_data = 1;
	
	// start
	key_fd = open(KEY_FILE_NAME, O_RDWR | O_NONBLOCK);
	key2_fd = open(KEY_FILE_NAME2, O_RDWR | O_NONBLOCK);
	key3_fd = open(KEY_FILE_NAME3, O_RDWR | O_NONBLOCK);
	key4_fd = open(KEY_FILE_NAME4, O_RDWR | O_NONBLOCK);

	
	if (key_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", KEY_FILE_NAME);
		return -1;
	}
	
	if (key2_fd < 0 || key3_fd < 0 || key4_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", KEY_FILE_NAME);
		return -1;
	}

	// prepare starting
	while (mac_state == 0)
	{
		key_ev[0].fd = key_fd;
		key_ev[0].events = POLLIN;	// waiting read
		key1_ret = poll(key_ev, 1, 1000);		// event waiting
		if (key1_ret < 0)
		{
			fprintf(stderr, "Poll error\n");
			exit(0);
		}

		if (key1_ret == 0)
		{
			puts("key 1 please.");
		}
		if (key_ev[0].revents & POLLIN)
		{
			puts("machine please.");
			mac_state = 1;
		}
	}

	// start insert coin and calculate
	mot_fd = open(MOTOR_FILE_NAME, O_RDWR);
	
	// thread create
	int thread_state1,thread_state2,thread_state3;
	pthread_t sens1, sens2, sens3;
	void *sens1_ret, *sens2_ret, *sens3_ret;
	dot_fd = open(DOT_FILE_NAME, O_RDWR);
	write(dot_fd, &dot_char[4], sizeof(char));
	while(mac_state == 1){
	thread_state1 = pthread_create (&sens1, NULL, thread_sens1 , NULL);
	thread_state2 = pthread_create (&sens2, NULL, thread_sens2 , NULL);
	thread_state3 = pthread_create (&sens3, NULL, thread_sens3 , NULL);
	write(mot_fd, &mot_data, sizeof(char));
	write(dot_fd, &dot_char[4], sizeof(char));
	pthread_join(sens1, &sens1_ret);
	pthread_join(sens2, &sens2_ret);
	pthread_join(sens3, &sens3_ret);
	}
	
	mot_data = 0;
	write(mot_fd, &mot_data, sizeof(char));
	
	// thread end
	// dot matrix print start

	if (dot_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", DOT_FILE_NAME);
		return -1;
	}

	printf("mac %d\n", mac_state);
	while (mac_state == 2)
	{
		puts("1key is print machine off ");
		puts("2key is print total money");
		puts("3key is print the number of each coin");
		puts("4key is print the number of total coin");
		puts("key please");

		key2_ev[0].fd		= key_fd;
		key2_ev[0].events	= POLLIN;	// waiting read
		key2_ev[1].fd		= key2_fd;
		key2_ev[1].events	= POLLIN;	// waiting read
		key2_ev[2].fd		= key3_fd;
		key2_ev[2].events	= POLLIN;	// waiting read
		key2_ev[3].fd		= key4_fd;
		key2_ev[3].events	= POLLIN;	// waiting read
		
		key2_ret = poll(key2_ev, 4, 1000000);		// event waiting
		
		

		if (key2_ret < 0)
		{
			fprintf(stderr, "Poll error\n");
			exit(0);
		}
		if (key2_ret == 0)
		{
			puts("please key input\n");
		}
	////////////////////////////////////////
		if (key2_ev[0].revents & POLLIN)
		{
			puts("machine off");
			break;
		}

		
		if (key2_ev[1].revents & POLLIN)
		{
				char	*arra ;
				char	*value_arra ;
				
			//	key2_ret = poll(key_ev, 4, 1000);
				puts("total money ");
				m = 0, n = 0, size = 0;
				number = (500 * EA500) + (100 * EA100) + (50 * EA50);
				len = number; //1250
				
				do{
				len = (len / 10); //3
				size ++;	//4
				} while (len > 0);
				
				arra= (char*)malloc(sizeof(char)*(size));// 0 0 0 0
	
				do{
				a = number % 10; 
				b = number / 10;
				number = b;
				arra[n] = a;// 1 2 5 0
				n++; // 4
					} while (b!=0);

				n = n-1;  //n 3
	
				value_arra = (char*)malloc(sizeof(char)*8*(size+3)); // 1250 size 4       1111 1111t /1111 1111 : /1111 1111 1/1111 11112 /1111 1111 5/1111 1111 0 /1111 1111w
				
				for(i=0; i<8; i++){
					value_arra[m] = dot_char[0][i];  // T print store
					m++; //8
				}

				for(i=0; i<8; i++){
					value_arra[m] = dot_char[2][i];  // : print store
					m++; //16
				}
				
				
				while(n>=0)
				{
					k = arra[n];
					for(i= 0; i<8; i++){
					value_arra[m] = dot_values[k][i];
					m++;
					}
				n--;
				}
				
				
				for(i=0; i<8; i++){
					value_arra[m] = dot_char[3][i];  // W print store
					m++;
				}
				
				
				
				//do
				//{
					for(i =0; i<m-7; i++){
						while(time < 100000){
							start = clock();
							write(dot_fd, &value_arra[i], sizeof(char));
							end = clock();
							time += (int)(end - start);			
						}
						time = 0;
					}
				//} while (key2_ret == 0 && !(key2_ev[1].revents & POLLIN));
				
					
				free(arra);
				free(value_arra);	
		}	
		

		if (key2_ev[2].revents & POLLIN)
		{
				
				char	*arrb1;
				char	*arrb2;
				char	*arrb3;
				char	*value_arrb;
				puts("the number of each coin ");	  // each coin
				
				// 500 input
				m = 0, n = 0, size = 0;
				number = EA500;
				len = number;
				do{
				len = (len / 10);
				size ++;	
				} while (len > 0);
				
				arrb1 = (char*)malloc(sizeof(char)*(size));

				n = 0, size2 = 0;
				number = EA100;
				len = number;
				
				do{
				len = (len / 10);
				size2 ++;	
				} while (len > 0);

				arrb2 = (char*)malloc(sizeof(char)*(size2));

				n = 0, size3 = 0;
				number = EA50;
				len = number;
				
				do{
				len = (len / 10);
				size3 ++;	
				} while (len > 0);
				
				arrb3 = (char*)malloc(sizeof(char)*(size3));
		
				value_arrb = (char*)malloc(sizeof(char)*8*(size+size2+size3+16));
			
				number = EA500;

				do{
				a = number % 10;
				b = number / 10;
				number = b;
				arrb1[n] = a;
				n++;
					} while (b!=0);

				n = n-1;

				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[1][i];  // c print store	1
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[2][i];  // : print store	2
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[5][i];  // 5 print store  	3
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[0][i];  // 0 print store	4
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[0][i];  // 0 print store	5
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[2][i];  // : print store	6
					m++;
				}			
				
				while(n>=0)
				{
					k = arrb1[n];
					for(i= 0; i<8; i++){
					value_arrb[m] = dot_values[k][i];
					m++;
					}
				n--;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[4][i];  // NULL print store	7
					m++;
				}
	
				// 100 input
				n = 0;
				number = EA100;
/*				
				len = number;
				
				do{
				len = (len / 10);
				size2 ++;	
				} while (len > 0);
				arrb2 = (char*)malloc(sizeof(char)*(size2));
*/
				do{
				a = number % 10;
				b = number / 10;
				number = b;
				arrb2[n] = a;
				n++;
					} while (b!=0);

				n = n-1;
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[1][i];  // 1 print store	8
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[0][i];  // 0 print store	9
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[0][i];  // 0 print store	10
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[2][i];  // : print store	11
					m++;
				}			
				
				while(n>=0)
				{
					k = arrb2[n];
					for(i= 0; i<8; i++){
					value_arrb[m] = dot_values[k][i];
					m++;
					}
				n--;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[4][i];  // NULL print store	12
					m++;
				}
	
				// 50 input
				n = 0;
				number = EA50;
				
				do{
				a = number % 10;
				b = number / 10;
				number = b;
				arrb3[n] = a;
				n++;
					} while (b!=0);

				n = n-1;
	
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[5][i];  // 5 print store	13
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_values[0][i];  // 0 print store	14
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[2][i];  // : print store	15
					m++;
				}			
				
				while(n>=0)
				{
					k = arrb3[n];
					for(i= 0; i<8; i++){
					value_arrb[m] = dot_values[k][i];
					m++;
					}
				n--;
				}
				
				for(i=0; i<8; i++){
					value_arrb[m] = dot_char[4][i];  // NULL print store	16
					m++;
				}
	
			//	while (key2_ret == 0)
			//	{
					for(i =0; i<m-7; i++){
						while(time < 100000){
							start = clock();
							write(dot_fd, &value_arrb[i], sizeof(char)); 
							end = clock();
							time += (int)(end - start);			
						}
						time = 0;
					}
			//	}	
				free(arrb1);
				free(arrb2);
				free(arrb3);
				free(value_arrb);		
		}
			
	
		if (key2_ev[3].revents & POLLIN)
		{
						
				char	*arrc;
				char	*value_arrc;		
				puts("the number of total coin ");
				m = 0, n = 0, size = 0;
				number = EA500 + EA100 + EA50;
				len = number;
				
				do{
				len = (len / 10);
				size ++;	
				} while (len > 0);
				
				arrc = (char*)malloc(sizeof(char)*(size));
	
				do{
				a = number % 10;
				b = number / 10;
				number = b;
				arrc[n] = a;
				n++;
					} while (b!=0);

				n = n-1;
	
				value_arrc = (char*)malloc(sizeof(char)*8*(size+4));
				
				for(i=0; i<8; i++){
					value_arrc[m] = dot_char[0][i];  // T print store	1
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrc[m] = dot_char[1][i];  // C print store	2
					m++;
				}
				
				for(i=0; i<8; i++){
					value_arrc[m] = dot_char[2][i];  // : print store	3
					m++;
				}
				
				while(n>=0)
				{
					k = arrc[n];
					for(i= 0; i<8; i++){
					value_arrc[m] = dot_values[k][i];
					m++;
					}
				n--;
				}
				
				for(i=0; i<8; i++){
					value_arrc[m] = dot_char[4][i];  // null print store	4
					m++;
				}
				
				
			//	while (key2_ret == 0)
			//	{
					for(i =0; i<m-7; i++){
						while(time < 100000){
							start = clock();
							write(dot_fd, &value_arrc[i], sizeof(char));
							end = clock();
							time += (int)(end - start);			
						}
						time = 0;
					}
			//	}
				free(arrc);
				free(value_arrc);
				
		}


		
	}
	close(key_fd);
	close(key2_fd);
	close(key3_fd);
	close(key4_fd);
	//close(mot_fd);
	//close(sens_fd);
	//close(sens2_fd);
	//close(sens3_fd);
	return 0;	
}

void *thread_sens1(void *arg)
{
	int		key1_ret;
	char		key;
	struct pollfd	key_ev[1];

	int		sens_fd;
	struct pollfd	sens_ev[1];
	char		sens_value1; 
	char		fir = 0; 
	char		sed = 0; 
	int		j = 0;
	sens_fd = open(SENS_FILE_NAME, O_RDWR | O_NONBLOCK);
	while(mac_state == 1){
	key_ev[0].fd = key2_fd;
	key_ev[0].events = POLLIN;
	read(sens_fd, &sens_value1, 1);
	key1_ret = poll(key_ev, 1, 50);
	if (sens_value1 == 1 && fir == 0) {
			fir = 1;
			printf("500W : %d\n", ++EA500);
		}
	if (fir == 1) {
		for (j = 0; j < 6; j++) {
			read(sens_fd, &sed, 1);
		}
	fir = 0;
	}
	if (key_ev[0].revents & POLLIN)
		{
			puts("coin insert end");
			mac_state = 2;
		}
	}
}

void *thread_sens2(void *arg)
{
	int		key1_ret;
	char		key;
	struct pollfd	key_ev[1];

	int		sens2_fd;
	struct pollfd	sens_ev[1];
	char		sens_value2; 
	char		fir2 = 0; 
	char		sed2 = 0; 
	int		j = 0;
	sens2_fd = open(SENS_FILE_NAME2, O_RDWR | O_NONBLOCK);
	while(mac_state == 1){
	key_ev[0].fd = key2_fd;
	key_ev[0].events = POLLIN;
	read(sens2_fd, &sens_value2, 1);
	key1_ret = poll(key_ev, 1, 50);
	if (sens_value2 == 1 && fir2 == 0) {
			fir2 = 1;
			printf("100W : %d\n", ++EA100);
	}
	if (fir2 == 1) {
		for (j = 0; j < 6; j++) {
			read(sens2_fd, &sed2, 1);
		}
	fir2 = 0;
	}
	if (key_ev[0].revents & POLLIN)
		{
			puts("coin insert end");
			mac_state = 2;
		}	
	}
}

void *thread_sens3(void *arg)
{

	int		key1_ret;
	char		key;
	struct pollfd	key_ev[1];

	int		sens3_fd;
	struct pollfd	sens_ev[1];
	char		sens_value3; 
	char		fir3 = 0; 
	char		sed3 = 0; 
	int		j = 0;
	sens3_fd = open(SENS_FILE_NAME3, O_RDWR | O_NONBLOCK);
	while(mac_state == 1){
		key_ev[0].fd = key2_fd;
		key_ev[0].events = POLLIN;
		read(sens3_fd, &sens_value3, 1);
		key1_ret = poll(key_ev, 1, 50);
		if (sens_value3 == 1 && fir3 == 0) {
			fir3 = 1;
			printf("50W : %d\n", ++EA50);
		}
		if (fir3 == 1) {
			for (j = 0; j < 6; j++) {
				read(sens3_fd, &sed3, 1);
			}
		fir3 = 0;
		}
		if (key_ev[0].revents & POLLIN)
		{
			puts("coin insert end");
			mac_state = 2;
		}		
	}
}


#include "dlab_def.h"
#include <ctype.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>

sem_t data_avail;

#define MAXS 10000
#define Ku 51
#define Pu 0.05
float theta[MAXS];
float ref[MAXS];

int N = 20;
float Ti = Pu / 2;
float Td = Pu / 8;
double Kp = 0.6 * Ku;
double run_time = 10.0;
float Fs = 200.0;
int motor_number = 11;
char input;
char reference[9];
int no_of_samples;

float actSat(float in) {
	if (in < -3)
    return  -3;
else if (in > 3)
    return 3;
else 
	return in;
}

void *Control(void *arg)
{
	int k = 0;
	float motor_position, p;
	float e[]={0};  // error
	float u[]={0};  //
	float i[]={0};  //integral
	float d[]={0};  //derivative 
	float a[]={0};	//u-v
	float v[]={0};  //output from pid
	
	no_of_samples = (int)(run_time*Fs);
	while (k+1 < no_of_samples) {
		sem_wait(&data_avail);
		motor_position = EtoR(ReadEncoder());
		e[k] = ref[k] - motor_position;
		p = Kp * e[k];
		a[k] = actSat(k) - v[k];
		i[k] = i(k-1) + (Kp / Ti) * e(k-1);
		// if anti-windup is used
		// i[k] = i(k-1) + (Kp / Ti) * e(k-1) + (1 / 0.01) * a[k-1];
		d[k] = ((Td / (N + Td)) * d[k-1]) + ((Kp*Td*N / (N + Td)) * (e[k] - e[k-1]));
		v[k] = p + i[k] + d[k];
		DtoA(VtoD(uk));
		theta[k] = motor_position;
		k++;
	}
	pthread_exit(NULL);

}

int main(void *arg)
{
 pthread_t controlling;
 int mag = 0, i = 0;
	while(1)
	{
		printf("Please enter on of the following:\nr: Run the algorithm\n");
		printf("p: Change value of Kp \nf: Change sampling frequency\n");
		printf("i: Change value of Ti \nd: Change value of Td\nn: Change value of N\n");
		printf("t: Change total Runtime \nu: Change the type of Input Step or Square\n");
		printf("g: Plot Motor position on screen \nh: Save a hard copy of the plot in Postscript\n");
		printf("q: exit\n");
		scanf(" %c", &input);
		switch(input) {
			case 'r':
				sem_init(&data_avail,0,0);
				if (Initialize(Fs,motor_number) != 0) {
					printf("\nDAMMIT\n");
					exit(0);
				}
				if (pthread_create(&controlling,NULL,&Control,NULL) != 0) {
					printf("\nError starting control system thread\n");
					exit(-1);
				}
				pthread_join(controlling,NULL);
				plot(ref, theta, Fs, no_of_samples, PS, "Position","Time","Angle in Radians");
				sem_destroy(&data_avail);
				Terminate();
				break;
			case 'u':
				printf("\nStep or Square?\n");
				scanf(" %s", &reference);
				while( reference[i] ) {
					putchar(tolower(reference[i]));
					i++;
				}
				if (strcmp(reference,"step") == 0) {
					printf("\nEnter the magnitude\n");
					scanf(" %d",&mag);
					for (i=0;i<MAXS;i++) {
						ref[i] = (float)1*mag;
					}
				} else if (strcmp(reference,"square") == 0) {
					float mag,freq,dc;
					printf("\nEnter magnitude\n");
					scanf(" %f",&mag);
					printf("Enter frequency\n");
					scanf(" %f",&freq);
					printf("Enter duty cycl\n");
					scanf(" %f",&dc);
					Square(ref, MAXS, Fs, mag, freq, dc);
				} else {
					printf("\nInvalid Input\n");
					exit(-1);
				}
				break;
			case 'i':
				printf("Enter new Ti value:\n");
				scanf(" %lf",&Ti);
				break;
			case 'd':
				printf("Enter new Td value:\n");
				scanf(" %lf",&Td);
				break;
			case 'n':
				printf("Enter new N value:\n");
				scanf(" %d",&N);
				break;	
			case 'p':
				printf("Enter new Kp value:\n");
				scanf(" %lf",&Kp);
				break;
			case 'f':
				printf("Enter new Fs value:\n");
				scanf(" %f",&Fs);
				break;
			case 't':
				printf("Enter new run time:\n");
				scanf(" %lf",&run_time);
				break;
			case 'g':
				plot(ref, theta, Fs, run_time, SCREEN, "Position","Time","Angle in Radians");
				break;
			case 'h':
				plot(ref, theta, Fs, run_time, PS, "Position","Time","Angle in Radians");
				break;
			case 'q':
				exit(0);
			default:
				printf("\nYa messed up A-Aron!\n");
		}
	}	
}

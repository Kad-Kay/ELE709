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
#define Pi 3.14159
#define N 20
#define Ku 51.48
#define Pu 0.055

float theta[MAXS];
float ref[MAXS];

float Kp = 0.6*Ku, Ti = Pu/2, Td = Pu/8;
double run_time = 10.0;
float Fs = 200.0;
int motor_number = 11;
char input;
char reference[9];
int no_of_samples;
float lastsample = 0;


void *Control(void *arg)
{
	float T = 1/Fs;
	printf("***\n");
	int k = 0;
	float motor_position;
	float ek;
	float uk;
	no_of_samples = (int)(run_time*Fs);
	float I;
	float D;
	float E;
	float Ik1 = 0.0, Ek1 = 0.0, Dk1 = 0.0;
	while (k < no_of_samples) {
		sem_wait(&data_avail);
		motor_position = EtoR(ReadEncoder());

		E = ref[k] - motor_position;
		I = Ik1*T + (Kp/Ti) * Ek1 * T * T;
		D = (Td/((N*T)+Td))*Dk1 + ((Kp*Td*20)/(N*T+Td))*(E - Ek1);
		uk = (Kp*E) + D + I;
		DtoA(VtoD(uk));
		theta[k] = motor_position;
		Ik1 = I;
		Ek1 = E;
		Dk1 = D;
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
		printf("Please enter on of the following:\nr: Run the algorithm\np: Change the value of Kp\nf: Change sampling frequency\nt: Chnage the total Runtime\nu: Change the type of Input Step or Square\ng: Plot Motor position on screen\nh: Save a hard copy of the plot in Postscript\nq: exit\n");
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
				for(i=0;i<MAXS;i++) {
					ref[i] = ref[i]*(180/Pi);
					theta[i] = theta[i]*(180/Pi);
				}
				plot(ref, theta, Fs, no_of_samples, PS, "Position","Time","Angle in Radians");
				for(i=0;i<MAXS;i++) {
					ref[i] = 0;
					theta[i] = 0;
				}
				sem_destroy(&data_avail);
				Terminate();
				break;
			case 'u':
				printf("\nStep or Square?\n");
				scanf(" %s", &reference);
				i=0;
				if (strcmp(reference,"step") == 0) {
					printf("\nEnter the magnitude\n");
					scanf(" %d",&mag);
					for (i=0;i<MAXS;i++) {
						ref[i] = (float)mag*(Pi/180);
					}
					i=0;
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
				plot(ref, theta, Fs, no_of_samples, SCREEN, "Position","Time","Angle in Radians");
				break;
			case 'h':
				plot(ref, theta, Fs, no_of_samples, PS, "Position","Time","Angle in Radians");
				break;
			case 'q':
				exit(0);
			default:
				printf("\nYa messed up A-Aron!\n");
		}
	}	
}

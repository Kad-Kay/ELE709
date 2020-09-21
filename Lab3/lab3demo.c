#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


int a[18][16]={0}, b[16][18]={0};
int ab[18][18] = {0};

int *ptr;
 
struct thread_info {
	int first, second;			// The numbers a and b
	int m, n;
};

typedef struct thread_info thread_info_t;

void *compute_C_ij(void *arg)
{
	thread_info_t *info;
	int m, n, temp, k;    
	info = (thread_info_t *)arg;
	m = info->m;
	n = info->n;
	
	temp=0;
	for(k=0;k<16;k++) {
		temp+=a[m][k]*b[k][n];
	}
	ab[m][n]=temp;
	
	pthread_exit(NULL);   
}

int main(void)
{
	int i, j, k;
	pthread_t thread[18][18];
	thread_info_t info;
   
   //initializing first array
   for (i = 0; i < 18; i++) {
	   for (j = 0; j < 16; j++) {
		   a[i][j] = i + j;
	   }
   }   
      //initializing second array
   for (i = 0; i < 16; i++) {
	   for (j = 0; j < 18; j++) {
		   b[i][j] = i + (2*j);
	   }
   }
   //creating threads to compute multiplication 
   for (i = 0; i < 18; i++) {
	   for (j = 0; j < 18; j++) {
		    ptr = malloc(2 * sizeof(int));
			*(ptr) = i;
			*(ptr+1) = j;
			info.m = *(ptr); 
			info.n = *(ptr+1);
		    pthread_create(&thread[i][j], NULL, &compute_C_ij, &info);			
	   }
   }
    free(ptr);
	ptr=NULL;
   	printf("\nPrinting final array: \n");
	for(i=0;i<18;i++) {
			printf("\n");
			for(j=0;j<18;j++) {
				pthread_join(thread[i][j],NULL);
				printf("  %d  ",ab[i][j]);

			}
	}

   pthread_exit(NULL);
}


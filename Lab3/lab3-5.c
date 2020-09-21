#include <stdio.h>
#include <math.h>
#include <sys/mman.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>


struct thread_info {
	int a[16]; 
	int b[16];
	int c;
};

typedef struct thread_info thread_info_t;

int compute_C_ij(int x[16], int y[16])
{
	int i;
	int sum=0;
	for(i=0;i<16;i++)
	 sum = sum + x[i]*y[i];
	return sum;
}

void *func(void *arg)
{
	int i;
	thread_info_t *info;
	info = (thread_info_t *)arg;
	info->c = compute_C_ij(info->a, info->b);
}
	
int main(void)
{
	int i, j, k;
	int A[18][16];
	int B[16][18];
	
	pthread_t thread1;
	thread_info_t info1[18][18];


	for(i=0;i<18;i++)
	 for(j=0;j<16;j++){
	  A[i][j] = (i+1)+(j+1);
	 }

	for(i=0;i<16;i++)
	 for(j=0;j<18;j++){
	  B[i][j] = (i+1)+2*(j+1);
	 }

	for(i=0;i<18;i++)
	 for(j=0;j<18;j++)
	  for(k=0;k<16;k++){
	   info1[i][j].a[k] = A[i][k];
	   info1[i][j].b[k] = B[k][j];
	}

	for(i=0;i<18;i++)
	 for(j=0;j<18;j++){
	  if (pthread_create(&thread1, NULL, &func, &info1[i][j]) != 0) {
           printf("Error in creating thread 1\n");
           exit(1);
   	  }
	 }

	for(i=0;i<18;i++)
	 for(j=0;j<18;j++)
	  pthread_join(thread1, NULL);

	for(i=0;i<18;i++)
	 printf("%d \n", info1[i][i].c); 

	return 0;
}

#include <stdio.h>
#include <time.h>

int main()
{
  int maxitr = 2000000;
  int a,b,i;
  double c;
  char operator;
  scanf("Please enter two whole integer numbers: a = %d and b = %d", &a, &b);
  scanf("Please enter the operation to conduct: %c", &operator);
  switch (operator) 
  {
     case '+' :
       clock_t CPU_time_1 = clock();
       for(i=0;i<maxitr;i++)
       {
         c=a+b;
       }
       clock_t CPU_time_2 = clock();
       break;
     case '-' :
       clock_t CPU_time_1 = clock();
       for(i=0;i<maxitr;i++)
       {
         c=a-b;
       }
       clock_t CPU_time_2 = clock();
       break;
     case '*' :
       clock_t CPU_time_1 = clock();
       for(i=0;i<maxitr;i++)
       {
         c=a*b;
       }
       clock_t CPU_time_2 = clock();
       break;
     case '/' :
       clock_t CPU_time_1 = clock();
       for(i=0;i<maxitr;i++)
       {
         c=a/b;
       }
       clock_t CPU_time_2 = clock();
       break;
     default :
       printf("Invalid Operator");
   }
  clock_t Elapsed_time = CPU_time_2 - CPU_time_1;
  printf("The %c operation took %d time", operator, Elapsed_time);
  
  return 0
}

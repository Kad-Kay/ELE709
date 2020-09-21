#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <errno.h>

struct Msg {char str[30]; unsigned int priority; };
int errno;

const char *client1 = "/c_request";  // Define the name of the MQ, use your
                                     // last name as part of the name to create
                                     // a unique name.
const char *server1 = "/s_return";

int stop_printing; 

void *prompt(void *arg)  // This thread 
{
   mqd_t request;   
   struct mq_attr req_attrs;
   ssize_t n;
   int retcode, value;
   int input;
   struct Msg reqMsg;
   unsigned int priority = 10;   


   request = mq_open(client1, O_WRONLY);  // Open the MQ for write only
   if (request < 0 ) perror("Receiver mq_open:");  // Error checking
   
   stop_printing = 0;

   while(1) {
	    sleep(2);
	    printf("Enter value for n: \n");
		   scanf("%d", &input);	
		
		sprintf(reqMsg.str, "%d", input);  // Set up message to be sent
		reqMsg.priority = priority--;    // Set up priority of this message
		retcode = mq_send(request, reqMsg.str, (size_t)strlen(reqMsg.str), reqMsg.priority);
		if (retcode < 0) perror("Sender mq_send:");  // Error checking	
		if (input == 0) {
			mq_close(request);
			stop_printing = 1;
			pthread_exit(NULL);
		}		
   }	   
}
   
void *print(void *arg)  // This thread 
{
	int retcode, sMsgs;
	mqd_t sreturn;
	struct Msg returnMsg;
	struct mq_attr ret_attrs;
	ssize_t n;
	unsigned int priority;  // Set up message priorities
	char inBuf[50];
	
	
	sreturn = mq_open(server1, O_RDONLY);  // Open the MQ for reading only 

	if (sreturn < 0) perror("Sender mq_open:");  // Error checking	
	
	while (1) {
		mq_getattr(sreturn, &ret_attrs);     // Determine no. of msgs currently in the client request MQ
		sMsgs = ret_attrs.mq_curmsgs;
		while (sMsgs-- > 0) {
			printf("I am here\n");
			n = mq_receive(sreturn, inBuf, sizeof(inBuf), &priority);
			inBuf[n] = '\0';
			printf("%s\n", inBuf);
			if (n < 0) perror("Receiver mq_receive:");  // Error checking
		}
		
		if (stop_printing == 1) {
			mq_close(sreturn);
			pthread_exit(NULL);
		}
	}	
}

int main(void *arg)
{
   pthread_t prompt_thread, print_thread;

   // Create a thread to send messages to the MQ
   printf("Creating prompt thread\n");
   if (pthread_create(&prompt_thread, NULL, &prompt, NULL) != 0) {
       printf("Error creating prompt thread.\n");
       exit(-1);
   }

   // Cretae a thread to receive the messages from the MQ
   sleep(1);
   printf("Creating print thread\n");
   if (pthread_create(&print_thread, NULL, &print, NULL) != 0) {
       printf("Error creating print thread.\n");
       exit(-1);
   }

   pthread_join(prompt_thread, NULL);
   pthread_join(print_thread, NULL);   

   mq_unlink(client1);     // Destroy the MQ
   mq_unlink(server1); 
   exit(0);
}

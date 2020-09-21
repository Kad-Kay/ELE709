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

int compute_prime (int arg) {
	int candidate = 2;
	int n = arg;
		while (1) {
			int factor;
			int is_prime = 1;
			/* Test primality by successive division. */
			for (factor = 2; factor < candidate; ++factor)
				if (candidate % factor == 0) {
					is_prime = 0;
					break;
				}
			/* Is this the prime number we’re looking for? */
			if (is_prime) {
				if (--n == 0)
				/* Return the desired prime number as the thread return value. */
				return candidate;
			}
			++candidate;
		}
	return -1;
}

int main(void *arg)
{
   mqd_t cRequest, sReturn;
   mode_t mode;
   int oflags;
   struct mq_attr c_attrs, s_attrs;
   struct Msg cMsg, sMsg;
   int cMsgs, sMsgs, inputValue, result, retcode;
   char input[10], buff[50], serverreq[50];   
   unsigned int priorityC;
   ssize_t n;

   c_attrs.mq_maxmsg = 1;      // Set max no of msg in queue to 10 msgs
   c_attrs.mq_msgsize = 50;     // Set the max msg size to 50 bytes
   s_attrs.mq_maxmsg = 1;
   s_attrs.mq_msgsize = 50;   

   oflags = (O_CREAT | O_RDWR) ;   // Set oflags to create queue (O_CREAT)
                                   //  and for read & write (O_RDWR)
   
   mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;     // Set mode to allow other threads
                                   //  (created by same user) to use the queue
				mq_unlink(client1);  //Destroy both MQs 
				mq_unlink(server1);
   cRequest = mq_open(client1, oflags, mode, &c_attrs);  // Create the client request MQ
   if (cRequest < 0) perror("cRequest mq_open:");  // Error checking
   printf("client created ,");
   sReturn = mq_open(server1, oflags, mode, &s_attrs);  // Create the server return MQ
   if (sReturn < 0) perror("sReturn mq_open:");  // Error checking
   printf("server created \n");
   
   
   
   while (1) {
		mq_getattr(cRequest, &c_attrs);     // Determine no. of msgs currently in the client request MQ
		cMsgs = c_attrs.mq_curmsgs;
		
		while (cMsgs-- > 0) {
			printf("Receiving\n");
			n = mq_receive(cRequest, serverreq, sizeof(serverreq), &priorityC);
			serverreq[n] = '\0';
			if (n < 0) perror("Receiver mq_receive:");  // Error checking
			inputValue = atoi(serverreq);
			
			if (inputValue == 0) {
				mq_close(cRequest);   //Close both MQs
				mq_close(sReturn);
				
				mq_unlink(client1);  //Destroy both MQs 
				mq_unlink(server1);
				
				pthread_exit(NULL);
			}
			 result = compute_prime(inputValue);
			 sprintf(buff, "%d", result);
			 printf("The prime number is %s\n", buff);
			 
			 snprintf(sMsg.str, 30, "The prime number is %s\n", buff);
			 //sprintf(sMsg.str, "The prime number is %d\n", result);  // Set up message to be sent
		     sMsg.priority = cMsgs--;    // Set up priority of this message
		     retcode = mq_send(sReturn, sMsg.str, (size_t)strlen(sMsg.str), sMsg.priority);
		     if (retcode < 0) perror("Sender mq_send:");  // Error checking		
			 //printf("**%s**\n", sMsg.str);
		}
   }  
   exit(0);
}

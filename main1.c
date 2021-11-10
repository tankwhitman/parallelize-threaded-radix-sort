/*
Joshua Whitman
JSW625
CLANG
program 3

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>


#define BUF_SIZE 1024
int count = 0;
int openFD; //the stream for writing so each thread can access it
union dtob
  {
    int value;
    char bytes[4];
  };
union dtob num;




pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditional = PTHREAD_COND_INITIALIZER;

//standard my print
void myPrint(const char *str)
{
    if (write(STDOUT_FILENO, str, strlen(str)) == -1)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }
}
int myPrintInt(const int val)
{
  char buf[BUF_SIZE];
  sprintf(buf, "%d", val);
  if((write(STDOUT_FILENO, buf, strlen(buf))) == -1)
  {
    return 0;
  }
  return 1;
}
//dup string to turn array of chars into char*
char * dupString(const char *val)
{
    int len = strlen(val);
    char * outStr = (char*) calloc(len+1, sizeof(char));
    if(outStr == NULL)
    {
        perror("Calloc");
        exit(EXIT_FAILURE);
    }
    strcpy(outStr, val);
}

int genRand(int low, int high)
{
  int rng = high - low + 1;
  double drnd = rand();
  int irnd = drnd/((double) RAND_MAX + 1) * rng;
  return low + irnd;
}

int calcDig(int number)
{
    int count = 0;
    do
    {
        count++;
        number /= 10;
    } while(number != 0);

    return count;

}

//genrand thread()
void * thread(void* high, void* low, void *count)
{
    int highVal = *((int *)high);
    int lowVal = *((int*)low);
    int numCount = *((int *) count);
    int out = 0;
    for(;;)
    {
        int chunksO = 0;
        for(chunksO; chunksO < 3; chunksO++)
        {
            num.value = genRand(lowVal, highVal);
            //CRITICAL SECTION
            pthread_mutex_lock(&mutex);
            /*while(count == 0)
            {
                pthread_cond_wait(&conditional, &mutex);
            }*/

            write(openFD, num.bytes, 4);

            pthread_mutex_unlock(&mutex);


            //END CRITICAL
            out++;
            if(out == count)
            {
                pthread_exit(NULL);
            }

        }
        nanosleep((const struct timespec[]){{0, 500}}, NULL);

    }


}





int main(int argc, char *argv[])
{
    int smallest=0;
    int largest=0;
    int numCreate=0;
    char buf[BUF_SIZE];

    srand(time(NULL));

    myPrint("Please enter the smallest number :");
    read(STDIN_FILENO, &buf, BUF_SIZE) ;
    smallest = atoi(buf);
    buf[0]= '\0';
    
    myPrint("\nPlease enter the largest number: ");
    read(STDIN_FILENO, &buf, BUF_SIZE) ;
    largest = atoi(buf);
    buf[0]= '\0';
    
    myPrint("\nPlease enter the number of integers to genterate: ");
    read(STDIN_FILENO, &buf, BUF_SIZE) ;
    numCreate = atoi(buf);
    buf[0]= '\0';

    int numDigSmall = calcDig(smallest);
    int numDigLarge = calcDig(largest);
    int numBuckets = numDigLarge - numDigSmall + 1;
    //myPrintInt(numBuckets);
    int valsPB = numCreate / numBuckets;
    int openFD;

    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH;
    mode_t flags = O_CREAT | O_TRUNC | O_WRONLY;

    openFD = open("data.dat", flags, perms);

    if(openFD < 0)
    {
        exit(EXIT_FAILURE);
    }
    //for loop for num of threads
    //you can spawn them to run at multiple times
    //however you  need to use mutexes to lock the critical section - the write part.
    for(int i = 0; i < numBuckets; i++)
    {
        

    }

    //wait for threads to exit and the close file and exit

    exit(EXIT_SUCCESS);


    return 0;
}
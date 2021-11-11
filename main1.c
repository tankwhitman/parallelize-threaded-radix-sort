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


struct {
    pthread_t threadID;
} *thread;

struct bucket_t{
    int bucketID;
    int lowrange;
    int highrange;
    int numDig;
} *bucket;


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
    return outStr;
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
void * threadFunc(void* bucketpass)
{
    myPrint("WOW IM IN A THREAD");
    
    struct bucket_t *tmpBuck = bucketpass;
    int highVal = tmpBuck->highrange;
    int lowVal = tmpBuck->lowrange;
    int numCount = tmpBuck->numDig;
    int out = 0;
    myPrint("\nhere is my high: ");
    myPrintInt(highVal);
    myPrint("\nLow: ");
    myPrintInt(lowVal);
    myPrint("\nCount");
    myPrintInt(numCount);
    
    union dtob num;
    for(;;)
    {
        int chunksO = 0;
        for(; chunksO < 3; chunksO++)
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
            if(out == numCount)
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
    bucket = calloc(numBuckets, sizeof(*bucket));

    int j = 0;
    int tmp = largest;
    int range[] = {10, 100, 1000, 10000, 100000, 1000000, 100000000, 99999999};
    for(int i = 0; i < numBuckets; i ++)
    {
        if(i == 0)
        {
            bucket[i].numDig = numDigSmall;
            bucket[i].lowrange = smallest;
            while(range[j] < smallest)
            {
                j++;
            }
            bucket[i].highrange = range[j] - 1;
            
        }
        else if(i +1 == numBuckets)
        {
            j++;
            bucket[i].numDig = bucket[i-1].numDig + 1;
            bucket[i].lowrange = range[j-1];
            bucket[i].highrange = largest;
            break;
        }
        else
        {
            j++;
            bucket[i].numDig = bucket[i-1].numDig + 1;
            bucket[i].lowrange = range[j-1];
            bucket[i].highrange = range[j] - 1;
        }
    }

    myPrint("\nACCEPTED VALUES\n");


    int valsPB = numCreate / numBuckets;
    

    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH;
    mode_t flags = O_CREAT | O_TRUNC | O_WRONLY;

    openFD = open("data.dat", flags, perms);

    if(openFD < 0)
    {
        perror("FILE DIDN'T OPEN");
        exit(EXIT_FAILURE);
    }
    //for loop for num of threads
    //you can spawn them to run at multiple times
    //however you  need to use mutexes to lock the critical section - the write part.
    thread = calloc(numBuckets, sizeof(*thread));
    int s;
    for(int i = 0; i < numBuckets; i++)
    {
        //create conditions for low - high bucket
        myPrint("\nMaking thread ");
        myPrintInt(i+1);
        s = pthread_create(&thread[i].threadID,NULL,threadFunc, &bucket[i]);
        

    }
    for( int i = 0; i < numBuckets; i++)
    {
        pthread_join(thread[i].threadID, NULL);
    }


    //wait for threads to exit and the close file and exit

    exit(EXIT_SUCCESS);


    return 0;
}
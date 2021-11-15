/*
Joshua Whitman
JSW625
CLANG
program 4

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

typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;


struct {
    pthread_t threadID;
} *thread;

struct bucket_t{
    int bucketID;
    int lowrange;
    int highrange;
    int numDig;
    Array *buckAr;
} *bucket;



void initArray(Array *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(int));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, int element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(int));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

int get_max (int a[], int n){
   int max = a[0];
   for (int i = 1; i < n; i++)
      if (a[i] > max)
         max = a[i];
   return max;
}


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

//radix sort algo
void radix_sort (int a[], int n){
   int bucket[10][10], bucket_cnt[10];
   int i, j, k, r, NOP = 0, divisor = 1, lar, pass;
   lar = get_max (a, n);
   while (lar > 0){
      NOP++;
      lar /= 10;
   }
   for (pass = 0; pass < NOP; pass++){
      for (i = 0; i < 10; i++){
         bucket_cnt[i] = 0;
      }
      for (i = 0; i < n; i++){
         r = (a[i] / divisor) % 10;
         bucket[r][bucket_cnt[r]] = a[i];
         bucket_cnt[r] += 1;
      }
      i = 0;
      for (k = 0; k < 10; k++){
         for (j = 0; j < bucket_cnt[k]; j++){
            a[i] = bucket[k][j];
            i++;
         }
      }
      divisor *= 10;
   }
}


//threads
//#2 sorter
void * sorter(void* bucketpass)
{
    struct bucket_t *tmpBuck = bucketpass;
    int size = tmpBuck->buckAr->used;
    int Arr[size];
    for(int i = 0; i < size; i ++)
    {
        Arr[i] = tmpBuck->buckAr->array[i]; 
    }
    
    radix_sort(Arr, size);

    for(int i = 0; i < size; i++)
    {
        myPrint("\n");
        myPrintInt(Arr[i]);
    }
    
    pthread_exit(result);
}

//#1 processor

void* processor()
{
    //open file
    //read file (have an iterator to know how many values)
    //determine num dig of each value and add it to array (1 array for each num of digit)
    //
    int openFD, count;
    
    
    int checker[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Array ar1;Array ar2;Array ar3;Array ar4;Array ar5;Array ar6;Array ar7;Array ar8;Array ar9; //ar(ray){digits in value}
    Array* myAr[] = {&ar1, &ar2, &ar3, &ar4, &ar5, &ar6, &ar7, &ar8, &ar9};
    initArray(&ar1, 10);
    initArray(&ar2, 10);
    initArray(&ar3, 10);
    initArray(&ar4, 10);
    initArray(&ar5, 10);
    initArray(&ar6, 10);
    initArray(&ar7, 10);
    initArray(&ar8, 10);
    initArray(&ar9, 10);
    count = 0;
    openFD = open("data.dat", O_RDONLY);
    while(read(openFD, num.bytes, 4) > 0)
    {
        count++;
        int numDig = calcDig(num.value);
        myPrintInt(num.value);
        myPrint(" and it has this many digits: ");
        myPrintInt(numDig);
        myPrint("\n");
        
        insertArray(myAr[numDig-1], num.value);        
    }
    //myPrint("MADE IT");

    bucket = calloc(9, sizeof(*bucket));
    for(int i = 0; i < 9; i++)
    {
        bucket[i].buckAr = myAr[i];
    }
    int s;
    thread = calloc(9, sizeof(*thread));
    for(int i = 0; i < 9; i++)
    {
        if((s = pthread_create(&thread[i].threadID, NULL, sorter, &bucket[i].buckAr->array) != 0))
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);            
        }
    }
    

    for( int i = 0; i < 9; i++)
    {
        
        pthread_join(thread[i].threadID, NULL);
        
        
    }
    myPrint("MADE IT");
    for(int i = 0; i < 9; i++)
    {
        for(int j =0 ; j < bucket->buckAr->used; j++  )
        {
            myPrint("MADE IT");
            myPrintInt(bucket[i].buckAr->array[j]); 
        }
        freeArray(myAr[i]);
    }
    exit(EXIT_SUCCESS);

}






int main()
{
    //spawn processor
    int s;
    pthread_t join;
    s = pthread_create(&join,NULL, processor, NULL);
    //join processor
    pthread_join(join, NULL);


    return 0;
}
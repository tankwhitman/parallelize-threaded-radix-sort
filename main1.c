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

#define BUF_SIZE 1024


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


int main(int argc, char *argv[])
{
    int smallest;
    int largest;
    int numCreate;
    do
    {
        myPrint("Please enter the smallest number greater than 0: ");
        read(STDIN_FILENO, &smallest, sizeof(int)) ;
    } while (smallest < 0);
    myPrint("\n");
    do
    {
        myPrint("Please enter the largest number less than 100000: ");
        read(STDIN_FILENO, &largest, sizeof(int)) ;

    } while (largest > 100000);
    myPrint("\n");
    do
    {
        myPrint("Please enter the number of integers to genterate: ");
        read(STDIN_FILENO, &numCreate, sizeof(int)) ;
    } while (numCreate < 0);
    
    myPrint("smallest:");
    myPrintInt(smallest);
    myPrint("\nlargest:");
    myPrintInt(largest);
    myPrint("\nnum to create:");
    myPrintInt(numCreate);
    
    exit(EXIT_SUCCESS);
}
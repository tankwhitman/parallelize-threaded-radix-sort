Program-4

Let's take a momentary break from the Flow Sensors and consider an application for threading that is fairly common: parallelizing an algorithm.

The algorithm in this case will be Radix Sort - a linear time sort with fairly stringent requirements:

everything being sorted must have the same number of characters or digits
the data set should be reasonably large
You don't need to work out how to parallelize the algorithm; but you do need to determine how to implement each thread.

You will need to write two programs.

Program 1 should create a data file containing an equal number of integers with differing lengths; Program 2 should read that file, sort them, and write the sorted integers into a new file.

In broad strokes, Program 1 should do the following:

Prompt the user for how many integers to generate
Prompt the user for the smallest integer to generate
Prompt the user for the largest integer to generate
Compute the number of digits in the smallest integer
Compute the number of digits in the largest integer
Create a file of integers called "data.dat" using the above parameters.
This program should take advantage of threads to accomplish this task. You should have two types of threads: the main thread and an output thread.

The main thread should do the following:
    Prompt the user for the details (smallest, largest, and number to create)
    Compute the number of digits in the smallest and largest integers
    Compute the difference between the smallest number of digits and largest number of digits. This is how many buckets you need to create
       For example, if you are generating values between 50 and 200 you would need to create 2 buckets .. one for 2 digit numbers and one for 3 digit numbers.
    Divide the total number of integers being created equally amongst each bucket. This is the bucket size.
    Open data.dat for write
    Loop from smallest digit count to largest digit count
         Spawn an output process to generate the appropriate number of digits with that digit count.

    Loop from smallest digit count to largest digit count
         Join the thread
    Close data.dat

The output threads must perform the following:
    Loop forever
         Set the chunks output to 0
         Loop while chucks output < chunk size (choose one larger than 1)
               Generate a random number in the appropriate range (determined by the digit count and the largest and smallest values)
               Output the value into the file
               Increment the total values output, if it is equal to the bucket size then exit the thread
          Sleep the thread for 500 nanoseconds
        

In broad strokes, Program 2 should do the following:

Read data from "data.dat" into an integer array
Determine the smallest element of data
Determine the largest element of data
Determine the total number of integers read.
Sort the array of integers with Radix Sort
Output the contents of the sorted array into the file "sorted.dat"
 Much of how you achieve this is left to the programmer.

These are the parts that aren't -- you should implement this program using multiple threads. 
You should have at least three types of threads: the main thread, a processor and a sorter.

The main thread must perform the following:
     spawn the processor
     join the processor

The processor thread must perform the following:
     determine how many sorter threads are necessary
     spawn each sorter thread
     join each sorter thread
     combine the results of the sorter threads into the final sorted list
Note: There should be one sorter  thread per digit in the largest number in the list. 
Basically, you are calling a sorter thread for 1 digit numbers, 2 digit numbers, 3 digit numbers, etc.
Make certain to pass each sorter thread appropriate parameters to identify the range of numbers they are 
supposed to sort.

A sorter thread must perform the following:
     copy all values of the proper length into a new array
     perform the radix sort on the new array
     set the exit status of the thread to the new array and its size.

 

 

Notice: there a few details that haven't been mentioned. This is largely because you can perform them in more
than one location and the program still be functional. These details include where the user is prompted for the 
necessary information, where the array is created, and where the array is actually being displayed to the user.

Feel free to incorporate additional threads if you feel that it is necessary.
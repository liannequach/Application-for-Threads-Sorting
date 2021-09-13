#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <time.h>

struct Params {
  int *start;
  size_t length;
  int depth;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // for synchronizing stdout from overlap.

void *sorter(void *para); // declare our thread from original list
void sort(int *start, size_t length); // sorting threads 1 and 2
void merge(int *start, int *mid, int *end); //merge to the parent thread
void sorter_multi(int *start, size_t length, int depth); //sorted the parent thread 

int main() {
   //call fun() and measure time taken by fun() 
   clock_t start_time, end_time;
   double cpu_time_used;
   start_time = clock(); //start run time
    
   static const unsigned int N = 20; //depth or how many numbers in the list
   int *data = malloc(N * sizeof(*data));
   unsigned int i;
   srand((unsigned)time(0));
   printf("Original list: \n");
   for (i=0; i<N; ++i) {
     data[i] = rand() % 100; // 100- 1 = 99, 99 will be the largest number in the list
     printf("%4d ", data[i]);
     if ((i+1)%10 == 0)    // 10 columns
       printf("\n");
   }
   
   //output for sorted first half sublist
   int *fhalf = data; //initilize first half
   sort(fhalf, N/2);
   printf("Sorted sublist 1: \n");
   for (i=0; i< (N/2); ++i) { //sort from first element to mid element of orginal list in increasing order
       printf("%4d ", data[i]);
   }
   
   //output for sorted second half sublist
   int *lhalf = data + N/2; //initilize last half
   sort(lhalf, N/2); 
   printf("\nSorted sublist 2: \n");
   for (i=N/2; i < N; ++i) {  //sort from mid element to last element of orginal list in increasing order
       printf("%4d ", data[i]);
   }
   
   //output for the sorted array
   sort(data, N); 
   printf("\nSorted list: \n");
   for (i=0; i<N; ++i) {
     printf("%4d ", data[i]);
     if ((i+1)%10 == 0)   // 10 columns
       printf("\n");
   }
 
   // Calculate the time taken by fun()
   end_time = clock(); //end run time
   cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC; 
   printf("fun() took %f seconds to execute \n", cpu_time_used);
   
   return 0;
}


// merging thread algorithm
void merge(int *start, int *mid, int *end) {
  int *result = malloc((end - start)*sizeof(*result));
  int *left = start, *right = mid, *temp = result;
  while (left != mid && right != end)
    *temp++ = (*left <= *right) ? *left++ : *right++;
  while (left != mid)
    *temp++ = *left++;
  while (right != end)
    *temp++ = *right++;
  memcpy(start, result, (end - start)*sizeof(*result));  // copy results
}

// entering parent thread
void sorter_multi(int *start, size_t length, int depth) {
   if (length < 2) // checking if there are more than one thread before entering to the parent thread
     return;
   if (depth <= 0) { 
     sorter_multi(start, length/2, 0);
     sorter_multi(start+length/2, length-length/2, 0);
   }
   else {
     struct Params params = { start, length/2, depth/2 };
     pthread_t thread;
     pthread_create(&thread, NULL, sorter, &params);  // create thread
     sorter_multi(start+length/2, length-length/2, depth/2); // sort again into the parent thread
     pthread_join(thread, NULL); // join on the launched thread
   }
   merge(start, start+length/2, start+length); //merge threads 1 & 2 into the parent thread
}

// passing parameters off to sorter algorithm
void *sorter(void *para) {
   struct Params *params = para;
   sorter_multi(params->start, params->length, params->depth);
   return para;
}

// sorting threads 1 & 2 from the original list 
void sort(int *start, size_t length) {
   sorter_multi(start, length, 2); // 2n - 1 where n is 2, there will be 3 threads in total 
}



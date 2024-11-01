#include "utility.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#define BUFF_SIZE 150

//** Write your victim function here */
// Assume secret_array[47] is your secret value
// Assume the bounds check bypass prevents you from loading values above 20
// Use a secondary load instruction (as shown in pseudo-code) to convert secret value to address

#define LINE_OFFSET 64
#define SECRET_VAL 32 
#define MAX_SECRET 110

uint32_t access_times[MAX_SECRET];
uint8_t array2[256 * LINE_OFFSET];

void victim(uint8_t * array, int idx){
    // No bounds check for cover channel
    (void) one_block_access((uint64_t) &array2[array[idx] * LINE_OFFSET]);
}

int main(int argc, char **argv)
{
    // Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *huge_page= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE |
    MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

    if (huge_page == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    // The first access to a page triggers overhead associated with
    // page allocation, TLB insertion, etc.
    // Thus, we use a dummy write here to trigger page allocation
    // so later access will not suffer from such overhead.
    *((char *)huge_page) = 1; // dummy write to trigger page allocation

    //** STEP 1: Allocate an array into the mmap */
    uint8_t *secret_array = (uint8_t *)huge_page;

    // Initialize the array
    for (uint8_t i = 0; i < 100; i++) {
        secret_array[i] = i;
    }

    for(int i = 0; i < 256; i++){
        access_times[i] = 0 ;
    }

    secret_array[47] = SECRET_VAL;

    //** STEP 2: Mistrain the branch predictor by calling victim function here */
    // To prevent any kind of patterns, ensure each time you train it a different number of times

    //** STEP 3: Clear cache using clflsuh from utility.h */
    for(int i = 0; i < 256 * LINE_OFFSET; i++){
        clflush((void*)&array2[i]);
    }

    //** STEP 4: Call victim function again with bounds bypass value */
    victim(secret_array, 47);

    //** STEP 5: Reload mmap to see load times */
    // Just read the mmap's first 100 integers

    for(int i = 0; i < MAX_SECRET; i++){
        access_times[i] = measure_one_block_access_time((uint64_t) &array2[i*LINE_OFFSET]);
    }
    /*
     *    for(int i = 0; i < MAX_SECRET; i++){
     *      printf("%d: %ld, \n", i, access_times[i]);
}
*/

    for(int i = 0; i < MAX_SECRET; i++){
        uint64_t access = access_times[i];
        if(access < 100){
            printf("Secret is  %d\n", i);
        }
    }


    return 0;
}


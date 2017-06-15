/**
 * generate.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Generates pseudorandom numbers in [0,LIMIT), one per line.
 *
 * Usage: generate n [s]
 *
 * where n is number of pseudorandom numbers to print
 * and s is an optional seed
 */
 
#define _XOPEN_SOURCE

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// constant
#define LIMIT 65536

int main(int argc, string argv[])
{
    // If user inputs an unexpected number of arguments (not 2, or 3), 
    // inform them of usage, and end the program
    if (argc != 2 && argc != 3)
    {
        printf("Usage: generate n [s]\n");
        return 1;
    }

    // Store the first argument: number of random numbers to output
    int n = atoi(argv[1]);

    // drand48() generates a random positive number between [0.0, 1.0) 
    // with two digits (a single decimal point). Contrary to erand48(),
    // drand48 needs to be initialized via an initialization function.
    
    // srand48(), seed48(), and lcong48() are such functions. 
    
    // If a seed was entered with the arguments, use it to initialize
    // srand48(). Convert it to an appropriate input argument of srand48()
    // with the atoi function
    // If a seed was not entered, use NULL as the input into srand48().
    
    if (argc == 3)
    {
        srand48((long int) atoi(argv[2]));
    }
    else
    {
        srand48((long int) time(NULL));
    }

    // Since drand48() generates a random float between [0,1), drand48()
    // * LIMIT will generate a float between [0, LIMIT). This is then
    // converted to an integer before outputted.
    
    // This sequence of steps is completed n times.
    
    for (int i = 0; i < n; i++)
    {
        printf("%i\n", (int) (drand48() * LIMIT));
    }

    // success
    return 0;
}
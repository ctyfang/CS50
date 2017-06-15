/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
       
#include <cs50.h>
#include <stdio.h>
#include "helpers.h"

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // TODO: implement an O(n^2) sorting algorithm
    // goal: implement selection sort
    
    // run the loop until all items have been "selected"
    for( int i = 0; i < n-1; i++){ 
    
        int current_lowest = values[i];
        int lowest_index = i;
    
        for( int j = i+1; j < n; j++){
            
            if( values[j] < current_lowest ){
                
                current_lowest = values[j];
                lowest_index = j;
                
            }
            
        }
        
        //printf("Smallest integer in unsorted portion is %i\n", values[lowest_index]);
        
        while( values[lowest_index] < values[lowest_index-1]){
            
            int temp = values[lowest_index-1];
            values[lowest_index-1] = values[lowest_index];
            values[lowest_index] = temp;
            lowest_index--;
            
        }
        
    }
    return;
}

/**
 * Returns true if value is in array of n values, else false.
 */

int find_mid(int low, int high){
    
    int mid = (high-low)/2 + low;
    
    return mid;
}

bool search(int value, int values[], int n)
{
    // TODO: implement binary search through array "values[]"" of size n
    // to find "value"
    
    sort(values, n);
    
    int upperlim = n;
    int lowerlim = 0;
    int middle_index = find_mid(lowerlim, upperlim);
    
    do{
        
        if(values[middle_index] < value)
            lowerlim = middle_index;
            
        else
            upperlim = middle_index;
        
        //printf("Upperlim is %i, Lowerlim is %i\n", upperlim, lowerlim);
        middle_index = find_mid(lowerlim, upperlim);
        
            
    }while(values[middle_index]!=value);
    
    
    return true;
}


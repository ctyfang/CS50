#include <stdio.h>
#include <cs50.h>

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
        
        printf("Smallest integer in unsorted portion is %i\n", values[lowest_index]);
        
        while( values[lowest_index] < values[lowest_index-1]){
            
            int temp = values[lowest_index-1];
            values[lowest_index-1] = values[lowest_index];
            values[lowest_index] = temp;
            lowest_index--;
            
        }
        
    }
    return;
}

int main(void){
    
    printf("How many items are to be sorted?\n");
    int n = GetInt();
    
    int list[n];
    
    for(int i = 0; i < n; i++){
        
        printf("Enter item number %i\n", i);
        list[i] = GetInt();
        
    }
    
    sort(list, n);
    
    return 0;
}
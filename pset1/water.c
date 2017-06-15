#include <stdio.h>
#include <cs50.h>


int main(void){
    
    int minutes;
    int bottles;
    
    printf("minutes:");
    minutes = GetInt();
    
    bottles = minutes * 12;
    
    printf("bottles:%i\n", bottles);
    
}
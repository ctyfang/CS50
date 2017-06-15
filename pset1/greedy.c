#include <stdio.h>
#include <cs50.h>
#include <math.h>


int main(void){
    
    float change_owed;
    float conversion_factor = 100;
    
    do {
        
        printf("How much change is owed?\n");
        change_owed = GetFloat();
        
    } while (change_owed < 0);
    
    printf("Change entered:%.24f\n", change_owed);
    
    int change_in_cents = round(change_owed * conversion_factor);
    //printf("In cents, that's %i\n", change_in_cents);
    
    int coin_values[3] = {25,10,5};
    int num_coins = 0;
    
    while (change_in_cents > 4){
        
        if(change_in_cents >= coin_values[0]){
                
            change_in_cents -= coin_values[0];
            num_coins++;                    
            
        }
        else if(change_in_cents >= coin_values[1]){
            
            change_in_cents -= coin_values[1];
            num_coins++;  
            
        }
        else if(change_in_cents >= coin_values[2]){
            
            change_in_cents -= coin_values[2];
            num_coins++;  
            
        }
        
        //printf("In cents, that's %i\n", change_in_cents);
    }
    
    if( change_in_cents > 0 ) num_coins += change_in_cents;
    
    printf("%i\n",num_coins);
    
}
#include <stdio.h>
#include <cs50.h>

int main(void){
    
    int pyramid_height;
    
    do {
        
       printf("Height:\n");    
       pyramid_height = GetInt(); 
       
    } while ((pyramid_height > 23) || (pyramid_height < 0));
    

    int j;
    int k;
    
    for (int i = 1; i <= pyramid_height; i++){
        
        for (k = pyramid_height - i; k > 0; k--){
            printf(" ");
        }
        
        for (j = i + 1; j > 0; j--){
            printf("#");
        }
        
        printf("\n");
    }
    
}
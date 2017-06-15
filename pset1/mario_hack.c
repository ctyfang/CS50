#include <stdio.h>
#include <cs50.h>

int main(void){
    
    
    // Prompt user for height
    int pyramid_height;
    
    do {
        
       printf("Height:");    
       pyramid_height = GetInt(); 
       
    } while ((pyramid_height > 23) || (pyramid_height < 0));
    
    
    // Print characters of each level, starting with the top
    int j;
    int k;
    
    for (int i = 1; i <= pyramid_height; i++){
        
        for (k = pyramid_height - i; k > 0; k--){
            printf(" ");
        }
        
        for (j = i; j > 0; j--){
            printf("#");
        }
        
        printf("  ");
        
        for (j = i; j > 0; j--){
            printf("#");
        }
        
        for (k = pyramid_height - i; k > 0; k--){
            printf(" ");
        }
        
        printf("\n");
    }
    
    return 0;
}
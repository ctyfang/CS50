#include <stdio.h>
#include <cs50.h>
#include <string.h>


int main(void){
    
    
    // Get name from user
    string name;
    name = GetString();
    
    
    // Find number of white-space
    int num_spaces = 0;
    
    for(int i = 0; i < strlen(name); i++){
        
        if(name[i] == 32)
        {
            num_spaces++;
            //printf("Space found at index %i\n", i);
        }
        
        
    }
    
    
                                    //printf("Number is spaces is %i\n", num_spaces);
    // Store indices of white-spaces
    int indices_found = 0;
    int counter = 0;
    int indices_array[num_spaces];
    
    while(indices_found < num_spaces){
        
        if(name[counter] == 32){
            
            indices_array[indices_found] = counter;
            //printf("Counter is %i\n",counter);
            //printf("Space index found was %i\n", indices_array[indices_found]);
            indices_found++;
            
        }
            
            
        counter++;
        
    }
    
    
    // Capitalize all characters in the string
    for(int i = 0; i < strlen(name); i++){
        
        //printf("%c",name[i]);
        if(name[i] >= 97)
            name[i] -= 32;
        
    }
    
    
    // Print initials
    printf("%c",name[0]); // First initial
    
    for(int i = 0; i < num_spaces; i++){ // Print all initials after first
        
        printf("%c",name[indices_array[i]+1]);
        
    }
    
    printf("\n");
    
    return 0;
    
    
    
}
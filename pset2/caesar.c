#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char* argv[]){
    
    if(argc != 2){
        printf("You didn't enter a key!\n");
        return 1;
    }
    else{
        
        // Extract the key from the input arguments
        int key = 0;
        
        key = atoi(argv[1]);
        
        //printf("Key is %i\n", key);
        key %= 26;
        
        
        //printf("Enter a string for encryption:\n");
        string message = GetString();
        
        int ascii_conversion;
        
        for(int i = 0; i < strlen(message); i++){
            
            // Each iteration addresses a single character in the given string
            if(isalpha(message[i])){
                
                // Is letter capitalized?
                if(message[i] <96)
                    ascii_conversion = 65;
                else 
                    ascii_conversion = 97;
                    
                message[i] -= ascii_conversion;
                message[i] += key;
                message[i] %= 26;
                message[i] += ascii_conversion;
                
            }
            
        }
        
        printf("%s\n", message);
        
        return 0;
    }
    
}
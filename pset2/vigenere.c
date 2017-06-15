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
        string key = argv[1];
        
        // Convert key to upper case and check for symbols and numbers
        for(int i = 0; i < strlen(key); i++){
            
            if(isalpha(key[i])){
                
                if(key[i] > 96) // Is letter lower case
                    key[i] -= 32; // Change to upper case
                
            }
            else{
                
                printf("You didn't enter a key!\n");
                return 1;
                
                
            }
        }
        
        // Get user input
        //printf("Enter a string for encryption:\n");
        string message = GetString();
        
        // Determine shift of each character using an overlayed key sequence
        int char_shifts[strlen(message)];
        int j = 0;
        
        for(int i = 0; i < strlen(message); i++){
            
            // Each iteration addresses a single character in the given message
            if(isalpha(message[i])){
                
                char_shifts[i] = key[j % strlen(key)] - 65;
                j++;
                
            }
            else{
                
                char_shifts[i] = 0;
                
            }
            
        }
        
        // print the message and the shifts
         //printf("%s\n", message);
        /*for(int i = 0; i < strlen(message); i++){
            
            printf("%i ", char_shifts[i]);
            
        }*/
        
        // Shift the characters in the message
        int ascii_conversion;
        
        for(int i = 0; i < strlen(message); i++){
            
            if(isalpha(message[i])){
                
                // Is letter capitalized?
                if(message[i] < 96)
                    ascii_conversion = 65;
                else 
                    ascii_conversion = 97;
                    
                message[i] -= ascii_conversion; // converts ascii value to 0-25
                message[i] += char_shifts[i]; // shifts ascii value
                message[i] %= 26; // accounts for overflow 
                message[i] += ascii_conversion; // converts back to appropriate ascii
                
            }
            
            
        }
        
        // Print shifted message
        printf("%s\n", message);
            
        
        return 0;
    }
    
}
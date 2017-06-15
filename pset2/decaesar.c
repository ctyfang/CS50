#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// decrypts a caesar-encrypted message by brute force
void caesar(string message, int key);

int main(void){
    
    string message = GetString();
    
    for(int j = 1; j < 26; j++){
        
        caesar(message,j);
        
    }
    
    return 0;
}

void caesar(string message, int key){

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
            
    
        
    printf("Key= %i, Msg= %s\n", key, message);
    
}
/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image. Stores each image in a new file with a three digit filename. E.g. 000, 001, 002, etc.
 */

/**
 * Important facts:
 * -JPEGs are stored in 512 byte "blocks"
 * -The start of a JPEG is indicated by 0xff 0xd8 0xff, followed by 0xe[0-f], i.e. first four bits are 0111 of last byte
 * -Headers are block-aligned
 * -Blocks can look like 1 of 2 things: contain 1) metadata and part of an image, 2) part of an image
 * -sprintf (buffer, "%d plus %d is %d", a, b, a+b), stores the string into a char* called buffer, and returns the number of
 *  characters written
 * -For our purposes, sprintf (curr_name, "00%i",i), would update the filename everytime a new file is found
*/

/**
 * What's wrong atm? 
 * -bytes_count isn't right
 * -i.e. if you're on a metadata trail, and on the second byte, you discover that it's not metadata.. that data was not written
 *  to the outfile and the flag isn't set to 0
 * -atm, with every byte read, you're doing 1 of 2 things: 1) tailing potential metadata, 2) writing
 * -formatting of output files is wrong
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef uint8_t  BYTE;

int main(int argc, char* argv[])
{
    BYTE buffer[512];
    
    // error checking
    if (argc != 1)
    {
        printf("Usage: ./recover");
        return 1;
    }
    
    // open input file 
    FILE* inptr = fopen("card.raw", "r");
    if (inptr == NULL)
    {
        printf("Could not open card.raw\n");
        return 2;
    }

    // loop through each block in the raw data
    int num_files   = 0;
    char outfile[8];
    sprintf(outfile,"%03d.jpg", num_files);
    FILE* outptr = fopen(outfile, "w");
    
    int started_reading = 0;
            
    while(fread(&buffer, sizeof(BYTE), 512, inptr)){
        
        
            
        // check first three bytes
        if((buffer[0] == 0xff) && (buffer[1] == 0xd8) && (buffer[2] == 0xff)){
            
            if(!started_reading)
                started_reading = 1;
            
            if((buffer[3] >> 4) & 0xe){
                if(num_files){
                    fclose(outptr);
                    sprintf(outfile,"%03d.jpg", num_files);
                    outptr = fopen(outfile, "w");
                
                }
                    
                fwrite(&buffer, sizeof(BYTE), 512, outptr);
                num_files++;
            }
            else
                break;
            
        }
        else if(num_files < 50 && started_reading){
            
            fwrite(&buffer, sizeof(BYTE), 512, outptr);
            
        }
        else{
            
        }
        
    }
        
            
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;    
    
}

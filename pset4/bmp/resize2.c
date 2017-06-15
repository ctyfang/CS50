/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    if((n < 1) || (n > 100)){
        
        printf("Scaling factor should be between 1 and 100, inclusive.\n");
        return 1;
    }
    
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    BYTE buffer[50];
    
    fread(buffer, sizeof(BYTE), 50, inptr);
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    // create oufile's BF and BI headers
    BITMAPFILEHEADER bf_out;
    BITMAPINFOHEADER bi_out;
    
    bf_out = bf;
    bi_out = bi;
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // modify outfile's headers
    bi_out.biWidth *= n;
    bi_out.biHeight *= n;
    int padding_old =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_new =  (4 - (bi_out.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi_out.biSizeImage = ((bi_out.biWidth * sizeof(RGBTRIPLE)) + padding_new) * abs(bi_out.biHeight); // file size
    bf_out.bfSize = bi_out.biSizeImage + 54; // file size
    
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_out, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_out, sizeof(BITMAPINFOHEADER), 1, outptr);


    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int count2 = 0; count2 < n; count2++){
            
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
    
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                for (int count = 0; count < n; count++){
                    // write RGB triple to outfile n times
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
                    
            }
            
            for (int k = 0; k < padding_new; k++)
            {
                fputc(0x00, outptr);
            }
            
            fseek(inptr, -bi.biWidth*sizeof(RGBTRIPLE), SEEK_CUR);
        }    
            

        // skip over padding, if any
        fseek(inptr, (bi.biWidth*sizeof(RGBTRIPLE)) + padding_old, SEEK_CUR);

        // then add it back (to demonstrate how)
        
    }
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

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
#include <cs50.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    int   n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];

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

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    BITMAPINFOHEADER bi_out;
    BITMAPFILEHEADER bf_out;
    
    bi_out = bi;
    bf_out = bf;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // determine padding for incoming scanlines
    bi_out.biWidth *= n;
    bi_out.biHeight *= n;
    // determine padding for new scanlines
    int padding_old =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_new =  (4 - (bi_out.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi_out.biSizeImage = ((bi_out.biWidth * sizeof(RGBTRIPLE)) + padding_new)*abs(bi_out.biHeight); // file size
    bf_out.bfSize = bi_out.biSizeImage + 54; // file size
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_out, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_out, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
            
            for (int m = 0; m < n; m++){
                
                for (int j = 0; j < bi_out.biWidth; j++)
                {
                    // temporary storage
                    RGBTRIPLE triple;
    
                    // read RGB triple from infile
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    
                    for (int l = 0; l < n; l++){
                        // write RGB triple to outfile n number of times horizontally
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    }
                        
    
                }
                
                for (int k = 0; k < padding_new; k++)
                {
                    fputc(0x00, outptr);
                }
                
                fseek(inptr, -bi_out.biWidth*sizeof(RGBTRIPLE), SEEK_CUR);
            }
        
            // skip over padding, if any
            fseek(inptr, padding_old + (bi_out.biWidth*sizeof(RGBTRIPLE)), SEEK_CUR);
        // iterate over pixels in scanline
        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

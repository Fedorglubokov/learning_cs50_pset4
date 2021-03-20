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
#include <math.h>
#include <unistd.h>

#include "bmp.h"

int enlarge(int n, char* infile, char* outfile);
int compress_10_times(char* infile, char* outfile);


int enlarge(int n, char* infile, char* outfile)
{
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

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    int initial_size = bf.bfSize;
    int initial_width = bi.biWidth;
    int initial_height = bi.biHeight;

    bf.bfSize = (initial_size-sizeof(BITMAPFILEHEADER)-sizeof(BITMAPINFOHEADER))*n*n+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    bi.biWidth = initial_width*n;
    bi.biHeight = initial_height*n;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // determine number of bytes passed
    int bytes_passed_in_scanline;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        printf("\n");
        for(int u=0; u<n; u++)
        {
            bytes_passed_in_scanline = 0;

            // iterate over pixels in scanline
            for (int j = 0; j < initial_width; j++)
            {

                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                bytes_passed_in_scanline = bytes_passed_in_scanline + sizeof(RGBTRIPLE);

                // write RGB triple to outfile
                for(int m=0; m<n; m++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    printf("%i_%i_%i ", triple.rgbtBlue, triple.rgbtGreen, triple.rgbtRed);
                }
            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);
            bytes_passed_in_scanline = bytes_passed_in_scanline + padding;

            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }
            if(u<(n-1))
            {
                fseek(inptr, -bytes_passed_in_scanline, SEEK_CUR);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

int compress_10_times(char* infile, char* outfile)
{
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

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    int initial_size = bf.bfSize;
    int initial_width = bi.biWidth;
    int initial_height = bi.biHeight;

    bf.bfSize = (int) (initial_size-sizeof(BITMAPFILEHEADER)-sizeof(BITMAPINFOHEADER))/100+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    bi.biWidth = (int) initial_width/10;
    bi.biHeight = (int) initial_height/10;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding_out =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_in = (4 - (initial_width * sizeof(RGBTRIPLE)) % 4) % 4;

    // determine number of bytes passed
    int bytes_passed_in_scanline;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        bytes_passed_in_scanline=0;
        printf("\nnew string\n");
        // iterate over pixels in scanline
        for (int j = 0; j < initial_width; j++)
        {
            // temporary storage
            RGBTRIPLE triple;
            int blue=0;
            int green=0;
            int red=0;

            // read RGB triple from infile 10 times
            for(int u = 0; u<10 /*|| u<initial_width*/; u++)
            {
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                blue = blue + triple.rgbtBlue;
                green = green + triple.rgbtGreen;
                red = red + triple.rgbtRed;
                bytes_passed_in_scanline = bytes_passed_in_scanline + sizeof(RGBTRIPLE);
                printf("%i_%i_%i ", blue, green, red);
            }
            /*if(initial_width-j>=10)
            {*/
                blue = blue/10;
                green = green/10;
                red = red/10;
                j=j+10;
            /*}
            else
            {
                j=initial_width;
            }*/

            triple.rgbtBlue = blue;
            triple.rgbtGreen = green;
            triple.rgbtRed = red;

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            printf("%i_%i_%i ", triple.rgbtBlue, triple.rgbtGreen, triple.rgbtRed);
        }

        // skip over padding, if any
        fseek(inptr, padding_in, SEEK_CUR);
        //bytes_passed_in_scanline = bytes_passed_in_scanline + padding;

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding_out; k++)
        {
            fputc(0x00, outptr);
        }
        //fseek(inptr, -bytes_passed_in_scanline+initial_width, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

int main(int argc, char* argv[])
{
    //remember a
    float a = atof(argv[1]);
    int n=ceil(a*10);

    // ensure proper usage
    if (argc != 4 || a<0 || a>1)
    {
        printf("Pls give me float a from 0 to 1, infile and outfile \n");
        return 1;
    }

    // remember filenames
    char* incoming = argv[2];
    char* outcoming = argv[3];
    char* temp = "temp.bmp";

    printf("Enlarged file:");
    // enlarge infile a*10 times
    enlarge(n, incoming, temp);

    printf("\nComressed file:");
    // compress the result 10 times
    compress_10_times(temp, outcoming);

    printf("\n");
    unlink(temp);
}

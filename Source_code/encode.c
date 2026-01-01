#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }
    printf("INFO: Opening %s\n", encInfo->src_image_fname );

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }
    printf("INFO: Opening %s\n", encInfo->secret_fname );

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    printf("INFO: Opening %s\n", encInfo->stego_image_fname );
    printf("INFO: Done\n");
    return e_success;
}

//function definition to check operation type from CLA
OperationType check_operation_type( char *argv[] )
{
    if( strcmp(argv[1], "-e") == 0 )
        return e_encode;
    else
        return e_unsupported;
}

//function definition to read and validate encode CLA and file
Status read_and_validate_encode_args( char *argv[], EncodeInfo *encInfo )
{
    if( strstr(argv[2],".") != NULL )
    {
        if( strcmp(strstr(argv[2],"."), ".bmp")  == 0 )
        {
            encInfo->src_image_fname = argv[2];
        }
        else
            return e_failure;
    }
    else
        return e_failure;

    if( strstr( argv[3], ".") != NULL )
    {
        if( encInfo->size_extn = strlen(strstr(argv[3],".") ))
            strcpy( encInfo->extn_secret_file, strstr( argv[3], "."));

        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }

    if( argv[4] == NULL )
    {
        printf("INFO: Output File not mentioned. Creating stego_img.bmp as default\n");
        encInfo->stego_image_fname = "stego_img.bmp";
    }
    else
    {
        char *ptr, *buf = (char*)malloc(50);
        buf = argv[4];

        if(  argv[4] )
        {
            ptr = strstr(argv[4],".");
        }
        if( ptr != NULL )
        {
            if( strcmp( ptr , ".bmp") == 0 )
            {
                encInfo->stego_image_fname = argv[4];
            }
            else
            {
                return e_failure;
            }
        }
        else
        {
            strcat( buf, ".bmp");
            encInfo->stego_image_fname = buf;
        }
    }
    return e_success;
}

//function definition to check capacity of source image to handle encoding secret file
Status check_capacity( EncodeInfo *encInfo )
{
    encInfo->image_capacity = get_image_size_for_bmp( encInfo->fptr_src_image );
    fseek( encInfo->fptr_secret, 0, SEEK_END );
    encInfo->size_secret_file = ftell( encInfo->fptr_secret );

    printf("INFO: Checking for %s size\n", encInfo->secret_fname );
    if( encInfo->fptr_secret == 0 )
    {
        perror("INFO: Done. Empty\n");
        return e_failure;
    }
    else
    {
        printf("INFO: Done. Not Empty\n");
        printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname );
        if( encInfo->image_capacity >= 54+((2+10+4+4+4+encInfo->size_secret_file)*8) )
        {
            printf("INFO: Done. Found OK\n");
            return e_success;
        }
        else
        {
            return e_failure;
        }
    }
    return e_success;

}

//function definition to copy source bmp header
Status copy_bmp_header( FILE *fptr_src_image, FILE *fptr_stego_image )
{
    printf("INFO: Copying Image Header\n");
    char buffer[54];
    fseek( fptr_src_image, 0, SEEK_SET );
    fread( buffer, 1, 54, fptr_src_image );
    fwrite( buffer, 1, 54, fptr_stego_image );
    printf("INFO: Done\n");
    return e_success;
}

//function definition to encode magic string 
Status encode_magic_string( const char *magic_string, EncodeInfo *encInfo )
{
    printf("INFO: Encoding Magic String Signature\n");
    char data[3] = MAGIC_STRING;
    if( encode_data_to_image( data, 2, encInfo->fptr_src_image, encInfo->fptr_stego_image ) ==  e_success )
    { 
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;   
}

//function definition to encode secret file extension size 
Status encode_secret_file_extn_size( long extn_size, EncodeInfo *encInfo )
{
    printf("INFO: Encoding %s File Extension Size\n", encInfo->secret_fname);
    char data[10];
    if( my_atoi( extn_size, data ) == e_success ) 
        if( encode_data_to_image( data, sizeof(long), encInfo->fptr_src_image, encInfo->fptr_stego_image ) ==  e_success )
        { 
            printf("INFO: Done\n");
            return e_success;
        }
        else
            return e_failure;   

}

//function definition to encode secret file extension 
Status encode_secret_file_extn( const char *file_extn, EncodeInfo *encInfo )
{
    printf("INFO: Encoding %s File Extension\n", encInfo->secret_fname);
    char data[encInfo->size_extn+5];
    strcpy( data, encInfo->extn_secret_file );
    if( encode_data_to_image( data, encInfo->size_extn, encInfo->fptr_src_image, encInfo->fptr_stego_image ) ==  e_success ) 
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;   
}

//function definition to encode secret file size
Status encode_secret_file_size( long file_size, EncodeInfo *encInfo )
{
    printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    char data[10];
    if( my_atoi( file_size, data ) == e_success ) 
        if( encode_data_to_image( data, sizeof(long), encInfo->fptr_src_image, encInfo->fptr_stego_image ) ==  e_success ) 
        {
            printf("INFO: Done\n");
            return e_success;
        }
        else
            return e_failure;   
}

//function definition to convert string to integer
Status my_atoi( long file_size, char* data )
{
    char t_data[10]={48,48,48,48,48,48,48,48,48,48};
    long temp = file_size, i=0,j=0;
    if( file_size == 0 )
    {
        data[0] = 49;
        return e_success;
    }
    while( temp )
    {
        temp = file_size%10;
        t_data[i++] = 48 + temp;
        file_size = file_size/10;
    }

    for( i=7; i>=0; i-- )
    {
        data[j] = t_data[i]; 
        j++;
    }
    return e_success;
}

//function definition to encode secret file data
Status encode_secret_file_data( EncodeInfo *encInfo )
{
    printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
    char data[encInfo->size_secret_file+10];
    int size =  encInfo->size_secret_file-1;
    fseek( encInfo->fptr_secret, 0, SEEK_SET );
    fread( data, 1,  size, encInfo->fptr_secret );
    if( encode_data_to_image( data, size, encInfo->fptr_src_image, encInfo->fptr_stego_image ) ==  e_success )
    { 
        printf("INFO: Done\n");
        fclose( encInfo->fptr_secret);
        return e_success;
    }
    else
        return e_failure;   
}

//function definition to encode data to destination file
Status encode_data_to_image( char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image )
{
    char buffer[8];
    int n;
    for( int i=0; i<size; i++ )
    {
        n=fread( buffer, 1, 8, fptr_src_image );
        if( n != 8 )
        {
            return e_failure;
        }
        else
        {
            if( encode_byte_to_lsb( data[i], buffer ) ==  e_success )
            {
                fwrite( buffer, 1, 8, fptr_stego_image );
            }
            else
            {
                return e_failure;
            }
        }
    }
    return e_success;
}

//function definition to encode copy remaining source image data to desitination file
Status copy_remaining_img_data( FILE *fptr_src_image, FILE *fptr_stego_image )
{
    printf("INFO: Copying Left Over Data\n");
    int start, end, n=19,count=0;
    unsigned long total ;
    FILE *t_fptr_src_image = fptr_src_image; 
    start = ftell( t_fptr_src_image );

    fseek( t_fptr_src_image, 0, SEEK_END );
    end = ftell( t_fptr_src_image );
    total=end-start;
    fseek( fptr_src_image, 0, SEEK_SET );
    fseek( fptr_src_image, start, SEEK_CUR );
    start = ftell( fptr_src_image );
    char *buffer = (char*) malloc( total+10 );
    n=fread(buffer, 1, total, fptr_src_image );
    if( n == total )
    {
        fwrite(buffer, 1, total, fptr_stego_image );
    }
    else
    {
        return e_failure;
    }
    printf("INFO: Done\n");
    fclose( fptr_src_image);
    fclose( fptr_stego_image );
    printf("INFO: ####### Encoding Done Successfully #######\n");
    return e_success;
}

//function definition to encode each byte of data to destination file
Status encode_byte_to_lsb ( char data, char *image_buffer )
{
    int temp=0;
    for( int i=0; i<8; i++ )
    {
        image_buffer[i] = (image_buffer[i] & 0xFE);
        temp = ((1<<i) & data);
        image_buffer[i] = image_buffer[i] | (temp>>i);
    }
    return e_success;
}

//function definition to handle all the encoding functions and progress to destination file
Status do_encoding( EncodeInfo *encInfo )
{
    printf("INFO: ####### Encoding Procedure Started #######\n");
    if( check_capacity ( encInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                       ERROR : CAPACITY FAILED                               \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    if( copy_bmp_header( encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : COPY HEADER FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    const char ms[5] = MAGIC_STRING;

    if( encode_magic_string( ms, encInfo ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : MAGIC STRING FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    if( encode_secret_file_extn_size( encInfo->size_extn, encInfo ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : EXTN SIZE FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    const char extn[10] ;
    if( encode_secret_file_extn( extn, encInfo ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : EXTN COPY  FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    if( encode_secret_file_size( encInfo->size_secret_file, encInfo ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : SECRET SIZE FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    if( encode_secret_file_data( encInfo ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : SECRET DATA FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }
    if( copy_remaining_img_data( encInfo->fptr_src_image, encInfo-> fptr_stego_image ) == e_success )
    {

    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : REMAINING DATA FAILED                         \n");    
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }
    printf("----------------------------------------------------------------------------------------\n");
    return e_success;
}

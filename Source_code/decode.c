#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"decode.h"
#include"common.h"

//function definition to check decode operation type
OperationType d_check_operation_type( char *argv[] )
{
    if( strcmp (argv[1], "-d")  == 0 )
        return e_decode;
    else
        return e_unsupported;
}

//function definition to read and validate CLA and files
Status read_and_validate_decode_args( char *argv[],  DecodeInfo *decInfo )
{
    if( strstr( argv[2],"." ) != NULL )
    {
        if( strcmp( strstr( argv[2], "."), ".bmp" ) == 0 )
        {
            decInfo->d_src_image_fname = argv[2];
        }
        else
            return e_failure;
    }
    else
        return e_failure;

    if( argv[3] == NULL )
    {
        strcpy(decInfo->d_secrect_fname ,"decoded");
        decInfo->flag = 2;
    }
    else
    {
        if( strstr(argv[3],".") != NULL )
        {
            printf("NO ""."" file extension in arguments\n");
            return e_failure;
        }
        strcpy(decInfo->d_secrect_fname,argv[3]);
    }
    return e_success;
}

//function definition to open all required files to decode
Status d_open_files( DecodeInfo *decInfo )
{
    decInfo->d_fptr_src_image = fopen( decInfo->d_src_image_fname, "r" );

    if (decInfo->d_fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_src_image_fname);

        return e_failure;
    }
    printf("INFO: Opening required files\n");
    printf("INFO: Opening %s\n", decInfo->d_src_image_fname );
    return e_success;
}

//function definition to decode magic string
Status decode_magic_string(  DecodeInfo *decInfo )
{
    printf("INFO: Decoding Magic String Signature\n");
    fseek( decInfo->d_fptr_src_image, 54, SEEK_CUR);
    char string[3];
    if ( decode_data_to_file( string, 2, decInfo ) == e_success )
    {
        if( strcmp(string,MAGIC_STRING ) == 0)
        {
            printf("INFO: Done\n");
            return e_success;
        }
    }
    else
        return e_failure;
}

//function definition to decode data to destination file
Status decode_data_to_file( char *string, int size, DecodeInfo *decInfo )
{
    char image_buffer[8],array[size+10];
    int n=1;
    for( int i=0; i<size; i++ )
    {
        n = fread(image_buffer, 1, 8, decInfo->d_fptr_src_image );
        if( n == 8 )
        {
            array[i] = decode_byte_to_lsb( image_buffer);
            string[i] = array[i];
        }
        else
        {
            printf("ERROR : decode_data_to_file failed\n");
            return e_failure;
        }
    }
    return e_success;
}

//funtion definition to decode secret file extension
Status decode_secret_file_extn ( DecodeInfo *decInfo )
{
    printf("INFO: Decoding Output File Extension\n");
    char string[decInfo->d_size_extn+1];
    string[decInfo->d_size_extn] = '\0';
    char file_name[20];
    strcpy( file_name, decInfo->d_secrect_fname );
    if( decode_data_to_file( string, decInfo->d_size_extn, decInfo ) == e_success )
    {
        printf("INFO: Done\n");
        string[decInfo->d_size_extn] = '\0';
        strcat( file_name, string );
        strcpy(decInfo->d_secrect_fname,file_name);
        if( decInfo->flag == 2 )
            printf("INFO: Out File not mentioned. Creating decoded.<extension> as default\n");
        decInfo->d_fptr_secret = fopen(file_name,"w");
        if( decInfo->d_fptr_secret == NULL )
        {
            perror("fopen");
            printf("Unable to open %s\n", file_name );
            return e_failure;
        }
        printf("INFO: Opened %s\n", decInfo->d_secrect_fname);
        printf("INFO: Done. Opened all required files\n");
        return e_success;
    }
    else
        return e_failure;
}

//function definition to decode secret file extension size
Status decode_secret_file_extn_size( DecodeInfo *decInfo )
{
    printf("INFO: Decoding Output File Extension Size\n");
    char string[10];

    if( decode_data_to_file(  string , sizeof(long), decInfo ) == e_success )
    {
        decInfo->d_size_extn = atoi(string);
        decInfo->d_size_secret_file = atoi( string );
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

//function definition to decode secret file size
Status decode_secret_file_size( DecodeInfo *decInfo )
{
    printf("INFO: Decoding %s File Size\n", decInfo->d_secrect_fname);
    char string[10];

    if( decode_data_to_file(  string , sizeof(long), decInfo ) == e_success )
    {
        decInfo->d_size_secret_file = atoi( string );
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

//function definition to decode secret file data
Status decode_secret_file_data( DecodeInfo *decInfo )
{
    printf("INFO: Decoding %s File Data\n", decInfo->d_secrect_fname);
    int size=decInfo->d_size_secret_file-1;
    char string[size +10],ch;

    if( decode_data_to_file( string, decInfo->d_size_secret_file, decInfo ) == e_success )
    {
        fwrite( string, 1, size, decInfo->d_fptr_secret); 
        fseek(decInfo->d_fptr_secret, 0, SEEK_SET );
        fclose( decInfo->d_fptr_src_image);
        fclose( decInfo->d_fptr_secret);
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

//function definition to decode each byte from source file
Status decode_byte_to_lsb(  char *image_buffer )
{
    char data = 0;
    for( int i=7; i>=0; i--)
    {
        image_buffer[i] = image_buffer[i] & 0x01;
        data = data | (image_buffer[i] << i);
    }
    return data;
}

//function definition to do all decoding function and processes to achieve secret file
Status do_decoding( DecodeInfo *decInfo )
{
    printf("INFO: ####### Decoding Procedure Started #######\n");
    if( d_open_files( decInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                       ERROR : OPEN FILES FAILED                               \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }


    if( decode_magic_string( decInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : MAGIC STRING FAILED                         \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }
    if( decode_secret_file_extn_size( decInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : SECRET EXTN SIZE FAILED                         \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }
    if( decode_secret_file_extn( decInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : SECRET EXTN FAILED                         \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }
    if( decode_secret_file_size( decInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : SECRET SIZE FAILED                         \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    if( decode_secret_file_data( decInfo ) == e_success )
    {
    }
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("                      ERROR : SECRET DATA FAILED                         \n");
        printf("-------------------------------------------------------------------------------\n");
        return e_failure;
    }

    printf("INFO: ####### Decoding Done Successfully  #######\n");
    printf("----------------------------------------------------------------------------------------\n");
    return e_success;
}

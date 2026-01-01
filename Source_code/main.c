#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "encode.h"
#include "types.h"

//main fuction with CLA
int main( int argc, char *argv[])
{
    //declare structures
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;

    //condition to check no. of arguments passed
    if( argc >=3 )
    {
        //check encode operation type from CLA
        if( check_operation_type ( argv ) == e_encode )
        {
            if( argc >= 4 );
            else
            {
                printf("ENCODING : %s -e <.bmp file> < .file > [output file]\n", argv[0]);
                return e_failure;
            }

            printf("----------------------------------------------------------------------------------------\n");
            printf("                                        ENCODING\n");
            printf("----------------------------------------------------------------------------------------\n");
            //read and validate CLA and files
            if( read_and_validate_encode_args(argv, &encInfo) == e_success )
            {
                //check open files operations
                if( open_files( &encInfo ) == e_success )
                {
                    //check actual encoding processes
                    if( do_encoding( &encInfo ) == e_success )
                    {

                    }
                    else
                    {
                        printf("-------------------------------------------------------------------------------\n");
                        printf("                       ERROR : ENCODING FAILED                               \n");
                        printf("-------------------------------------------------------------------------------\n");

                    }
                }
                else
                {
                    printf("-------------------------------------------------------------------------------\n");
                    printf("                       ERROR : OPEN FILES FAILED                               \n");
                    printf("-------------------------------------------------------------------------------\n");
                }
            }
            else
            {
                printf("                       ERROR : VALIDATION FAILED                               \n");
                printf("ENCODING : %s -e <.bmp file> < .file > [output file]\n", argv[0]);
                printf("-------------------------------------------------------------------------------\n");
            }

        }
        //check decode operation type from CLA
        else  if ( d_check_operation_type( argv ) == e_decode )
        {
            printf("----------------------------------------------------------------------------------------\n");
            printf("                                        DECODING\n");
            printf("----------------------------------------------------------------------------------------\n");
            //read and validate CLA and files
            if( read_and_validate_decode_args(argv, &decInfo) == e_success )
            {
                //check actual decoding processes
                if( do_decoding ( &decInfo ) == e_success )
                {
                }
                else
                {
                    printf("-------------------------------------------------------------------------------\n");
                    printf("                       ERROR : DECODING FAILED                               \n");
                    printf("-------------------------------------------------------------------------------\n");
                }

            }
            else
            {
                printf("                       ERROR : VALIDATION FAILED                               \n");
                printf("DECODING : %s -d <.bmp file> [output file]\n", argv[0]);
                printf("-------------------------------------------------------------------------------\n");
            }

        }
        else
        {
            printf("ENCODING : %s -e <.bmp file> < .file > [output file]\n", argv[0]);
            printf("DECODING : %s -d <.bmp file> [output file]\n", argv[0]);
        }
    }
    //for failed requirements from CLA
    else
    {
        if( argc > 1 )
        {
            if( strcmp(argv[1],"-e" ) == 0 )
            {
                printf("ENCODING : %s -e <.bmp file> < .file > [output file]\n", argv[0]);
            }
            else if( strcmp(argv[1],"-d" ) == 0 )
            {
                printf("DECODING : %s -d <.bmp file> [output file]\n", argv[0]);
            }
            else
            {
                printf("ENCODING : %s -e <.bmp file> < .file > [output file]\n", argv[0]);
                printf("DECODING : %s -d <.bmp file> [output file]\n", argv[0]);
            }
        }
        else
        {
            printf("ENCODING : %s -e <.bmp file> < .file > [output file]\n", argv[0]);
            printf("DECODING : %s -d <.bmp file> [output file]\n", argv[0]);
        }
    }
    //terminate program
    return 0;
}

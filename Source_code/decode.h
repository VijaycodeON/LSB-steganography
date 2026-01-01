#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    char *d_src_image_fname;
    FILE *d_fptr_src_image;

    char d_secrect_fname[50];
    FILE *d_fptr_secret;
    char d_extn_secret_file[5];
    char d_secret_data[50];
    long d_size_extn;
    int flag;
    //char d_magic_string[5];
    long d_size_secret_file;
}DecodeInfo;

/* check operation type */
OperationType d_check_operation_type( char *argv[] );

/* Read and validate decode argc and argv */
Status read_and_validate_decode_args( char *argv[], DecodeInfo *decInfo );

/* Perform the decoding */
Status do_decoding( DecodeInfo *decInfo );

/* Get file pointers for i/p anf o/p files */
Status d_open_files( DecodeInfo *decInfo );


/* store magic string */
Status decode_magic_string (  DecodeInfo *decInfo );

/* decode secret file extension */
Status decode_secret_file_extn( DecodeInfo *decInfo );

Status decode_secret_file_extn_size( DecodeInfo *decInfo );
/* decode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo );

/* decode secret file data */
Status decode_secret_file_data ( DecodeInfo *decInfo );

/* decode function, which does the real decoding */
Status decode_data_to_file( char* string, int size, DecodeInfo *decInfo);

/* decode a byte into LSB of image data array */
Status decode_byte_to_lsb ( char *image_buffer);

#endif

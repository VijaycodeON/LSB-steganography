# LSB Steganography in C

## Overview
This project implements Least Significant Bit (LSB) steganography
to hide secret data inside a BMP image using the C programming language.

All source files and input files are kept in the same directory
because the implementation is tightly coupled and each file
depends directly on the others during compilation and execution.

## Directory Structure
- `Source_code/` directory contains all C source files and image inputs
- `Design/` contains algorithm explanation and design references
- `Output_Images/` documents input assumptions and expected results

## Features
- Encode secret data file into an image using LSB technique
- Decode hidden data from a stego image or user given stego name
- No external libraries used
- Focused on bit-level understanding and file handling

## Build Instructions
Compile using GCC: 

```bash
gcc *.c -o lsb_steg

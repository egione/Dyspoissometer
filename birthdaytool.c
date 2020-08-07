/*
Dyspoissometer
Copyright 2016 Russell Leidich
http://dyspoissonism.blogspot.com

This collection of files constitutes the Dyspoissometer Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Dyspoissometer Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Dyspoissometer Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Dyspoissometer Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Birthday Tool
*/
#include "flag.h"
#include "flag_ascii.h"
#include "flag_dyspoissometer.h"
#include "flag_birthdaytool.h"
#include <math.h>
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include <quadmath.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include "debug_quad.h"
  #include "debug_quad_xtrn.h"
#endif
#include "ascii_xtrn.h"
#include "dyspoissometer.h"
#include "dyspoissometer_xtrn.h"

#ifdef WINDOWS
  #define FSEEKO fseeko64
  #define FTELLO ftello64
#else
  #define FSEEKO fseeko
  #define FTELLO ftello
#endif

void
birthdaytool_out_of_memory_print(void){
  DEBUG_PRINT("\nERROR: Out of memory!");
  #if(DYSPOISSOMETER_UINT_BIT_MAX==U16_BIT_MAX)
    DEBUG_PRINT(" \"make birthdaytool_double\" might help.");
  #elif(DYSPOISSOMETER_UINT_BIT_MAX==U32_BIT_MAX)
    DEBUG_PRINT(" \"make birthdaytool_quad\" might help.");
  #endif
  DEBUG_PRINT("\n");
  return;
}

int
main(int argc, char *argv[]){
  u64 birthday_bound;
  u64 file_size;
  char *filename_base;
  u8 granularity;
  FILE *handle;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_UINT mask_idx_max;
  DYSPOISSOMETER_UINT mask_max;
  DYSPOISSOMETER_UINT *mask_list_base;
  u8 mask_size;
  DYSPOISSOMETER_UINT mibr;
  DYSPOISSOMETER_UINT mibr_median;
  u64 parameter;
  DYSPOISSOMETER_NUMBER skew;
  u8 status;
  u64 transfer_size;
  ULONG u8_idx;
  u8 *u8_list_base;

  status=ascii_init(0, 0);
  status=(u8)(status|dyspoissometer_init(0, 0));
  handle=NULL;
  mask_list_base=NULL;
  u8_list_base=NULL;
  do{
    if(status){
      DEBUG_PRINT("\nERROR: Outdated source code!\n");
      break;
    }
    status=1;
    if(argc!=3){
      DEBUG_PRINT("Birthday Tool\nCopyright 2016 Russell Leidich\nhttp://dyspoissonism.blogspot.com\n");
      DEBUG_U32("build_id_in_hex", BIRTHDAYTOOL_BUILD_ID);
      DEBUG_PRINT("Birthday bound analyzer.\n\n");
      DEBUG_PRINT("Syntax:\n\n");
      DEBUG_PRINT("birthdaytool filename granularity\n\n");
      DEBUG_PRINT("where:\n\n");
      DEBUG_PRINT("filename name of the file to analyze.\n\n");
      DEBUG_PRINT("granularity is the number of bytes per mask, less one, on the interval [0, 7].\n\n");
      DEBUG_PRINT("Note that the output is displayed in a combination of floating-point and\nhexadecimal.\n\n");
      break;
    }
    status=ascii_decimal_to_u64_convert(argv[2], &parameter, U64_BYTE_MAX);
    if(status){
      DEBUG_PRINT("ERROR: Invalid granularity!\n");
      break;
    }
    granularity=(u8)(parameter);
    filename_base=argv[1];
    handle=fopen(filename_base, "rb");
    if(!handle){
      DEBUG_PRINT("ERROR: Cannot open that file for reading!\n");
      break;
    }
    status=!!FSEEKO(handle, (off_t)(0), SEEK_END);
    file_size=0;
    if(!status){
      file_size=(u64)(FTELLO(handle));
      status=1;
      if(file_size!=U64_MAX){
        status=!!FSEEKO(handle, (off_t)(0), SEEK_SET);
      }
    }
    if(status||(!file_size)){
      status=1;
      DEBUG_PRINT("ERROR: Cannot determine file size!\n");
      break;
    }
    status=1;
    mask_size=(u8)(granularity+1);
    if(file_size%mask_size){
      DEBUG_PRINT("ERROR: File size is not a multiple of (granularity+1) bytes!\n");
      break;
    }
    mask_idx_max=(DYSPOISSOMETER_UINT)((file_size/mask_size)-1);
    mask_list_base=dyspoissometer_uint_list_malloc(mask_idx_max);
    u8_list_base=malloc((size_t)(file_size));
    if(!(mask_list_base&&u8_list_base)){
      birthdaytool_out_of_memory_print();
      break;
    }
    transfer_size=(u64)(fread(u8_list_base, (size_t)(U8_SIZE), (size_t)(file_size), handle));
    if(file_size!=transfer_size){
      DEBUG_PRINT("ERROR: File read failed!\n");
      break;
    }
    mask_idx=0;
    u8_idx=0;
    do{
      mask_list_base[mask_idx]=0;
      memcpy(&mask_list_base[mask_idx], &u8_list_base[u8_idx], (size_t)(mask_size));
      u8_idx=(ULONG)(u8_idx+mask_size);
    }while((mask_idx++)!=mask_idx_max);
    status=dyspoissometer_mibr_get(mask_idx_max, mask_list_base, &mibr);
    if(status){
      birthdaytool_out_of_memory_print();
      break;
    }
    status=1;
    mask_max=(DYSPOISSOMETER_UINT)(((1ULL<<(granularity<<U8_BITS_LOG2))<<U8_BITS)-1);
    skew=dyspoissometer_skew_from_mibr_get(mask_max, mibr);
    mibr_median=dyspoissometer_mibr_from_skew_get(mask_max, 0.5f);
    birthday_bound=(u64)(mibr_median)+1;
    DEBUG_U64("birthday_bound", birthday_bound);
    DEBUG_U64("mask_idx_max", mask_idx_max);
    DEBUG_U64("mibr", mibr);
    DEBUG_U64("mibr_median", mibr_median);
    DEBUG_NUMBER("mibr_skew", skew);
    if((mask_idx_max==mibr)&&(mask_idx_max<=mibr_median)){
      DEBUG_PRINT("\nThis MIBR test is inconclusive because mask repetitions were neither found nor\nexpected to have been found.\n\n");
    }
    status=0;
  }while(0);
  if(handle){
    fclose(handle);
  }
  free(u8_list_base);
  dyspoissometer_free(mask_list_base);
  return status;
}

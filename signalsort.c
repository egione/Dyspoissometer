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
Signal Sort
*/
#include "flag.h"
#include "flag_ascii.h"
#include "flag_dyspoissometer.h"
#include "flag_signalsort.h"
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
signalsort_out_of_memory_print(void){
  DEBUG_PRINT("\nERROR: Out of memory!");
  #if(DYSPOISSOMETER_UINT_BIT_MAX==U16_BIT_MAX)
    DEBUG_PRINT(" \"make signalsort_double\" might help.");
  #elif(DYSPOISSOMETER_UINT_BIT_MAX==U32_BIT_MAX)
    DEBUG_PRINT(" \"make signalsort_quad\" might help.");
  #endif
  DEBUG_PRINT("\n");
  return;
}

int
main(int argc, char *argv[]){
  u8 bit_idx;
  u8 byte;
  DYSPOISSOMETER_UINT byte_pop;
  DYSPOISSOMETER_UINT *byte_pop_list_base;
  DYSPOISSOMETER_NUMBER dyspoissonism;
  u8 *footer_base;
  ULONG footer_size;
  u64 dyspoissonism_fraction;
  u64 *dyspoissonism_fraction_list0_base;
  u64 *dyspoissonism_fraction_list1_base;
  u64 file_size;
  u64 file_size_min;
  char *filename_base;
  DYSPOISSOMETER_UINT frame_count;
  u64 frame_count_u64;
  DYSPOISSOMETER_UINT frame_granularity;
  u64 frame_granularity_u64;
  DYSPOISSOMETER_UINT frame_idx;
  DYSPOISSOMETER_UINT frame_idx_new;
  DYSPOISSOMETER_UINT frame_idx_old;
  DYSPOISSOMETER_UINT *frame_idx_list0_base;
  DYSPOISSOMETER_UINT *frame_idx_list1_base;
  ULONG frame_size;
  u64 frame_size_u64;
  FILE *handle;
  u8 *header_base;
  ULONG header_size;
  u64 header_size_u64;
  u8 header_status;
  u64 header_status_u64;
  ULONG list_size;
  DYSPOISSOMETER_NUMBER logfreedom;
  u8 mask_granularity;
  u64 mask_granularity_u64;
  DYSPOISSOMETER_UINT mask_idx_max;
  DYSPOISSOMETER_UINT mask_idx_min;
  u32 mask_max;
  DYSPOISSOMETER_UINT mask_max_uint;
  u8 output_status;
  u8 status;
  ULONG transfer_size;
  ULONG transfer_size_old;
  u64 transfer_size_u64;
  u8 *u8_list0_base;
  u8 *u8_list1_base;
  u16 *u16_list0_base;
  u16 *u16_list1_base;
  u32 *u32_list0_base;
  u32 *u32_list1_base;
  ULONG uint_idx_new;
  ULONG uint_idx_old;
  u8 zero;

  status=ascii_init(0, 0);
  status=(u8)(status|dyspoissometer_init(0, 0));
  handle=NULL;
  u16_list0_base=NULL;
  u16_list1_base=NULL;
  u32_list0_base=NULL;
  u32_list1_base=NULL;
  u8_list0_base=NULL;
  u8_list1_base=NULL;
  header_base=NULL;
  footer_base=NULL;
  dyspoissonism_fraction_list0_base=NULL;
  dyspoissonism_fraction_list1_base=NULL;
  frame_idx_list0_base=NULL;
  frame_idx_list1_base=NULL;
  byte_pop_list_base=NULL;
  zero=0;
  do{
    if(status){
      DEBUG_PRINT("\nERROR: Outdated source code!\n");
      break;
    }
    status=1;
    if((argc!=6)&&(argc!=7)){
      DEBUG_PRINT("Signal Sort\nCopyright 2016 Russell Leidich\nhttp://dyspoissonism.blogspot.com\n");
      DEBUG_U32("build_id_in_hex", SIGNALSORT_BUILD_ID);
      DEBUG_PRINT("Split a file into frames, then sort them by dyspoissonism.\n\n");
      DEBUG_PRINT("Syntax:\n\n");
      DEBUG_PRINT("signalsort input_filename header_size header_status mask_granularity\n           frame_granularity [output_filename]\n\n");
      DEBUG_PRINT("where:\n\n");
      DEBUG_PRINT("input_filename name of the file to analyze.\n\n");
      DEBUG_PRINT("header_size is the decimal number of bytes in input_filename's header. They\nwill be ignored.\n\n");
      DEBUG_PRINT("header_status tells whether or not you care about the file header and footer.\nThe size of the footer is implied by the number of bytes remaining after the\nheader and all frames have been read. If header_status is one and\noutput_filename is not specified, then the header will be displayed as\nhexadecimal bytes and the footer will be ignored. Otherwise, both the header\nand footer will be copied to output_filename. This is useful, for example, to\nallow uncompressed images (e.g. TARGA files) to be rearranged as pixel masks\nsorted by dyspoissonism without disrupting the ability of image utilities to\ndisplay them.\n\n.");
      DEBUG_PRINT("mask_granularity is the number of bytes per mask, less one: 0 for 8-bit\nmasks, 1 for 16-bit, 2 for 24-bit, or 3 for 32-bit. 32-bit masks require\nO(10^11) bytes of\nmemory!\n\n");
      DEBUG_PRINT("frame_granularity is the decimal number of masks per frame, less one, such that\nthe dyspoissonism of each frame will be calculated.\n\n");
      DEBUG_PRINT("output_filename, if provided, is name of the file to which to write all frames\nsorted ascending by dyspoissonism. If not provided, then zero-based frame\nnumbers will be displayed in hexadecimal to the left of their dyspoissonisms,\nwhich will be displayed as rounded-nearest hexadecimal fractions, where (2^63)\nis one.\n\n");
      break;
    }
    output_status=(u8)(7-argc);
    status=ascii_decimal_to_u64_convert(argv[2], &header_size_u64, ULONG_MAX);
    if(status){
      DEBUG_PRINT("ERROR: Invalid header_size!\n");
      break;
    }
    header_size=(ULONG)(header_size_u64);
    status=ascii_decimal_to_u64_convert(argv[3], &header_status_u64, 1);
    if(status){
      DEBUG_PRINT("ERROR: Invalid header_status!\n");
      break;
    }
    header_status=(u8)(header_status_u64);
    status=ascii_decimal_to_u64_convert(argv[4], &mask_granularity_u64, U32_BYTE_MAX);
    if(status){
      DEBUG_PRINT("ERROR: Invalid mask_granularity!\n");
      break;
    }
    mask_granularity=(u8)(mask_granularity_u64);
    status=ascii_decimal_to_u64_convert(argv[5], &frame_granularity_u64, DYSPOISSOMETER_UINT_MAX);
    if(status){
      DEBUG_PRINT("ERROR: Invalid frame_granularity!\n");
      break;
    }
    frame_granularity=(DYSPOISSOMETER_UINT)(frame_granularity_u64);
    status=1;
    frame_size_u64=frame_granularity_u64+1;
    if(!frame_size_u64||((ULONG_MAX/(u8)(mask_granularity+1))<frame_size_u64)){
      DEBUG_PRINT("ERROR: Invalid frame_granularity!\n");
      break;
    }
    frame_size_u64*=(u8)(mask_granularity+1);
    frame_size=(ULONG)(frame_size_u64);
    filename_base=argv[1];
    handle=fopen(filename_base, "rb");
    if(!handle){
      DEBUG_PRINT("ERROR: Cannot open input file for reading!\n");
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
    file_size_min=header_size+frame_size_u64;
    if((file_size<file_size_min)||(file_size_min<frame_size_u64)){
      DEBUG_PRINT("ERROR: File is too small to contain header and one frame!\n");
      break;
    }
    frame_count_u64=(file_size-header_size)/frame_size;
    transfer_size_u64=frame_count_u64*frame_size;
    if((DYSPOISSOMETER_UINT_MAX<frame_count_u64)||(ULONG_MAX<transfer_size_u64)){
      DEBUG_PRINT("ERROR: File too big. Try \"make signalsort_quad\".\n");
      break;
    }
    frame_count=(DYSPOISSOMETER_UINT)(frame_count_u64);
    transfer_size=(ULONG)(transfer_size_u64);
    footer_size=(ULONG)(file_size-header_size-transfer_size);
    if(header_status){
      if(header_size){
        header_base=(u8 *)(malloc((size_t)(header_size)));
        if(!header_base){
          signalsort_out_of_memory_print();
          break;
        }
        transfer_size_old=(ULONG)(fread(header_base, (size_t)(U8_SIZE), (size_t)(header_size), handle));
        if(transfer_size_old!=header_size){
          DEBUG_PRINT("ERROR: Header read failed!\n");
          break;
        }
      }
      if(footer_size&&!output_status){
        footer_base=(u8 *)(malloc((size_t)(footer_size)));
        if(!footer_base){
          signalsort_out_of_memory_print();
          break;
        }
        transfer_size_old=(ULONG)(file_size-footer_size);
        status=!!FSEEKO(handle, (off_t)(transfer_size_old), SEEK_SET);
        transfer_size_old=(ULONG)(fread(footer_base, (size_t)(U8_SIZE), (size_t)(footer_size), handle));
        status=(u8)(status|!!FSEEKO(handle, (off_t)(header_size), SEEK_SET));
        if(status||(transfer_size_old!=footer_size)){
          status=1;
          DEBUG_PRINT("ERROR: Footer read failed!\n");
          break;
        }
        status=1;
      }
    }
    status=1;
    switch(mask_granularity){
    case 0:
    case 2:
      u8_list0_base=(u8 *)(malloc((size_t)(transfer_size)));
      u8_list1_base=(u8 *)(malloc((size_t)(transfer_size)));
      if(u8_list0_base&&u8_list1_base){
        status=0;
      }
      break;
    case 1:
      u16_list0_base=(u16 *)(malloc((size_t)(transfer_size)));
      u16_list1_base=(u16 *)(malloc((size_t)(transfer_size)));
      if(u16_list0_base&&u16_list1_base){
        status=0;
      }
      break;
    case 3:
      u32_list0_base=(u32 *)(malloc((size_t)(transfer_size)));
      u32_list1_base=(u32 *)(malloc((size_t)(transfer_size)));
      if(u32_list0_base&&u32_list1_base){
        status=0;
      }
      break;
    }
    if(status){
      signalsort_out_of_memory_print();
      break;
    }
    status=1;
    transfer_size_old=transfer_size;
    switch(mask_granularity){
    case 0:
    case 2:
      transfer_size=(ULONG)(fread(u8_list0_base, (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
      break;
    case 1:
      transfer_size=(ULONG)(fread((u8 *)(u16_list0_base), (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
      break;
    case 3:
      transfer_size=(ULONG)(fread((u8 *)(u32_list0_base), (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
      break;
    }
    fclose(handle);
    handle=NULL;
    if(transfer_size!=transfer_size_old){
      DEBUG_PRINT("ERROR: Input file read failed!\n");
      break;
    }
    if(((ULONG_MAX>>U64_SIZE_LOG2)<frame_count_u64)||((ULONG_MAX>>DYSPOISSOMETER_UINT_SIZE_LOG2)<frame_count_u64)){
      signalsort_out_of_memory_print();
      break;
    }
    list_size=(ULONG)(frame_count_u64<<U64_SIZE_LOG2);
    dyspoissonism_fraction_list0_base=(u64 *)(malloc((size_t)(list_size)));
    dyspoissonism_fraction_list1_base=(u64 *)(malloc((size_t)(list_size)));
    list_size=(ULONG)(frame_count_u64<<DYSPOISSOMETER_UINT_SIZE_LOG2);
    frame_idx_list0_base=(DYSPOISSOMETER_UINT *)(malloc((size_t)(list_size)));
    frame_idx_list1_base=(DYSPOISSOMETER_UINT *)(malloc((size_t)(list_size)));
    byte_pop_list_base=(DYSPOISSOMETER_UINT *)(malloc((size_t)(U8_SPAN<<DYSPOISSOMETER_UINT_SIZE_LOG2)));
    if(!(dyspoissonism_fraction_list0_base&&dyspoissonism_fraction_list1_base&&frame_idx_list0_base&&frame_idx_list1_base&&byte_pop_list_base)){
      signalsort_out_of_memory_print();
      break;
    }
    status=0;
    logfreedom=0.0f;
    mask_idx_min=0;
    frame_idx=0;
    mask_max=((u32)(1U<<(mask_granularity<<U8_BITS_LOG2))<<U8_BITS)-1;
    mask_max_uint=(DYSPOISSOMETER_UINT)(mask_max);
    do{
      mask_idx_max=(DYSPOISSOMETER_UINT)(mask_idx_min+frame_granularity);
      switch(mask_granularity){
      case 0:
        logfreedom=dyspoissometer_u8_list_logfreedom_get(mask_idx_max, mask_idx_min, (u8)(mask_max), u8_list0_base);
        break;
      case 1:
        logfreedom=dyspoissometer_u16_list_logfreedom_get(mask_idx_max, mask_idx_min, (u16)(mask_max), u16_list0_base);
        break;
      case 2:
        logfreedom=dyspoissometer_u24_list_logfreedom_get(mask_idx_max, mask_idx_min, mask_max, u8_list0_base);
        break;
      case 3:
        logfreedom=dyspoissometer_u32_list_logfreedom_get(mask_idx_max, mask_idx_min, mask_max, u32_list0_base);
        break;
      }
      if(logfreedom<0.0f){
        signalsort_out_of_memory_print();
        status=1;
        break;
      }
      dyspoissonism=dyspoissometer_dyspoissonism_get(logfreedom, frame_granularity, mask_max_uint);
      dyspoissonism_fraction=(u64)((dyspoissonism*U64_SPAN_HALF)+0.5f);
      dyspoissonism_fraction_list0_base[frame_idx]=dyspoissonism_fraction;
      frame_idx_list0_base[frame_idx]=frame_idx;
      mask_idx_min=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
      frame_idx++;
    }while(frame_idx<frame_count);
    if(status){
      break;
    }
    status=1;
    bit_idx=0;
    do{
      memset(byte_pop_list_base, 0, (size_t)(U8_SPAN<<DYSPOISSOMETER_UINT_SIZE_LOG2));
      frame_idx=0;
      do{
        dyspoissonism_fraction=dyspoissonism_fraction_list0_base[frame_idx];
        byte=(dyspoissonism_fraction>>bit_idx)&U8_MAX;
        byte_pop_list_base[byte]++;
        frame_idx++;
      }while(frame_idx<frame_count);
      byte=0;
      byte_pop=DYSPOISSOMETER_UINT_MAX;
      do{
        byte_pop=(DYSPOISSOMETER_UINT)(byte_pop+byte_pop_list_base[byte]);
        byte_pop_list_base[byte]=byte_pop;
        byte++;
      }while(byte);
      frame_idx=(DYSPOISSOMETER_UINT)(frame_count-1);
      do{
        dyspoissonism_fraction=dyspoissonism_fraction_list0_base[frame_idx];
        byte=(dyspoissonism_fraction>>bit_idx)&U8_MAX;
        frame_idx_new=byte_pop_list_base[byte];
        byte_pop_list_base[byte]=(DYSPOISSOMETER_UINT)(frame_idx_new-1);
        dyspoissonism_fraction_list1_base[frame_idx_new]=dyspoissonism_fraction;
        frame_idx_old=frame_idx_list0_base[frame_idx];
        frame_idx_list1_base[frame_idx_new]=frame_idx_old;
        frame_idx--;
      }while(frame_idx<frame_count);
      list_size=(ULONG)(frame_count_u64<<U64_SIZE_LOG2);
      memcpy(dyspoissonism_fraction_list0_base, dyspoissonism_fraction_list1_base, (size_t)(list_size));
      list_size=(ULONG)(frame_count_u64<<DYSPOISSOMETER_UINT_SIZE_LOG2);
      memcpy(frame_idx_list0_base, frame_idx_list1_base, (size_t)(list_size));
      bit_idx=(u8)(bit_idx+U8_BITS);
    }while(bit_idx<=U64_BIT_MAX);
    if(!output_status){
      filename_base=argv[6];
      handle=fopen(filename_base, "wb");
      if(!handle){
        DEBUG_PRINT("ERROR: Cannot open output file for writing!\n");
        break;
      }
    }
    if(header_status){
      if(!output_status){
        if(header_size){
          transfer_size=(ULONG)(fwrite(header_base, (size_t)(U8_SIZE), (size_t)(header_size), handle));
          if(transfer_size!=header_size){
            DEBUG_PRINT("ERROR: Header write failed!\n");
            break;
          }
        }
      }else{
        if(header_size){
          DEBUG_LIST("header", (ULONG)(header_size), header_base, 0);
        }else{
          DEBUG_LIST("header", (ULONG)(header_size), &zero, 0);
        }
      }
    }
    frame_idx=(DYSPOISSOMETER_UINT)(frame_count-1);
    if(!output_status){
      uint_idx_old=0;
      do{
        frame_idx_new=frame_idx_list0_base[frame_idx];
        uint_idx_new=(ULONG)((ULONG)(frame_idx_new)*frame_size);
        memcpy(&u8_list1_base[uint_idx_new], &u8_list0_base[uint_idx_old], (size_t)(frame_size));
        uint_idx_old=(ULONG)(uint_idx_old+frame_size);
        frame_idx--;
      }while(frame_idx<frame_count);
      switch(mask_granularity){
      case 0:
      case 2:
        transfer_size=(ULONG)(fwrite(u8_list1_base, (size_t)(U8_SIZE), (size_t)(transfer_size_old), handle));
        break;
      case 1:
        transfer_size=(ULONG)(fwrite((u8 *)(u16_list1_base), (size_t)(U8_SIZE), (size_t)(transfer_size_old), handle));
        break;
      case 3:
        transfer_size=(ULONG)(fwrite((u8 *)(u32_list1_base), (size_t)(U8_SIZE), (size_t)(transfer_size_old), handle));
        break;
      }
      if(transfer_size!=transfer_size_old){
        DEBUG_PRINT("ERROR: Output file write failed!\n");
        break;
      }
      if(footer_size){
        transfer_size_old=(ULONG)(fwrite(footer_base, (size_t)(U8_SIZE), (size_t)(footer_size), handle));
        if(transfer_size_old!=footer_size){
          DEBUG_PRINT("ERROR: Footer write failed!\n");
          break;
        }
      }
      fclose(handle);
      handle=NULL;
    }else{
      do{
        dyspoissonism_fraction=dyspoissonism_fraction_list0_base[frame_idx];
        frame_idx_old=frame_idx_list0_base[frame_idx];
        printf("%08X%08X: %08X%08X\n", (u32)((u64)(frame_idx_old)>>U32_BITS), (u32)(frame_idx_old), (u32)(dyspoissonism_fraction>>U32_BITS), (u32)(dyspoissonism_fraction));
        frame_idx--;
      }while(frame_idx<frame_count);
    }
    status=0;
  }while(0);
  if(handle){
    fclose(handle);
  }
  free(byte_pop_list_base);
  free(frame_idx_list1_base);
  free(frame_idx_list0_base);
  free(dyspoissonism_fraction_list1_base);
  free(dyspoissonism_fraction_list0_base);
  free(footer_base);
  free(header_base);
  free(u8_list1_base);
  free(u8_list0_base);
  free(u32_list1_base);
  free(u32_list0_base);
  free(u16_list1_base);
  free(u16_list0_base);
  return status;
}

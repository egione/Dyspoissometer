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
Deltafy
*/
#include "flag.h"
#include "flag_ascii.h"
#include "flag_deltafy.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#include "ascii_xtrn.h"

#ifdef WINDOWS
  #define FSEEKO fseeko64
  #define FTELLO ftello64
#else
  #define FSEEKO fseeko
  #define FTELLO ftello
#endif

void
deltafy_out_of_memory_print(void){
  DEBUG_PRINT("\nERROR: Out of memory!\n");
  return;
}

int
main(int argc, char *argv[]){
  u8 byte_lane_status;
  u64 byte_lane_status_u64;
  u8 *footer_base;
  ULONG footer_size;
  u64 file_size;
  u64 file_size_min;
  char *filename_base;
  ULONG frame_count;
  u64 frame_count_u64;
  ULONG frame_granularity;
  u64 frame_granularity_u64;
  ULONG frame_idx;
  ULONG frame_size;
  u64 frame_size_u64;
  FILE *handle;
  u8 *header_base;
  ULONG header_size;
  u64 header_size_u64;
  u8 header_status;
  u64 header_status_u64;
  u32 mask;
  u8 mask_granularity;
  u64 mask_granularity_u64;
  ULONG mask_idx;
  ULONG mask_idx_max;
  ULONG mask_idx_min;
  u32 mask_new;
  u32 mask_old;
  u8 mode;
  u64 mode_u64;
  u8 status;
  ULONG transfer_size;
  ULONG transfer_size_old;
  u64 transfer_size_u64;
  u8 *u8_list_base;
  u16 *u16_list_base;
  u32 *u32_list_base;
  ULONG uint_idx;

  status=ascii_init(0, 0);
  footer_base=NULL;
  handle=NULL;
  header_base=NULL;
  u16_list_base=NULL;
  u32_list_base=NULL;
  u8_list_base=NULL;
  do{
    if(status){
      DEBUG_PRINT("\nERROR: Outdated source code!\n");
      break;
    }
    status=1;
    if(argc!=9){
      DEBUG_PRINT("Deltafy\nCopyright 2016 Russell Leidich\nhttp://dyspoissonism.blogspot.com\n");
      DEBUG_U32("build_id_in_hex", DELTAFY_BUILD_ID);
      DEBUG_PRINT("Split a file into frames, then perform discrete differentiation or integration.\n\n");
      DEBUG_PRINT("Syntax:\n\n");
      DEBUG_PRINT("deltafy input_filename header_size header_status mask_granularity\n        frame_granularity output_filename mode byte_lane_status\n\n");
      DEBUG_PRINT("where:\n\n");
      DEBUG_PRINT("input_filename name of the file to analyze.\n\n");
      DEBUG_PRINT("header_size is the decimal number of bytes in input_filename's header, which\nwill not be modified.\n\n");
      DEBUG_PRINT("header_status tells whether or not to preserve file header and footer. The size\nof the footer is implied by the number of bytes remaining after the header and\nall frames have been read. If header_status is one, then both the header and\nfooter will be copied to output_filename. Otherwise they will be discarded.\nThis is useful, for example, to allow uncompressed images (e.g. TARGA files) to\nbe rearranged as pixel masks sorted by dyspoissonism without disrupting the\nability of image utilities to display them.\n\n");
      DEBUG_PRINT("mask_granularity is the number of bytes per mask, less one: 0 for 8-bit\nmasks, 1 for 16-bit, 2 for 24-bit, or 3 for 32-bit.\n\n");
      DEBUG_PRINT("frame_granularity is the decimal number of masks per frame, less one, such that\nthe derivative or integral of each frame will be calculated.\n\n");
      DEBUG_PRINT("output_filename is name of the file to which to write all processed frames.\n\n");
      DEBUG_PRINT("mode is: 0 to differentiate once, 1 to integrate once, 2 to differentiate\ntwice, or 3 to integrate twice. 1 inverts 0 and 3 inverts 2; no other\nsequential operations result in correct inversion.\n\n");
      DEBUG_PRINT("byte_lane_status is zero to treat each mask as a unified integer; else one to\ntreat each byte as its own integer, in which case, neighbors are separated by a\ndistance of (mask_granularity+1) bytes. It has no effect when mask_granularity\nis zero.\n\n");
      break;
    }
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
    status=ascii_decimal_to_u64_convert(argv[5], &frame_granularity_u64, ULONG_MAX);
    if(status){
      DEBUG_PRINT("ERROR: Invalid frame_granularity!\n");
      break;
    }
    frame_granularity=(ULONG)(frame_granularity_u64);
    status=1;
    frame_size_u64=frame_granularity_u64+1;
    if(!frame_size_u64||((ULONG_MAX/(u8)(mask_granularity+1))<frame_size_u64)){
      DEBUG_PRINT("ERROR: Invalid frame_granularity!\n");
      break;
    }
    status=ascii_decimal_to_u64_convert(argv[7], &mode_u64, 3);
    if(status){
      DEBUG_PRINT("ERROR: Invalid mode!\n");
      break;
    }
    mode=(u8)(mode_u64);
    status=ascii_decimal_to_u64_convert(argv[8], &byte_lane_status_u64, 1);
    if(status){
      DEBUG_PRINT("ERROR: Invalid byte_lane_status!\n");
      break;
    }
    status=1;
    byte_lane_status=(u8)(byte_lane_status_u64);
    if(frame_granularity<=((mode>>1)&1)){
      DEBUG_PRINT("ERROR: frame_granularity too small to support mode!\n");
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
    if(ULONG_MAX<=transfer_size_u64){
      DEBUG_PRINT("ERROR: File too big.\n");
      break;
    }
    frame_count=(ULONG)(frame_count_u64);
    transfer_size=(ULONG)(transfer_size_u64);
    footer_size=(ULONG)(file_size-header_size-transfer_size);
    if(header_status){
      if(header_size){
        header_base=(u8 *)(malloc((size_t)(header_size)));
        if(!header_base){
          deltafy_out_of_memory_print();
          break;
        }
        transfer_size_old=(ULONG)(fread(header_base, (size_t)(U8_SIZE), (size_t)(header_size), handle));
        if(transfer_size_old!=header_size){
          DEBUG_PRINT("ERROR: Header read failed!\n");
          break;
        }
      }
      if(footer_size){
        footer_base=(u8 *)(malloc((size_t)(footer_size)));
        if(!footer_base){
          deltafy_out_of_memory_print();
          break;
        }
        transfer_size_old=(ULONG)(file_size-footer_size);
        status=!!FSEEKO(handle, (off_t)(transfer_size_old), SEEK_SET);
        if(status){
          DEBUG_PRINT("ERROR: Footer read failed!\n");
          break;
        }
        transfer_size_old=(ULONG)(fread(footer_base, (size_t)(U8_SIZE), (size_t)(footer_size), handle));
        if(transfer_size_old!=footer_size){
          status=1;
          DEBUG_PRINT("ERROR: Footer read failed!\n");
          break;
        }
      }
    }
    status=!!FSEEKO(handle, (off_t)(header_size), SEEK_SET);
    if(status){
      DEBUG_PRINT("ERROR: Seek past header failed!\n");
      break;
    }
    status=1;
    switch(mask_granularity){
    case 0:
    case 2:
      u8_list_base=(u8 *)(malloc((size_t)(transfer_size)));
      if(u8_list_base){
        status=0;
      }
      break;
    case 1:
      u16_list_base=(u16 *)(malloc((size_t)(transfer_size)));
      if(u16_list_base){
        status=0;
      }
      break;
    case 3:
      u32_list_base=(u32 *)(malloc((size_t)(transfer_size)));
      if(u32_list_base){
        status=0;
      }
      break;
    }
    if(status){
      deltafy_out_of_memory_print();
      break;
    }
    status=1;
    transfer_size_old=transfer_size;
    switch(mask_granularity){
    case 0:
    case 2:
      transfer_size=(ULONG)(fread(u8_list_base, (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
      break;
    case 1:
      transfer_size=(ULONG)(fread((u8 *)(u16_list_base), (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
      break;
    case 3:
      transfer_size=(ULONG)(fread((u8 *)(u32_list_base), (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
      break;
    }
    fclose(handle);
    handle=NULL;
    if(transfer_size!=transfer_size_old){
      DEBUG_PRINT("ERROR: Input file read failed!\n");
      break;
    }
    mask_idx_max=ULONG_MAX;
    frame_idx=0;
    mask_new=0;
    do{
      mask_idx_min=(ULONG)(mask_idx_max+1);
      mask_idx_max=(ULONG)(mask_idx_min+frame_granularity);
      if(mode&1){
        mask_idx_min=(ULONG)(mask_idx_min+(mode>>1));
      }
      mask_idx=mask_idx_min;
      mask_old=0;
      switch(mask_granularity){
      case 0:
        switch(mode){
        case 0:
        case 2:
          do{
            mask=u8_list_base[mask_idx];
            u8_list_base[mask_idx]=(u8)(mask-mask_old);
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode){
            mask_idx=(ULONG)(mask_idx_min+1);
            mask_old=0;
            do{
              mask=u8_list_base[mask_idx];
              u8_list_base[mask_idx]=(u8)(mask-mask_old);
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        case 1:
        case 3:
          mask_old=u8_list_base[mask_idx];
          mask_idx++;
          do{
            mask=u8_list_base[mask_idx]+mask_old;
            u8_list_base[mask_idx]=(u8)(mask);
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode==3){
            mask_idx=mask_idx_min;
            mask_idx_min--;
            mask_old=u8_list_base[mask_idx_min];
            do{
              mask=u8_list_base[mask_idx]+mask_old;
              u8_list_base[mask_idx]=(u8)(mask);
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        }
        break;
      case 1:
        switch(mode){
        case 0:
        case 2:
          do{
            mask=u16_list_base[mask_idx];
            if(!byte_lane_status){
              mask_new=mask-mask_old;
            }else{
              mask_new=((u32)((u8)((mask>>U8_BITS)-(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask-mask_old);
            }
            u16_list_base[mask_idx]=(u16)(mask_new);
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode){
            mask_idx=(ULONG)(mask_idx_min+1);
            mask_old=0;
            do{
              mask=u16_list_base[mask_idx];
              if(!byte_lane_status){
                mask_new=mask-mask_old;
              }else{
                mask_new=((u32)((u8)((mask>>U8_BITS)-(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask-mask_old);
              }
              u16_list_base[mask_idx]=(u16)(mask_new);
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        case 1:
        case 3:
          mask_old=u16_list_base[mask_idx];
          mask_idx++;
          do{
            mask=u16_list_base[mask_idx];
            if(!byte_lane_status){
              mask+=mask_old;
            }else{
              mask=((u32)((u8)((mask>>U8_BITS)+(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask+mask_old);
            }
            u16_list_base[mask_idx]=(u16)(mask);
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode==3){
            mask_idx=mask_idx_min;
            mask_idx_min--;
            mask_old=u16_list_base[mask_idx_min];
            do{
              mask=u16_list_base[mask_idx];
              if(!byte_lane_status){
                mask+=mask_old;
              }else{
                mask=((u32)((u8)((mask>>U8_BITS)+(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask+mask_old);
              }
              u16_list_base[mask_idx]=(u16)(mask);
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        }
        break;
      case 2:
        switch(mode){
        case 0:
        case 2:
          uint_idx=(ULONG)(mask_idx_min*3);
          do{
            mask=u8_list_base[uint_idx]+((u32)(u8_list_base[uint_idx+1])<<U8_BITS)+((u32)(u8_list_base[uint_idx+2])<<U16_BITS);
            if(!byte_lane_status){
              mask_new=mask-mask_old;
            }else{
              mask_new=((u32)((u8)((mask>>U16_BITS)-(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)-(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask-mask_old);
            }
            u8_list_base[uint_idx]=(u8)(mask_new);
            u8_list_base[uint_idx+1]=(u8)(mask_new>>U8_BITS);
            u8_list_base[uint_idx+2]=(u8)(mask_new>>U16_BITS);
            uint_idx=(ULONG)(uint_idx+3);
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode){
            mask_idx=(ULONG)(mask_idx_min+1);
            uint_idx=(ULONG)(mask_idx*3);
            mask_old=0;
            do{
              mask=u8_list_base[uint_idx]+((u32)(u8_list_base[uint_idx+1])<<U8_BITS)+((u32)(u8_list_base[uint_idx+2])<<U16_BITS);
              if(!byte_lane_status){
                mask_new=mask-mask_old;
              }else{
                mask_new=((u32)((u8)((mask>>U16_BITS)-(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)-(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask-mask_old);
              }
              u8_list_base[uint_idx]=(u8)(mask_new);
              u8_list_base[uint_idx+1]=(u8)(mask_new>>U8_BITS);
              u8_list_base[uint_idx+2]=(u8)(mask_new>>U16_BITS);
              uint_idx=(ULONG)(uint_idx+3);
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        case 1:
        case 3:
          uint_idx=(ULONG)(mask_idx*3);
          mask_old=u8_list_base[uint_idx]+((u32)(u8_list_base[uint_idx+1])<<U8_BITS)+((u32)(u8_list_base[uint_idx+2])<<U16_BITS);
          mask_idx++;
          uint_idx=(ULONG)(uint_idx+3);
          do{
            mask=u8_list_base[uint_idx]+((u32)(u8_list_base[uint_idx+1])<<U8_BITS)+((u32)(u8_list_base[uint_idx+2])<<U16_BITS);
            if(!byte_lane_status){
              mask+=mask_old;
            }else{
              mask=((u32)((u8)((mask>>U16_BITS)+(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)+(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask+mask_old);
            }
            u8_list_base[uint_idx]=(u8)(mask);
            u8_list_base[uint_idx+1]=(u8)(mask>>U8_BITS);
            u8_list_base[uint_idx+2]=(u8)(mask>>U16_BITS);
            uint_idx=(ULONG)(uint_idx+3);
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode==3){
            mask_idx=mask_idx_min;
            mask_idx_min--;
            uint_idx=(ULONG)(mask_idx_min*3);
            mask_old=u8_list_base[uint_idx]+((u32)(u8_list_base[uint_idx+1])<<U8_BITS)+((u32)(u8_list_base[uint_idx+2])<<U16_BITS);
            uint_idx=(ULONG)(uint_idx+3);
            do{
              mask=u8_list_base[uint_idx]+((u32)(u8_list_base[uint_idx+1])<<U8_BITS)+((u32)(u8_list_base[uint_idx+2])<<U16_BITS);
              if(!byte_lane_status){
                mask+=mask_old;
              }else{
                mask=((u32)((u8)((mask>>U16_BITS)+(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)+(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask+mask_old);
              }
              u8_list_base[uint_idx]=(u8)(mask);
              u8_list_base[uint_idx+1]=(u8)(mask>>U8_BITS);
              u8_list_base[uint_idx+2]=(u8)(mask>>U16_BITS);
              uint_idx=(ULONG)(uint_idx+3);
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        }
        break;
      case 3:
        switch(mode){
        case 0:
        case 2:
          do{
            mask=u32_list_base[mask_idx];
            if(!byte_lane_status){
              mask_new=mask-mask_old;
            }else{
              mask_new=((u32)((u8)((mask>>U24_BITS)-(mask_old>>U24_BITS)))<<U24_BITS)+((u32)((u8)((mask>>U16_BITS)-(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)-(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask-mask_old);
            }
            u32_list_base[mask_idx]=mask_new;
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode){
            mask_idx=(ULONG)(mask_idx_min+1);
            mask_old=0;
            do{
              mask=u32_list_base[mask_idx];
              if(!byte_lane_status){
                mask_new=mask-mask_old;
              }else{
                mask_new=((u32)((u8)((mask>>U24_BITS)-(mask_old>>U24_BITS)))<<U24_BITS)+((u32)((u8)((mask>>U16_BITS)-(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)-(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask-mask_old);
              }
              u32_list_base[mask_idx]=mask_new;
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        case 1:
        case 3:
          mask_old=u32_list_base[mask_idx];
          mask_idx++;
          do{
            mask=u32_list_base[mask_idx];
            if(!byte_lane_status){
              mask+=mask_old;
            }else{
              mask=((u32)((u8)((mask>>U24_BITS)+(mask_old>>U24_BITS)))<<U24_BITS)+((u32)((u8)((mask>>U16_BITS)+(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)+(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask+mask_old);
            }
            u32_list_base[mask_idx]=mask;
            mask_old=mask;
          }while((mask_idx++)!=mask_idx_max);
          if(mode==3){
            mask_idx=mask_idx_min;
            mask_idx_min--;
            mask_old=u32_list_base[mask_idx_min];
            do{
              mask=u32_list_base[mask_idx];
              if(!byte_lane_status){
                mask+=mask_old;
              }else{
                mask=((u32)((u8)((mask>>U24_BITS)+(mask_old>>U24_BITS)))<<U24_BITS)+((u32)((u8)((mask>>U16_BITS)+(mask_old>>U16_BITS)))<<U16_BITS)+((u32)((u8)((mask>>U8_BITS)+(mask_old>>U8_BITS)))<<U8_BITS)+(u8)(mask+mask_old);
              }
              u32_list_base[mask_idx]=mask;
              mask_old=mask;
            }while((mask_idx++)!=mask_idx_max);
          }
          break;
        }
        break;
      }
      frame_idx++;
    }while(frame_idx<frame_count);
    filename_base=argv[6];
    handle=fopen(filename_base, "wb");
    if(!handle){
      DEBUG_PRINT("ERROR: Cannot open output file for writing!\n");
      break;
    }
    if(header_status&&header_size){
      transfer_size=(ULONG)(fwrite(header_base, (size_t)(U8_SIZE), (size_t)(header_size), handle));
      if(transfer_size!=header_size){
        DEBUG_PRINT("ERROR: Header write failed!\n");
        break;
      }
    }
    switch(mask_granularity){
    case 0:
    case 2:
      transfer_size=(ULONG)(fwrite(u8_list_base, (size_t)(U8_SIZE), (size_t)(transfer_size_old), handle));
      break;
    case 1:
      transfer_size=(ULONG)(fwrite((u8 *)(u16_list_base), (size_t)(U8_SIZE), (size_t)(transfer_size_old), handle));
      break;
    case 3:
     transfer_size=(ULONG)(fwrite((u8 *)(u32_list_base), (size_t)(U8_SIZE), (size_t)(transfer_size_old), handle));
     break;
    }
    if(transfer_size!=transfer_size_old){
      DEBUG_PRINT("ERROR: Output file write failed!\n");
      break;
    }
    if(header_status&&footer_size){
      transfer_size_old=(ULONG)(fwrite(footer_base, (size_t)(U8_SIZE), (size_t)(footer_size), handle));
      if(transfer_size_old!=footer_size){
        DEBUG_PRINT("ERROR: Footer write failed!\n");
        break;
      }
    }
    fclose(handle);
    handle=NULL;
    status=0;
  }while(0);
  if(handle){
    fclose(handle);
  }
  free(footer_base);
  free(header_base);
  free(u8_list_base);
  free(u32_list_base);
  free(u16_list_base);
  return status;
}

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
Dyspoissometer for Files
*/
#include "flag.h"
#include "flag_ascii.h"
#include "flag_dyspoissometer.h"
#include "flag_dyspoissofile.h"
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
dyspoissofile_out_of_memory_print(void){
  DEBUG_PRINT("\nERROR: Out of memory!");
  #if(DYSPOISSOMETER_UINT_BIT_MAX==U16_BIT_MAX)
    DEBUG_PRINT(" \"make dyspoissofile_double\" might help.");
  #elif(DYSPOISSOMETER_UINT_BIT_MAX==U32_BIT_MAX)
    DEBUG_PRINT(" \"make dyspoissofile_quad\" might help.");
  #endif
  DEBUG_PRINT("\n");
  return;
}

int
main(int argc, char *argv[]){
  u8 automask_status;
  DYSPOISSOMETER_NUMBER dyspoissonism;
  u64 dyspoissonism_fraction;
  u64 file_size;
  char *filename_base;
  u8 granularity;
  u8 granularity_status;
  FILE *handle;
  DYSPOISSOMETER_NUMBER information_sparsity;
  u64 information_sparsity_fraction;
  DYSPOISSOMETER_UINT iteration_max;
  DYSPOISSOMETER_NUMBER logfreedom;
  DYSPOISSOMETER_NUMBER logfreedom_max;
  DYSPOISSOMETER_NUMBER logfreedom_median;
  u8 logfreedom_status;
  u32 mask;
  u64 mask_count;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_UINT mask_idx_max;
  u32 mask_max;
  DYSPOISSOMETER_UINT mask_max_uint;
  u8 max_log2;
  u8 median_log2;
  u64 parameter;
  u64 random_seed;
  u8 sign_status;
  DYSPOISSOMETER_NUMBER stat_kurtosis;
  DYSPOISSOMETER_NUMBER stat_mean;
  DYSPOISSOMETER_NUMBER stat_sigma;
  u8 stat_status;
  DYSPOISSOMETER_NUMBER stat_variance;
  u8 status;
  u64 transfer_size;
  u16 *u16_list_base;
  u32 *u32_list_base;
  u64 u8_idx;
  u8 *u8_list_base;

  status=ascii_init(0, 0);
  status=(u8)(status|dyspoissometer_init(0, 1));
  handle=NULL;
  u16_list_base=NULL;
  u32_list_base=NULL;
  u8_list_base=NULL;
  random_seed=1;
  do{
    if(status){
      DEBUG_PRINT("\nERROR: Outdated source code!\n");
      break;
    }
    status=1;
    if((argc<4)||(6<argc)){
      DEBUG_PRINT("Dyspoissometer for Files\nCopyright 2016 Russell Leidich\nhttp://dyspoissonism.blogspot.com\n");
      DEBUG_U32("build_id_in_hex", DYSPOISSOFILE_BUILD_ID);
      DEBUG_PRINT("Derive file statistics with Dyspoissometer.\n\n");
      DEBUG_PRINT("Syntax:\n\n");
      DEBUG_PRINT("dyspoissofile filename automask_status granularity [max_log2 [median_log2]]\n\n");
      DEBUG_PRINT("where:\n\n");
      DEBUG_PRINT("filename name of the file to analyze.\n\n");
      DEBUG_PRINT("automask_status is 0 to assume that the mask count is (256^(granularity+1)),\nelse 1 to discover it by examining the file at the given granularity.\n\n");
      DEBUG_PRINT("granularity is the number of bytes per mask, less one: 0 for 8-bit masks, 1 for\n16-bit, 2 for 24-bit, or 3 for 32-bit. Add 4 to this value if you also want to\ncompute the mean, standard deviation, variance, and kurtosis of the masks,\nwhich is slow and does not involve Bessel's correction because the population\nis assumed to be complete. Add 8 if you want all that and the masks are also\nsigned. Note that 32-bit masks require O(10^11) bytes of memory!\n\n");
      DEBUG_PRINT("max_log2 (optional) is zero if computing maximum logfreedom is not desired,\nelse the log2 (as a decimal integer) of the number of iterations to perform in\norder to approximate it with the given mask count and mask span. Lesser values\ncan produce larger errors but greater values waste time, so experiment!\n\n");
      DEBUG_PRINT("median_log2 (optional) is zero if computing median logfreedom is not desired,\nor else the equivalent of max_log2, but for the computation of median as\nopposed to maximum logfreedom.\n\n");
      DEBUG_PRINT("Note that the output is displayed in a combination of floating-point and\nhexadecimal. The hexadecimal values are rounded-nearest fractions where (2^63)\nis one.\n");
      break;
    }
    status=ascii_decimal_to_u64_convert(argv[2], &parameter, 1);
    if(status){
      DEBUG_PRINT("ERROR: Invalid automask_status!\n");
      break;
    }
    automask_status=(u8)(parameter);
    status=ascii_decimal_to_u64_convert(argv[3], &parameter, U32_BYTE_MAX+4+8);
    if(status){
      DEBUG_PRINT("ERROR: Invalid granularity!\n");
      break;
    }
    granularity=(u8)((u8)(parameter)&3);
    sign_status=(u8)(((u8)(parameter)>>3)&1);
    stat_status=(u8)(((u8)(parameter)>>2)&1);
    max_log2=0;
    median_log2=0;
    if(4<argc){
      status=ascii_decimal_to_u64_convert(argv[4], &parameter, DYSPOISSOMETER_UINT_BIT_MAX);
      if(status){
        DEBUG_PRINT("ERROR: Invalid max_count_log2!\n");
        break;
      }
      max_log2=(u8)(parameter);
      if(argc==6){
        status=ascii_decimal_to_u64_convert(argv[5], &parameter, DYSPOISSOMETER_UINT_BIT_MAX);
        if(status){
          DEBUG_PRINT("ERROR: Invalid median_count_log2!\n");
          break;
        }
        median_log2=(u8)(parameter);
      }
    }
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
    granularity_status=1;
    mask_count=file_size;
    switch(granularity){
    case 0:
      granularity_status=0;
      status=0;
      u8_list_base=(u8 *)(malloc((size_t)(file_size)));
      status=!u8_list_base;
      break;
    case 1:
      mask_count>>=U16_SIZE_LOG2;
      if((mask_count<<U16_SIZE_LOG2)==file_size){
        granularity_status=0;
        u16_list_base=(u16 *)(malloc((size_t)(file_size)));
        status=!u16_list_base;
      }
      break;
    case 2:
      mask_count/=3;
      if((mask_count*3)==file_size){
        granularity_status=0;
        u8_list_base=(u8 *)(malloc((size_t)(file_size)));
        status=!u8_list_base;
      }
      break;
    case 3:
      mask_count>>=U32_SIZE_LOG2;
      if((mask_count<<U32_SIZE_LOG2)==file_size){
        granularity_status=0;
        u32_list_base=(u32 *)(malloc((size_t)(file_size)));
        status=!u32_list_base;
      }
      break;
    }
    if(granularity_status){
      DEBUG_PRINT("ERROR: File size is not a multiple of (granularity+1) bytes!\n");
      break;
    }
    if(ULONG_MAX<=file_size){
      status=1;
    }
    if(status){
      dyspoissofile_out_of_memory_print();
      break;
    }
    status=1;
    if(DYSPOISSOMETER_UINT_MAX<=mask_count){
      DEBUG_PRINT("ERROR: More precision is required for this big file. Try\n\"make dyspoissofile_double\" or \"make dyspoissofile_quad\".\n");
      break;
    }
    transfer_size=0;
    switch(granularity){
    case 0:
      transfer_size=(u64)(fread(u8_list_base, (size_t)(U8_SIZE), (size_t)(file_size), handle));
      break;
    case 1:
      transfer_size=(u64)(fread(u16_list_base, (size_t)(U8_SIZE), (size_t)(file_size), handle));
      break;
    case 2:
      transfer_size=(u64)(fread(u8_list_base, (size_t)(U8_SIZE), (size_t)(file_size), handle));
      break;
    case 3:
      transfer_size=(u64)(fread(u32_list_base, (size_t)(U8_SIZE), (size_t)(file_size), handle));
      break;
    }
    if(file_size!=transfer_size){
      DEBUG_PRINT("ERROR: File read failed!\n");
      break;
    }
    mask_idx_max=(DYSPOISSOMETER_UINT)(mask_count-1);
    mask_max=(u32)(((u32)(1U<<(granularity<<U8_BITS_LOG2))<<U8_BITS)-1);
    logfreedom=0.0f;
    stat_mean=0.0f;
    switch(granularity){
    case 0:
      if(automask_status){
        mask_idx=0;
        mask_max=0;
        do{
          mask=u8_list_base[mask_idx];
          if(mask_max<mask){
            mask_max=mask;
          }
        }while((mask_idx++)!=mask_idx_max);
      }
      logfreedom=dyspoissometer_u8_list_logfreedom_get(mask_idx_max, 0, (u8)(mask_max), u8_list_base);
      if(stat_status){
        stat_mean=dyspoissometer_u8_list_stats_get(mask_idx_max, 0, sign_status, &stat_kurtosis, &stat_sigma, &stat_variance, u8_list_base);
      }
      break;
    case 1:
      if(automask_status){
        mask_idx=0;
        mask_max=0;
        do{
          mask=u16_list_base[mask_idx];
          if(mask_max<mask){
            mask_max=mask;
          }
        }while((mask_idx++)!=mask_idx_max);
      }
      logfreedom=dyspoissometer_u16_list_logfreedom_get(mask_idx_max, 0, (u16)(mask_max), u16_list_base);
      if(stat_status){
        stat_mean=dyspoissometer_u16_list_stats_get(mask_idx_max, 0, sign_status, &stat_kurtosis, &stat_sigma, &stat_variance, u16_list_base);
      }
      break;
    case 2:
      if(automask_status){
        mask_idx=0;
        mask_max=0;
        u8_idx=0;
        do{
          mask=(u32)((u32)(u8_list_base[u8_idx+2])<<U16_BITS)+(u16)((u16)(u8_list_base[u8_idx+1])<<U8_BITS)+u8_list_base[u8_idx];
          if(mask_max<mask){
            mask_max=mask;
          }
          u8_idx+=3;
        }while((mask_idx++)!=mask_idx_max);
      }
      logfreedom=dyspoissometer_u24_list_logfreedom_get(mask_idx_max, 0, mask_max, u8_list_base);
      if(stat_status){
        stat_mean=dyspoissometer_u24_list_stats_get(mask_idx_max, 0, sign_status, &stat_kurtosis, &stat_sigma, &stat_variance, u8_list_base);
      }
      break;
    case 3:
      if(automask_status){
        mask_idx=0;
        mask_max=0;
        do{
          mask=u32_list_base[mask_idx];
          if(mask_max<mask){
            mask_max=mask;
          }
        }while((mask_idx++)!=mask_idx_max);
      }
      logfreedom=dyspoissometer_u32_list_logfreedom_get(mask_idx_max, 0, mask_max, u32_list_base);
      if(stat_status){
        stat_mean=dyspoissometer_u32_list_stats_get(mask_idx_max, 0, sign_status, &stat_kurtosis, &stat_sigma, &stat_variance, u32_list_base);
      }
      break;
    }
    if(logfreedom<0.0f){
      dyspoissofile_out_of_memory_print();
      break;
    }
    DEBUG_PRINT("filename=");
    DEBUG_PRINT(filename_base);
    DEBUG_PRINT("\n");
    DEBUG_U64("mask_idx_max", mask_idx_max);
    DEBUG_U64("mask_max", mask_max);
    DEBUG_NUMBER("logfreedom", logfreedom);
    if(stat_status){
      if(!sign_status){
        DEBUG_NUMBER("mean_unsigned", stat_mean);
        DEBUG_NUMBER("sigma_unsigned", stat_sigma);
        DEBUG_NUMBER("variance_unsigned", stat_variance);
        DEBUG_NUMBER("kurtosis_unsigned", stat_kurtosis);
      }else{
        DEBUG_NUMBER("mean_signed", stat_mean);
        DEBUG_NUMBER("sigma_signed", stat_sigma);
        DEBUG_NUMBER("variance_signed", stat_variance);
        DEBUG_NUMBER("kurtosis_signed", stat_kurtosis);
      }
    }
    mask_max_uint=(DYSPOISSOMETER_UINT)(mask_max);
    dyspoissonism=dyspoissometer_dyspoissonism_get(logfreedom, mask_idx_max, mask_max_uint);
    DEBUG_NUMBER("dyspoissonism", dyspoissonism);
    dyspoissonism_fraction=(u64)((dyspoissonism*U64_SPAN_HALF)+0.5f);
    DEBUG_U64("dyspoissonism_fraction", dyspoissonism_fraction);
    if(max_log2){
      DEBUG_PRINT("Approximating maximum logfreedom...\n");
      iteration_max=(DYSPOISSOMETER_UINT)((1ULL<<max_log2)-1);
      DEBUG_U64("logfreedom_max_iteration_max", iteration_max);
      logfreedom_max=dyspoissometer_logfreedom_max_get(iteration_max, mask_idx_max, mask_max_uint, &random_seed);
      if(logfreedom_max<0.0f){
        dyspoissofile_out_of_memory_print();
        break;
      }
      DEBUG_NUMBER("logfreedom_max_approximated", logfreedom_max);
      logfreedom_status=0;
      if(logfreedom_max<logfreedom){
        logfreedom_max=logfreedom;
        logfreedom_status=1;
      }
      information_sparsity=dyspoissometer_sparsity_get(logfreedom, logfreedom_max);
      DEBUG_NUMBER("information_sparsity", information_sparsity);
      information_sparsity_fraction=(u64)((information_sparsity*U64_SPAN_HALF)+0.5f);
      DEBUG_U64("information_sparsity_fraction", information_sparsity_fraction);
      if(logfreedom_status){
        DEBUG_PRINT("\nWARNING: That file had greater logfreedom than my own estimate of the maximum!\nNext time try with a greater iteration_count_log2.\n");
      }
    }
    if(median_log2){
      DEBUG_PRINT("Approximating median logfreedom...\n");
      iteration_max=(DYSPOISSOMETER_UINT)((1ULL<<median_log2)-1);
      DEBUG_U64("logfreedom_median_iteration_max", iteration_max);
      logfreedom_median=dyspoissometer_logfreedom_median_get(iteration_max, mask_idx_max, mask_max_uint, &random_seed);
      if(logfreedom_median<0.0f){
        dyspoissofile_out_of_memory_print();
        break;
      }
      DEBUG_NUMBER("logfreedom_median_approximated", logfreedom_median);
    }
    status=0;
  }while(0);
  if(handle){
    fclose(handle);
  }
  free(u8_list_base);
  free(u32_list_base);
  free(u16_list_base);
  return status;
}

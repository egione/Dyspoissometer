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
#ifdef DEBUG
  #include "flag.h"
  #include <float.h>
  #include <math.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "constant.h"
  #include "debug_xtrn.h"

  #define DEBUG_FREE_CORRUPTION_BYTE 0xD7
  #define DEBUG_MALLOC_CORRUPTION_BYTE 0xA9

  static u64 debug_allocation_count=0;

  void
  debug_allocation_check(void){
/*
Verify that all allocations have been freed exactly once.
*/
    if(debug_allocation_count){
      debug_print("\nERROR: Memory allocation count does not equal memory free count.\n");
    }
    return;
  }

  void
  debug_bitmap(char *name_base, u64 bit_count, u64 bit_idx_min, ULONG *chunk_list_base){
/*
Print a bitmap starting with its least significant bit.

In:

  *name_base is a text identifier for the output.

  bit_count is the number of bits to print.

  bit_idx_min is the bit index at which to begin print.

  *chunk_list_base is a list of ULONG chunks of bits containing the bitmap.
*/
    u8 bit;
    u8 row_bit_idx;

    printf("%s[%08X%08X]={\n", name_base, (u32)(bit_count>>U32_BITS), (u32)(bit_count));
    row_bit_idx=0;
    if(bit_count){
      do{
        if(!row_bit_idx){
          printf("  ");
        }
        bit=(u8)(BIT_GET(chunk_list_base, bit_idx_min));
        bit_idx_min++;
        printf("%d", bit);
        if(row_bit_idx!=U64_BIT_MAX){
          row_bit_idx++;
        }else{
          row_bit_idx=0;
          printf("\n");
        }
        bit_count--;
      }while(bit_count);
      if(row_bit_idx){
        printf("\n");
      }
    }
    printf("}\n");
    debug_print_flush();
    return;
  }

  void *
  debug_calloc_paranoid(ULONG size){
/*
Allocate zeroed memory using canary signatures for the detection of uninitialized usage and memory leaks.

In:

  size is the nonzero number of zero (u8)s to allocate.

Out:

  Returns NULL on failure, else the base of (size) bytes of zeroes surrounded by leak canaries.
*/
    void *base;
    ULONG size_plus_prefix;
    ULONG size_plus_prefix_plus_suffix;

    size_plus_prefix=(ULONG)((U64_SIZE<<1)+size);
    size_plus_prefix_plus_suffix=(ULONG)(size_plus_prefix+sizeof(void *));
    base=NULL;
    if(size<size_plus_prefix_plus_suffix){
      base=(u8 *)(malloc((size_t)(size_plus_prefix_plus_suffix)));
      if(base){
        debug_allocation_count++;
        memcpy(base, &size_plus_prefix, (size_t)(ULONG_SIZE));
        memcpy(&((u8 *)(base))[size_plus_prefix], &base, sizeof(void *));
/*
Increase base by the size of the largest atomic object; otherwise we might segfault. Then zero the newly allocated memory.
*/
        base+=(U64_SIZE<<1);
        memset(base, 0, (size_t)(size));
      }
    }
    return base;
  }

  void
  debug_double(char *name_base, double value){
/*
Print a double-precision floating-point value.

In:

  *name_base is a text identifier for the output.

  value is the double to print.
*/
    printf("%s=%+-1.15E\n", name_base, value);
    return;
  }

  void
  debug_double_list(char *name_base, ULONG double_count, double *double_list_base){
/*
Print a list of double-precision floating-point values.

In:

  *name_base is a text identifier for the output.

  double_count is the number of (double)s to print.

  *double_list_base is a list of (double)s.
*/
    ULONG double_idx;

    #ifdef _64_
      printf("%s[%08X%08X]={\n", name_base, (u32)(double_count>>U32_BITS), (u32)(double_count));
    #else
      printf("%s[%08X%08X]={\n", name_base, 0, double_count);
    #endif
    for(double_idx=0; double_idx<double_count; double_idx++){
      printf("  %+-1.15E\n", double_list_base[double_idx]);
    }
    printf("}\n");
    debug_print_flush();
    return;
  }

  void
  debug_exit(int status){
/*
Perform exit() in a manner which is easily trapped by instrumentation.
*/
    exit(status);
  }

  void
  debug_float(char *name_base, float value){
/*
Print a single-precision floating-point value.

In:

  *name_base is a text identifier for the output.

  value is the float to print.
*/
    printf("%s=%+-1.7E\n", name_base, value);
    return;
  }

  void
  debug_float_list(char *name_base, ULONG float_count, float *float_list_base){
/*
Print a list of single-precision floating-point values.

In:

  *name_base is a text identifier for the output.

  float_count is the number of (float)s to print.

  *float_list_base is a list of (float)s.
*/
    ULONG float_idx;

    #ifdef _64_
      printf("%s[%08X%08X]={\n", name_base, (u32)(float_count>>U32_BITS), (u32)(float_count));
    #else
      printf("%s[%08X%08X]={\n", name_base, 0, float_count);
    #endif
    for(float_idx=0; float_idx<float_count; float_idx++){
      printf("  %+-1.7E\n", float_list_base[float_idx]);
    }
    printf("}\n");
    debug_print_flush();
    return;
  }

  void
  debug_free_paranoid(void *base){
/*
Free memory after checking for memory leaks and installing a canary against illegal reuse.

In:

  base is the return value from debug_malloc_paranoid(), which may be NULL.

Out:

  Aborts via debug_memory_leak_report() if a leak is detected.
*/
    ULONG size_plus_prefix;
    void *size_plus_prefix_base;
    ULONG size_plus_prefix_plus_suffix;

    if(base){
      size_plus_prefix_base=base-(U64_SIZE<<1);
      memcpy(&size_plus_prefix, size_plus_prefix_base, (size_t)(ULONG_SIZE));
      if(memcmp(&size_plus_prefix_base, &((u8 *)(size_plus_prefix_base))[size_plus_prefix], sizeof(void *))){
        debug_memory_leak_report(base);
      }
/*
Corrupt the memory before we free it in order to improve detection of use-after-free.
*/
      size_plus_prefix_plus_suffix=(ULONG)(size_plus_prefix+sizeof(void *));
      memset(size_plus_prefix_base, DEBUG_FREE_CORRUPTION_BYTE, (size_t)(size_plus_prefix_plus_suffix));
      free(size_plus_prefix_base);
      debug_allocation_count--;
    }
    return;
  }

  void
  debug_line(ULONG line_idx){
/*
Print the line number of the line at which this function is called, to facilitate easy execution tracking.

In:

  line_idx is the source code line, taken from the __LINE__ compiler variable.
*/
    printf("~%d", (u32)(line_idx));
    debug_print_flush();
    return;
  }

  void
  debug_list(char *name_base, ULONG chunk_count, u8 *chunk_list_base, u8 chunk_size_log2){
/*
Print a list of chunks of 8/16/32/64-bit size.

In:

  *name_base is a text identifier for the output.

  chunk_count is the number of chunk to print.

  chunk_list_base is base of the list of chunks casted to (u8 *). This is normally considered bad coding practice, but this is only the debugger, which should not ship with published code, so the tradeoff favors convenience.

  chunk_size_log2 is 0/1/2/3 for 8/16/32/64-bit chunks, respectively.
*/
    debug_list_custom(name_base, chunk_count, chunk_list_base, chunk_size_log2, 0);
    return;
  }

  void
  debug_list_custom(char *name_base, ULONG chunk_count, u8 *chunk_list_base, u8 chunk_size_log2, u8 compilable_status){
/*
Print a list of chunks of 8/16/32/64-bit size.

In:

  *name_base is as defined in debug_list().

  chunk_count is as defined in debug_list().

  chunk_list_base is as defined in debug_list().

  chunk_size_log2 is as defined in debug_list().

  compilable_status is zero for a compact output format, else one if the output is intended to be compiled as C source code.
*/
    u8 byte;
    u8 byte_count;
    u8 char_idx;
    char char_list_base[0x20*3];
    u8 chunk_size;
    u8 column_count;
    ULONG i;
    u8 j;
    u8 nybble;
    ULONG row_count;
    ULONG u8_idx;

    if(chunk_size_log2<=U64_SIZE_LOG2){
      #ifdef _64_
        printf("%s[%08X%08X]={\n", name_base, (u32)(chunk_count>>U32_BITS), (u32)(chunk_count));
      #else
        printf("%s[%08X%08X]={\n", name_base, 0, chunk_count);
      #endif
      if(chunk_list_base){
        column_count=8;
        if(chunk_size_log2==U64_SIZE_LOG2){
          column_count=4;
        }else if(chunk_size_log2==U8_SIZE_LOG2){
          column_count=16;
        }
        if(compilable_status){
          column_count=(u8)((80-2-1)/((2U<<chunk_size_log2)+4));
        }
        row_count=(ULONG)((chunk_count/column_count)+!!(chunk_count%column_count));
        chunk_size=(u8)(1U<<chunk_size_log2);
        u8_idx=0;
        for(i=(ULONG)(row_count-1); i<row_count; i--){
          if(!i){
            chunk_count=(ULONG)(chunk_count%column_count);
            if(chunk_count){
              column_count=(u8)(chunk_count);
            }
          }
          char_idx=0;
          for(j=(u8)(column_count-1); j<column_count; j--){
            if(compilable_status){
              char_list_base[char_idx]='0';
              char_idx++;
              char_list_base[char_idx]='x';
              char_idx++;
            }
            byte_count=chunk_size;
            u8_idx=(ULONG)(u8_idx+chunk_size);
            do{
              u8_idx--;
              byte=chunk_list_base[u8_idx];
              nybble=(u8)(byte>>4);
              if(nybble<=9){
                nybble=(u8)(nybble+'0');
              }else{
                nybble=(u8)(nybble+'A'-0xA);
              }
              char_list_base[char_idx]=(char)(nybble);
              char_idx++;
              nybble=(u8)(byte&0xF);
              if(nybble<=9){
                nybble=(u8)(nybble+'0');
              }else{
                nybble=(u8)(nybble+'A'-0xA);
              }
              char_list_base[char_idx]=(char)(nybble);
              char_idx++;
              byte_count--;
            }while(byte_count);
            u8_idx=(ULONG)(u8_idx+chunk_size);
            if(j){
              char_list_base[char_idx]=',';
              char_idx++;
              if(compilable_status){
                char_list_base[char_idx]=' ';
                char_idx++;
              }
            }else{
              if(i){
                char_list_base[char_idx]=',';
                char_idx++;
                if(compilable_status){
                  char_list_base[char_idx]=' ';
                  char_idx++;
                  char_list_base[char_idx]='\\';
                  char_idx++;
                }
              }
              char_list_base[char_idx]=0;
            }
          }
          printf("  %s\n", char_list_base);
        }
        printf("}\n");
      }else{
        printf("\ndebug_list_custom(): List has NULL base.\n");
        debug_exit(1);
      }
    }else{
      printf("\ndebug_list_custom(): chunk_size_log2>%d.\n", (int)(U64_SIZE_LOG2));
      debug_exit(1);
    }
    debug_print_flush();
    return;
  }

  void *
  debug_malloc_paranoid(ULONG size){
/*
Allocate memory using autocorruption and canary signatures for the detection of uninitialized usage and memory leaks.

In:

  size is the nonzero number of (u8)s to allocate.

Out:

  Returns NULL on failure, else the base of (size) bytes of deterministically corrupted memory surrounded by leak canaries.
*/
    void *base;
    ULONG size_plus_prefix;
    ULONG size_plus_prefix_plus_suffix;

    size_plus_prefix=(ULONG)((U64_SIZE<<1)+size);
    size_plus_prefix_plus_suffix=(ULONG)(size_plus_prefix+sizeof(void *));
    base=NULL;
    if(size<size_plus_prefix_plus_suffix){
      base=(u8 *)(malloc((size_t)(size_plus_prefix_plus_suffix)));
      if(base){
        debug_allocation_count++;
        memcpy(base, &size_plus_prefix, (size_t)(ULONG_SIZE));
        memcpy(&((u8 *)(base))[size_plus_prefix], &base, sizeof(void *));
/*
Increase base by the size of the largest atomic object; otherwise we might segfault. Then corrupt the newly allocated memory with repeated garbage; this is fast and catches many memory leaks.
*/
        base+=(U64_SIZE<<1);
        memset(base, DEBUG_MALLOC_CORRUPTION_BYTE, (size_t)(size));
      }
    }
    return base;
  }

  void
  debug_memory_leak_report(void *base){
/*
Report a memory leak then exit.
*/
    debug_ptr("memory_leak_base", base);
    debug_exit(1);
  }

  void
  debug_print(char *string_base){
/*
Print a string.
*/
    printf("%s", string_base);
    debug_print_flush();
    return;
  }

  void
  debug_print_flush(void){
/*
Flush the OS print queue in order to ensure that text display occurs before the calling program can crash.
*/
    fflush(stdout);
    return;
  }

  void
  debug_print_if(u8 status, char *string_base){
/*
Print a string if and only if a status value is nonzero.
*/
    if(status){
      debug_print(string_base);
    }
    return;
  }

  void
  debug_ptr(char *name_base, void *base){
/*
Print a pointer to a memory address.

In:

  *name_base is a text identifier for the output.

  base is the pointer to print.
*/
    u64 base_u64;

    base_u64=0;
    memcpy(&base_u64, &base, sizeof(void *));
    debug_u64(name_base, base_u64);
    return;
  }

  void *
  debug_realloc_paranoid(void *base, ULONG size){
/*
Reallocate memory using autocorruption and canary signatures for the detection of uninitialized usage and memory leaks.

In:

  base is the nonnull return value from debug_malloc_paranoid().

  size is the nonzero number of (u8)s in the new allocation.

Out:

  Aborts via debug_memory_leak_report() if a leak is detected. Else returns a (void *) to the new allocation. If different from base, then base has been freed unless it's NULL, in which case nothing has changed and the reallocation failed.

  *base is only defined for the minimum of its previous and new sizes.
*/
    ULONG size_plus_prefix;
    ULONG size_plus_prefix_old;
    void *size_plus_prefix_old_base;
    ULONG size_plus_prefix_plus_suffix;

    size_plus_prefix_old_base=base-(U64_SIZE<<1);
    memcpy(&size_plus_prefix_old, size_plus_prefix_old_base, (size_t)(ULONG_SIZE));
    if(!memcmp(&((u8 *)(size_plus_prefix_old_base))[size_plus_prefix_old], &size_plus_prefix_old_base, sizeof(void *))){
      size_plus_prefix_plus_suffix=(ULONG)((U64_SIZE<<1)+size+sizeof(void *));
      base=NULL;
      if(size<size_plus_prefix_plus_suffix){
        base=realloc(size_plus_prefix_old_base, (size_t)(size_plus_prefix_plus_suffix));
        if(base){
          size_plus_prefix=(ULONG)((U64_SIZE<<1)+size);
/*
Continue the corruption pattern as though debug_malloc_paranoid() had done it in the first place.
*/
          if(size_plus_prefix_old<size_plus_prefix){
            size=(ULONG)(size_plus_prefix-size_plus_prefix_old);
            memset(&((u8 *)(base))[size_plus_prefix_old], DEBUG_MALLOC_CORRUPTION_BYTE, (size_t)(size));
          }
          memcpy(base, &size_plus_prefix, (size_t)(ULONG_SIZE));
          memcpy(&((u8 *)(base))[size_plus_prefix], &base, sizeof(void *));
          base+=(U64_SIZE<<1);
        }
      }
    }else{
      debug_memory_leak_report(base);
    }    
    return base;
  }

  void
  debug_u16(char *name_base, u16 value){
/*
Print a u16 value.

In:

  *name_base is a text identifier for the output.

  value is the u16 to print.
*/
    printf("%s=%04X\n", name_base, value);
    debug_print_flush();
    return;
  }

  void
  debug_u24(char *name_base, u32 value){
/*
Print a u24 value.

In:

  *name_base is a text identifier for the output.

  value is the u32 to print as a u24.
*/
    printf("%s=%02X%04X\n", name_base, (u8)(value>>U16_BITS), (u16)(value));
    debug_print_flush();
    return;
  }

  void
  debug_u32(char *name_base, u32 value){
/*
Print a u32 value.

In:

  *name_base is a text identifier for the output.

  value is the u32 to print.
*/
    printf("%s=%08X\n", name_base, value);
    debug_print_flush();
    return;
  }

  void
  debug_u64(char *name_base, u64 value){
/*
Print a u64 value.

In:

  *name_base is a text identifier for the output.

  value is the u64 to print.
*/
    printf("%s=%08X%08X\n", name_base, (u32)(value>>U32_BITS), (u32)(value));
    debug_print_flush();
    return;
  }

  void
  debug_u8(char *name_base, u8 value){
/*
Print a u8 value.

In:

  *name_base is a text identifier for the output.

  value is the u8 to print.
*/
    printf("%s=%02X\n", name_base, value);
    debug_print_flush();
    return;
  }
#endif

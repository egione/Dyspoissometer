/*
ASCII
Copyright 2016 Russell Leidich

This collection of files constitutes the ASCII Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The ASCII Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The ASCII Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the ASCII Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
ASCII Conversion Functions
*/
#include "flag.h"
#include "flag_ascii.h"
#include <stdint.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"

u8
ascii_decimal_to_u64_convert(char *digit_list_base, u64 *integer_base, u64 integer_max){
/*
Convert an allegedly ASCII decimal string to a u64.

In:

  *digit_list_base points to an untrustued string hopefully containing ASCII digits 0 through 9. Null termination must be guaranteed to occur within contiguously readable space.

  integer_base of the base of a u64 to receive the binary value of *digit_list_base.

  integer_max is the maximum acceptable value of Out:*integer_base.

Out:

  Returns zero on success, else one.

  *integer_base zero on failure, else the binary value of *digit_list_base on [0, integer_max].
*/
  u8 digit;
  u32 digit_idx;
  u64 integer;
  u8 status;

  integer=0;
  *integer_base=integer;
  status=1;
  if(digit_list_base[0]){
    digit_idx=0;
    do{
      digit=(u8)(digit_list_base[digit_idx]);
      digit_idx++;
      if(digit){
        digit=(u8)(digit-'0');
        if(digit<=9){
          if((U64_MAX/10)<=integer){
            if((U64_MAX/10)==integer){
              if(5<digit){
                break;
              }
            }else{
              break;
            }
          }
          integer=(integer*10)+digit;
        }else{
          break;
        }
      }else{
        if(integer<=integer_max){
          status=0;
          *integer_base=integer;
        }
        break;
      }
    }while(1);
  }
  return status;  
}

u8
ascii_hex_to_u64_convert(char *digit_list_base, u64 *integer_base, u64 integer_max){
/*
Convert an allegedly ASCII hexadecimal string to a u64.

In:

  *digit_list_base points to an untrustued string hopefully containing ASCII digits 0 through 9, "A" through "F", or "a" through "f". Null termination must be guaranteed to occur within contiguously readable space.

  integer_base of the base of a u64 to receive the binary value of *digit_list_base.

  integer_max is the maximum acceptable value of Out:*integer_base.

Out:

  Returns zero on success, else one.

  *integer_base zero on failure, else the binary value of *digit_list_base on [0, integer_max].
*/
  u8 digit;
  u32 digit_idx;
  u64 integer;
  u8 status;

  integer=0;
  *integer_base=integer;
  status=1;
  if(digit_list_base[0]){
    digit_idx=0;
    do{
      digit=(u8)(digit_list_base[digit_idx]);
      digit_idx++;
      if(digit){
        digit=(u8)(digit-'0');
        if(9<digit){
          digit=(u8)(((u8)(digit+'0')|('a'-'A'))-'a');
          if(5<digit){
            break;
          }
          digit=(u8)(digit+0xA);
        }
        if(digit_idx<=(U64_BITS>>2)){
          integer=(integer<<4)|digit;
        }else{
          break;
        }
      }else{
        if(integer<=integer_max){
          status=0;
          *integer_base=integer;
        }
        break;
      }
    }while(1);
  }
  return status;  
}

u8
ascii_hex_to_u8_list_convert(ULONG digit_idx_max, ULONG digit_idx_min, char *digit_list_base, u8 *u8_list_base){
/*
Convert an allegedly ASCII hexadecimal string to a list of (u8)s.

In:

  digit_idx_max is the maximum index of *digit_list_base which does not contain NULL.

  digit_idx_min is the index of *digit_list_base at which to start reading, on [0, digit_idx_max].
  
  *digit_list_base points to an untrustued string hopefully containing ASCII digits 0 through 9, "A" through "F", or "a" through "f". Null termination must be guaranteed to occur within contiguously readable space.

  u8_list_base is the base at which to store (u8)s converted from the ASCII hex values. It must be writable for (((digit_idx_max-digit_idx_min)>>1)+1) items.

Out:

  Returns zero on success, else one on failure due to an odd number of nybbles, else 2.

  *u8_list_base is unchanged on failure, else populated with the byte equivalent of *digit_list_base, ordered such that if (*digit_list_base=="12aB") then (u8_list_base[0]==0x12) and (u8_list_base[1]==0xAB).
*/
  u8 byte;
  u8 digit;
  ULONG digit_idx;
  u8 status;
  ULONG u8_idx;

  status=1;
  status=(u8)(((digit_idx_max-digit_idx_min)&status)^status);
  if(!status){
    digit_idx=digit_idx_min;
    status=2;
    do{
      digit=(u8)(digit_list_base[digit_idx]);
      digit=(u8)(digit-'0');
      if(9<digit){
        digit=(u8)(((u8)(digit+'0')|('a'-'A'))-'a');
        if(5<digit){
          break;
        }
        digit=(u8)(digit+0xA);
      }
      digit_idx++;
    }while(digit_idx<=digit_idx_max);
    if(digit_idx_max<digit_idx){
      digit_idx=digit_idx_min;
      status=0;
      u8_idx=0;
      do{
        digit=(u8)(digit_list_base[digit_idx]);
        digit=(u8)(digit-'0');
        digit_idx++;
        if(9<digit){
          digit=(u8)(((u8)(digit+'0')|('a'-'A'))-('a'-0xA));
        }
        byte=(u8)(digit<<4);
        digit=(u8)(digit_list_base[digit_idx]);
        digit=(u8)(digit-'0');
        digit_idx++;
        if(9<digit){
          digit=(u8)(((u8)(digit+'0')|('a'-'A'))-('a'-0xA));
        }
        byte|=digit;
        u8_list_base[u8_idx]=byte;
        u8_idx++;
      }while(digit_idx<=digit_idx_max);        
    }
  }
  return status;  
}

u8
ascii_init(u32 build_break_count, u32 build_feature_count){
/*
Verify that the source code is sufficiently updated.

In:

  build_break_count is the caller's most recent knowledge of ASCII_BUILD_BREAK_COUNT, which will fail if the caller is unaware of all critical updates.

  build_feature_count is the caller's most recent knowledge of ASCII_BUILD_FEATURE_COUNT, which will fail if this library is not up to date with the caller's expectations.

Out:

  Returns one if (build_break_count!=ASCII_BUILD_BREAK_COUNT) or (build_feature_count>ASCII_BUILD_FEATURE_COUNT). Otherwise, returns zero.
*/
  u8 status;

  status=(u8)(build_break_count!=ASCII_BUILD_BREAK_COUNT);
  status=(u8)(status|(ASCII_BUILD_FEATURE_COUNT<build_feature_count));
  return status;
}

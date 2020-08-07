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
extern u8 ascii_decimal_to_u64_convert(char *decimal_base, u64 *integer_base, u64 integer_max);
extern u8 ascii_hex_to_u64_convert(char *digit_list_base, u64 *integer_base, u64 integer_max);
extern u8 ascii_hex_to_u8_list_convert(ULONG digit_idx_max, ULONG digit_idx_min, char *digit_list_base, u8 *u8_list_base);
extern u8 ascii_init(u32 build_break_count, u32 build_feature_count);

/**
 ****************************************************************************************
 *
 * @file hash.c
 *
 * @brief Robert Jenkins' 32 bit integer hash function implementation.
 *
 * Source code downloaded from https://gist.github.com/badboy/6267743
 * Author  (Github user name): Jan-Erik Rediger (badboy)
 * Publication Author: Thomas Wang
 * Original Author: Robert Jenkins
 *
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

uint32_t hash(uint32_t a)
{
   a = (a + 0x7ed55d16) + (a << 12);
   a = (a ^ 0xc761c23c) ^ (a >> 19);
   a = (a + 0x165667b1) + (a << 5);
   a = (a + 0xd3a2646c) ^ (a << 9);
   a = (a + 0xfd7046c5) + (a << 3);
   a = (a ^ 0xb55a4f09) ^ (a >> 16);

   return a;
}

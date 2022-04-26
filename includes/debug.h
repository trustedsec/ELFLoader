#ifndef _DEBUG_H_
#define _DEBUG_H_

#if defined( DEBUG )
  #define DEBUG_PRINT(x, ...) printf(x, ##__VA_ARGS__)
 #else
   #define DEBUG_PRINT(x, ...)
 #endif
#endif

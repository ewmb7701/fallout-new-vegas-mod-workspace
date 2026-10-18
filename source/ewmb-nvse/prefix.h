#pragma once

#pragma warning(disable : 4018 4244 4267 4305 4288 4312 4311)

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned long UInt32;
typedef unsigned long long UInt64;
typedef signed char SInt8;
typedef signed short SInt16;
typedef signed long SInt32;
typedef signed long long SInt64;

#define STATIC_ASSERT(expression) static_assert(expression)

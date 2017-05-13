#ifndef ex1_h
#define ex1_h


#define TEST 0
#define NUM_ROWS 10
#define NUM_COLS 10
#define MY_MAX_PATH 1024


#include <iostream>
#include <string>
#include <fstream>

#if(TEST == 1)
#define DEBUG 1
#else
#define DEBUG 0
#endif

using namespace std;
#ifdef _UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif


#endif // !ex1_h
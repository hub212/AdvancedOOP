#ifndef UTILS_H
#define UTILS_H
#include "Types.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Board searching and indicating
class Utils
{
	////-------------------------
	////		Utils
	////-------------------------
public:

	// the function search upwards x,y in the common boards -
	// the function would stop searching when reaching water ' '
	static bool search_up(char** boards, int x, int y, char curr);

	// the function search downwards x,y in the common boards -
	// the function would stop searching when reaching water ' '
	static bool search_down(char** boards, int x, int y, char curr);

	// the function search rightwards x,y in the common boards -
	// the function would stop searching when reaching water ' '
	static bool search_right(char** boards, int x, int y, char curr);

	// the function search leftwards x,y in the common boards -
	// the function would stop searching when reaching water ' '
	static bool search_left(char** boards, int x, int y, char curr);

	// the function uses search_* functions and returns whether the current attack result is sink or not
	static bool is_sink(char** boards, int x, int y, char curr, char** boardCopy, int numRows, int numCols);


	static int copyBoard(const char** in_board, int numCols, int numRows, char*** out_board);

	// the function set the printf function to x,y coords
	static void gotoxy(int x, int y);

	static void ShowConsoleCursor(bool showFlag);

	static void GetFileNamesInDirectory(vector<string> *names, string folder);

	static 	bool string_has_suffix(const std::string &str, const std::string &suffix);

};


#endif
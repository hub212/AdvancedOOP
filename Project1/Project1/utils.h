#ifndef UTILS_H
#define UTILS_H
#include <Windows.h>
#include "Types.h"
#include "CommonAlgo.h"
#include "Game.h"

// Board searching and indicating
class Utils
{
	////-------------------------
	////		Utils
	////-------------------------
public:

	// input - 
	//		const char curr - (@,' ', BPMD, bpmd) a charecter representation of board point
	//		CommonAlgo PlayerA/B - two players with different char representation (BPMD vs. BPMD)
	//
	// output - 
	//		the function return the vessel type of the given char (transforming from char to vessel type)
	//
	// given a charecter representation of board point (@,' ', BPMD, bpmd) and two players with 
	// different char representation (BPMD vs. bpmd) the functino nill return the vessel on board (common game master board)
	// the function is being used for updating the game master board and for later (opt.) for complex algorithms
	static Vessel_ID get_vessel(const char curr, CommonAlgo playerA, CommonAlgo playerB);

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
	static bool is_sink(char** boards, int x, int y, char curr);


	static int copyBoard(const char** in_board, int numCols, int numRows, char*** out_board);


	// the function set the printf function to x,y coords
	static void gotoxy(int x, int y);

	static void ShowConsoleCursor(bool showFlag);
};


#endif
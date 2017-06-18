#ifndef UTILS_H
#define UTILS_H
#include "Types.h"
#include "Board.h"
#include <Windows.h>
#include <ostream>
#include <istream>
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

	// the function uses search_* functions and returns whether the current attack result is sink or not
	static bool is_sink(Board boards, Coordinate coor, Board boardCopy, Coordinate dims);

	// the function set the printf function to x,y coords
	static void gotoxy(int x, int y);

	static void GetFileNamesInDirectory(vector<string> *names, string folder);

	static 	bool string_has_suffix(const std::string &str, const std::string &suffix);

};

bool pairCompare(const std::pair<string, double>& firstElem, const std::pair<string, double>& secondElem);

/////////////////////////////////
//////	Coordinate
/////////////////////////////////

bool operator == (const Coordinate coor0, const Coordinate coor1);

bool operator != (const Coordinate coor0, const Coordinate coor1);

std::ostream& operator<<(std::ostream & out, const Coordinate& coor);

Coordinate operator + (const Coordinate coor0, const Coordinate coor1);


/////////////////////////////////
//////	Match
/////////////////////////////////

std::ostream& operator<<(std::ostream & out, const Match& match);

std::ostream& operator<<(std::ostream & out, const MatchHard& match);


#endif
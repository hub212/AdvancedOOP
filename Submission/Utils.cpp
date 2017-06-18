#include "Utils.h"
#include "Board.h"
#include <ostream>
#include <istream>
#include <vector>
#include <string>

////-------------------------
////		Utils
////-------------------------

using namespace std;


bool Utils::is_sink(Board gameBoard, Coordinate coor, Board origBoard, Coordinate dims)
{

	Board singleShipBoard(origBoard, coor.depth, coor.row, coor.col);

	for (int dpt = 0; dpt < singleShipBoard.depth(); dpt++) {
		for (int row = 0; row < singleShipBoard.rows(); row++) {
			for (int col = 0; col < singleShipBoard.cols(); col++) {
				if (singleShipBoard.get(dpt,row,col) != ' ') {
					if (gameBoard.get(dpt, row, col) != '@') {
						if (coor.col != col || coor.row != row || coor.depth != dpt) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}




 void Utils::gotoxy(int x, int y)
 {
	 static HANDLE h = NULL;
	 if (!h)
		 h = GetStdHandle(STD_OUTPUT_HANDLE);
	 COORD c = {(short) x, (short) y };
	 SetConsoleCursorPosition(h, c);
 }



 void Utils::GetFileNamesInDirectory(vector<string> *names, string folder)
{
	 string search_path = folder + "/*.*";
	 WIN32_FIND_DATAA  fd;
	 HANDLE hFind = FindFirstFileA(search_path.c_str(), &fd);
	 if (hFind != INVALID_HANDLE_VALUE) {
		 do {
			 // read all (real) files in current folder
			 // , delete '!' read other 2 default folder . and ..
			 if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				 (*names).push_back(folder + "\\" + fd.cFileName);
			 }
		 } while (::FindNextFileA(hFind, &fd));
		 ::FindClose(hFind);
	 }
 }


 bool Utils::string_has_suffix(const std::string &str, const std::string &suffix)
 {
	 return str.size() >= suffix.size() &&
		 str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
 }



 // This function returns true if the second pair is "less"
 // than the second one according to some metric
 // In this case, we say the first pair is "less" if the second element of the first pair
 // is less than the second element of the second pair
 bool pairCompare(const std::pair<string, double>& firstElem, const std::pair<string, double>& secondElem) {
	 return firstElem.second > secondElem.second;

 }


// Coordinate
 bool operator==(const Coordinate coor0, const Coordinate coor1)
 {
	 return coor0.col == coor1.col && coor0.row == coor1.row && coor0.depth == coor1.depth;
 }

 bool operator!=(const Coordinate coor0, const Coordinate coor1)
 {
	 return !(coor0 == coor1);
 }

 std::ostream& operator<<(std::ostream & out, const Coordinate& coor)
 {
	 out << "(" << coor.row << "," << coor.col << "," << coor.depth << ")";
	 return out;
 }

 Coordinate operator + (const Coordinate coor0, const Coordinate coor1)
 {
	 return{coor0.row + coor1.row, coor0.col + coor1.col, coor0.depth + coor1.depth};
 }


 /////////////////////////////////
 //////	Match
 /////////////////////////////////

 std::ostream& operator<<(std::ostream & out, const Match& match) {

	 out << "Player0: " << get<0>(match)->name << "\tPlayer1: " << get<1>(match)->name << endl;
	 return out;
 }

 std::ostream& operator<<(std::ostream & out, const MatchHard& match) {

	 out << "Player0: " << get<0>(match)->name << "\tPlayer1: " << get<1>(match)->name << endl;
	 return out;
 }




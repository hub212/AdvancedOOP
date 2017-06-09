#include "Utils.h"
#include "BoardChecker.h"
#include "Board.h"
#include "windows.h"
#include <memory>
#include <string>

using namespace std;

bool BoardChecker::isDebug;
vector<string> BoardChecker::boardsList;
vector<string> BoardChecker::dllsLists;
char** boards = NULL;
int num_rows = 0;
int num_cols = 0;


BoardChecker::BoardChecker()
{
}



void BoardChecker::printIllegalShapeError(string illegalShips, char ch) {

	bool contains = false;

	for (uint16_t i = 0; i < illegalShips.length(); i++) {

		if (illegalShips.at(i) == ch) {

			contains = true;
			break;
		}
	}

	if (contains) {
		if (isupper(ch)) {
			std::cout << "Wrong size or shape for ship " << ch << " for player A" << std::endl;
		}
		else {
			std::cout << "Wrong size or shape for ship " << ch << " for player B" << std::endl;
		}
	}
}

int BoardChecker::shipSize(char ch) {
	if (tolower(ch) == tolower('B')) {
		return 1;
	}
	if (tolower(ch) == tolower('P')) {
		return 2;
	}
	if (tolower(ch) == tolower('M')) {
		return 3;
	}
	if (tolower(ch) == tolower('D')) {
		return 4;
	}
	return -1;
}


bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}


int BoardChecker::checkPath(char* path, bool& isDllFound) {


	char dir[MY_MAX_PATH];
	
	vector<string> boards_list;
	vector<string> dlls_list;

	isDllFound = false;
	bool isBoardFound = false;

	std::ifstream* boardStream = 0;
	string boardName = "";
	
	strcpy_s(dir, MY_MAX_PATH, path);
	dir[MY_MAX_PATH - 1] = '\0';
	string files_dir(dir);



	if (!dirExists(files_dir)) {
		std::cout << "Wrong path: " << dir << std::endl;
		return 1;
	}
	

	// Extracting file list to dlls and boards lists
	Utils::GetFileNamesInDirectory(&boards_list, files_dir);
	dlls_list = boards_list;

	// filtering boards lists
	boards_list.erase(remove_if(boards_list.begin(), boards_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".sboard"); }), boards_list.end());
	std::sort(boards_list.begin(), boards_list.end());

	// boards list - open stream per file - check if readble
	if (static_cast<int> (boards_list.size()) > 0) {
		for (uint16_t i = 0; i < boards_list.size(); i++) {
			std::ifstream* boardStream = 0;
			boardStream = new ifstream(dlls_list[i]);
			if (!boardStream) {
				boards_list.erase(dlls_list.begin() + i);
			}
			else {
				boardStream->close();
				delete boardStream;
				boardStream = NULL;
				isBoardFound = true;
			}
		}
	}
	

	// filtering dlls list
	dlls_list.erase(remove_if(dlls_list.begin(), dlls_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".dll"); }), dlls_list.end());
	std::sort(dlls_list.begin(), dlls_list.end());

	// We need at least one boards file
	if (!isBoardFound) {
		std::cout << "Missing board file (*.sboard) looking in path: " << dir << std::endl;
	}

	// dlls list - open stream per file - check if readble
	for (uint16_t i = 0; i < dlls_list.size(); i++) {
		std::ifstream* dllStream = 0;
		dllStream = new ifstream(dlls_list[i]);
		if (!dllStream) {
			dlls_list.erase(dlls_list.begin() + i);
		}
		else {
			dllStream->close();
			delete dllStream;
			dllStream = NULL;
			isDllFound = true;
		}
	}
	

	// And at least two dll's
	if (dlls_list.size() < 2) {
		cout << "Missing an algorithm (dll) file looking in path: " << path << endl;
		isDllFound = false;
	}

	if (!isDllFound || !isBoardFound) {
		return 1;
	}

	// for exernal use in TournamentManager
	BoardChecker::boardsList = boards_list;
	BoardChecker::dllsLists = dlls_list;

	return 0;
}


bool BoardChecker::checkBoard(char* path, bool& isDllFound) {
	
	if (checkPath(path, isDllFound)) {
		if (DEBUG) {
			cout << "ERROR: checkboard failed; no board to check" << endl;
		}
		return false;
	}


	for (uint16_t i = 0; i < BoardChecker::boardsList.size(); i++) {

		std::unique_ptr<ifstream> boardStream(new ifstream(BoardChecker::boardsList[i]));
		int playerA_ships = 0;
		int playerB_ships = 0;
		string illegalShips = "";
		bool areAdjacentShips = false;
		std::unique_ptr<Board> newBoard(new Board(NUM_ROWS, NUM_COLS));
		std::unique_ptr<Board> boards(new Board(*boardStream, NUM_ROWS, NUM_COLS));
		boardStream->close();

		areAdjacentShips = boards->areAdjacentShapes();



		for (int row = 0; row < NUM_ROWS; row++) {
			for (int col = 0; col < NUM_COLS; col++) {
				char curChar = boards->get(row, col);


				if (curChar != ' ' && newBoard->get(row, col) == ' ') {

				std::unique_ptr <Board> tmpBoard(new Board(*boards, row, col));
					newBoard->add(*tmpBoard);


					if (BoardChecker::isDebug) {
						std::cout << "adding shape: " << std::endl;

						tmpBoard->printBoard();

						std::cout << "--------------------------------------------" << std::endl;

						std::cout << "shape added: " << std::endl;

						newBoard->printBoard();

						std::cout << "--------------------------------------------" << std::endl;

					}



					int minRow = tmpBoard->minOccupiedRow();
					int minCol = tmpBoard->minOccupiedCol();
					int maxRow = tmpBoard->maxOccupiedRow();
					int maxCol = tmpBoard->maxOccupiedCol();

					if (minRow == -1 || minCol == -1 || maxRow == -1 || maxCol == -1) {
						if (BoardChecker::isDebug)
							std::cout << "error, no chars in tmpBoard" << std::endl;
						continue;
					}

					if (minRow < maxRow && minCol < maxCol) {
						illegalShips.append(1, curChar);
						continue;
					}

					if (tmpBoard->countNonEmptyCells() != shipSize(curChar)) {
						illegalShips.append(1, curChar);
						continue;
					}

					if (isupper(curChar)) {
						playerA_ships++;
					}
					else {
						playerB_ships++;
					}
				}
			}
		}

		bool boardIsOk  = illegalShips.length() == 0;

		printIllegalShapeError(illegalShips, 'B');
		printIllegalShapeError(illegalShips, 'P');
		printIllegalShapeError(illegalShips, 'M');
		printIllegalShapeError(illegalShips, 'D');
		printIllegalShapeError(illegalShips, 'b');
		printIllegalShapeError(illegalShips, 'p');
		printIllegalShapeError(illegalShips, 'm');
		printIllegalShapeError(illegalShips, 'd');

		if (playerA_ships > 5) {
			std::cout << "Too many ships for player A" << std::endl;
			boardIsOk = false;
		}
		if (playerA_ships < 5) {
			std::cout << "Too few ships for player A" << std::endl;
			boardIsOk = false;
		}
		if (playerB_ships > 5) {
			std::cout << "Too many ships for player B" << std::endl;
			boardIsOk = false;
		}
		if (playerB_ships < 5) {
			std::cout << "Too few ships for player B" << std::endl;
			boardIsOk = false;
		}
		if (areAdjacentShips) {
			std::cout << "Adjacent Ships on Board" << std::endl;
			boardIsOk = false;
		}



		if (boardIsOk)
		{
			num_rows = newBoard->numRows();
			num_cols = newBoard->numCols();
			char** tmpArr = new char*[num_rows];
			for (int row_index = 0; row_index < num_rows; row_index++)
			{
				tmpArr[row_index] = new char[num_cols + 1];
				tmpArr[row_index][num_cols] = '\0';
				for (int col_index = 0; col_index < num_cols; col_index++) {
					tmpArr[row_index][col_index] = newBoard->get(row_index, col_index);
				}
			}
			this->boards.push_back(tmpArr);
		}
		else {
			BoardChecker::boardsList.erase(BoardChecker::boardsList.begin() + i);
		}
	}
		return !BoardChecker::boardsList.empty();
}

BoardChecker::~BoardChecker()
{
	if (BoardChecker::isDebug)
		std::cout << "deleting board checker" << std::endl;

	if (boards.size() != 0 && num_rows > 0 && num_cols > 0) {
		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			if (boards[row_index] != nullptr) {
				delete[] boards[row_index];
				boards[row_index] = nullptr;
			}
		}
	}
}

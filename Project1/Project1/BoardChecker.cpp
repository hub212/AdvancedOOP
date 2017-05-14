#include "Board.h"
#include "windows.h"

bool BoardChecker::isDebug;
string movesA_file;
string movesB_file;
char** board = NULL;
int num_rows = 0;
int num_cols = 0;

BoardChecker::BoardChecker()
{
}





void BoardChecker::printIllegalShapeError(string illegalShips, char ch) {

	bool contains = false;

	for (int i = 0; i < illegalShips.length(); i++) {

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

ifstream* BoardChecker::checkPath(char* path) {

	

	char dir[MY_MAX_PATH];
	strcpy_s(dir, MY_MAX_PATH, path);
	dir[MY_MAX_PATH - 1] = '\0';


	
	string dirStr(dir);
	if (!dirExists(dirStr)) {
		std::cout << "Wrong path: " << dir << std::endl;
		return NULL;
	}

	string command = "dir ";
	command.append("\"");
	command.append(dir);
	command.append("\\*.sboard\" ");

	command.append("\"");
	command.append(dir);
	command.append("\\*.attack\" ");

	command.append("\"");
	command.append(dir);
	command.append("\\*.dll\" ");

	command.append(" /b /a-d > file_names.txt  2>&1");


	TCHAR pwd[MY_MAX_PATH];
	GetCurrentDirectory(MY_MAX_PATH, pwd);
	std::wstring arr_w(pwd);
	std::string files(arr_w.begin(), arr_w.end());

	files.append("\\file_names.txt");
	system(command.c_str());
	
	ifstream files_stream(files);
	string boardName = "";
	boardName.append(dir);
	boardName.append("\\");
	std::ifstream* boardStream = 0;

	std::string dll_fileName = "";
	dll_fileName.append(dir);
	dll_fileName.append("\\");


	bool isBoardFound = false;
	bool isAttackFound = false;
	bool isDllFound = false;

	string str;
	while (!Board::safeGetline(files_stream, str).eof()) {

		if (!isBoardFound && Utils::string_has_suffix(str, ".sboard")) {
			boardName.append(str);
			isBoardFound = true;
			boardStream = new ifstream(boardName);
		}

		if (Utils::string_has_suffix(str, ".dll")) {;
			string curr_dll_filename = dll_fileName;
			curr_dll_filename.append(str);
			dllVec.push_back(curr_dll_filename);
			isDllFound = true;
		}

	}

	if (!isBoardFound || !boardStream) {
		std::cout << "Missing board file (*.sboard) looking in path: " << dir << std::endl;
	}
	
	if (isDllFound) {
		for (int i = 0; i < dllVec.size(); i++) {
			std::ifstream* dllStream = 0;
			dllStream = new ifstream(dllVec[i]);
			if (!dllStream) {
				dllVec.erase(dllVec.begin() + i);
			}
			else {
				dllStream->close();
				delete dllStream;
				dllStream = NULL;
			}
		}
	}

	if (dllVec.size() < 2) {
		cout << "Missing an algorithm (dll) file looking in path: " << path << endl;
		isDllFound = false;
	}

	if (!boardStream || !isDllFound) {
		return NULL;
	}

	return boardStream;
}

bool BoardChecker::checkBoard(char* path) {
	
	ifstream* boardStream = checkPath(path);
	if (boardStream == NULL) {
		return false;
	}



	int playerA_ships = 0;
	int playerB_ships = 0;
	string illegalShips = "";
	bool areAdjacentShips = false;
	Board* newBoard = new Board(NUM_ROWS, NUM_COLS);
	Board* board = new Board(*boardStream, NUM_ROWS, NUM_COLS);
	boardStream->close();
	delete boardStream;

	areAdjacentShips = board->areAdjacentShapes();

	Board* tmpBoard = 0;

	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLS; col++) {
			char curChar = board->get(row, col);


			if (curChar != ' ' && newBoard->get(row, col) == ' ') {

				if (tmpBoard != 0) {
					delete tmpBoard;
				}

				tmpBoard = new Board(*board, row, col);
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

	bool boardIsOk = illegalShips.length() == 0;

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
		this->board = tmpArr;
	}

	delete newBoard;
	delete tmpBoard;
	delete board;

	return boardIsOk;
}

BoardChecker::~BoardChecker()
{
	if (BoardChecker::isDebug)
		std::cout << "deleting board checker" << std::endl;

	if (board != nullptr && num_rows > 0 && num_cols > 0) {
		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			if (board[row_index] != nullptr) {
				delete[] board[row_index];
				board[row_index] = nullptr;
			}
		}
		delete[] board;
		board = nullptr;
	}
}

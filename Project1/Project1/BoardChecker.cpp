#include "BoardChecker.h"

bool BoardChecker::isDebug;

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

bool BoardChecker::string_has_suffix(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

ifstream* BoardChecker::checkPath(char* path) {
	char dir[MY_MAX_PATH];
	strcpy(dir, path);
	dir[MY_MAX_PATH - 1] = '\0';

	struct stat info;
	if (stat(dir, &info) != 0 || !(info.st_mode & S_IFDIR)) {
		std::cout << "Wrong path: " << dir << std::endl;
		return NULL;
	}

	string command = "dir ";
	command.append(dir);
	command.append("\\*.sboard ");

	command.append(dir);
	command.append("\\*.attack-a ");

	command.append(dir);
	command.append("\\*.attack-b ");

	command.append(" /b /a-d > file_names.txt  2>&1");


	TCHAR pwd[MY_MAX_PATH];
	GetCurrentDirectory(MY_MAX_PATH, pwd);
	string files = pwd;
	files.append("\\file_names.txt");
	system(command.c_str());
	
	ifstream files_stream(files);
	string boardName = "";
	boardName.append(dir);
	boardName.append("\\");
	std::ifstream* boardStream = 0;

	std::string playerA_fileName = "";
	playerA_fileName.append(dir);
	playerA_fileName.append("\\");
	std::ifstream* playerA_Stream = 0;

	std::string playerB_fileName = "";
	playerB_fileName.append(dir);
	playerB_fileName.append("\\");
	std::ifstream* playerB_Stream = 0;

	bool isBoardFound = false;
	bool isPlayerA_fileFound = false;
	bool isPlayerB_fileFound = false;

	string str;
	while (!Board::safeGetline(files_stream, str).eof()) {

		if (!isBoardFound && string_has_suffix(str, ".sboard")) {
			boardName.append(str);
			isBoardFound = true;
			boardStream = new ifstream(boardName);
		}

		if (!isPlayerA_fileFound && string_has_suffix(str, ".attack-a")) {
			playerA_fileName.append(str);
			isPlayerA_fileFound = true;
			playerA_Stream = new ifstream(playerA_fileName);
		}

		if (!isPlayerB_fileFound && string_has_suffix(str, ".attack-b")) {
			playerB_fileName.append(str);
			isPlayerB_fileFound = true;
			playerB_Stream = new ifstream(playerB_fileName);
		}
	}

	if (!isBoardFound || !boardStream) {
		std::cout << "Missing board file (*.sboard) looking in path: " << dir << std::endl;
	}

	if (!isPlayerA_fileFound || !playerA_Stream) {
		std::cout << "Missing attack file for player A (*.attack-a) looking in path: " << dir << std::endl;
	}
	if (playerA_Stream) {
		playerA_Stream->close();
		delete playerA_Stream;
		playerA_Stream = NULL;
	}
	else {
		isPlayerA_fileFound = false;
	}

	if (!isPlayerB_fileFound || !playerB_Stream) {
		std::cout << "Missing attack file for player B (*.attack-b) looking in path: " << dir << std::endl;
	}
	if (playerB_Stream) {
		playerB_Stream->close();
		delete playerB_Stream;
		playerB_Stream = NULL;
	}
	else {
		isPlayerB_fileFound = false;
	}

	if (!boardStream || !isPlayerA_fileFound || !isPlayerB_fileFound) {
		return NULL;
	}

	return boardStream;
}

void BoardChecker::checkBoard(char* path) {
	
	ifstream* boardStream = checkPath(path);
	if (boardStream == NULL) {
		return;
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
	}
	if (playerA_ships < 5) {
		std::cout << "Too few ships for player A" << std::endl;
	}
	if (playerB_ships > 5) {
		std::cout << "Too many ships for player B" << std::endl;
	}
	if (playerB_ships < 5) {
		std::cout << "Too few ships for player B" << std::endl;
	}
	if (areAdjacentShips) {
		std::cout << "Adjacent Ships on Board" << std::endl;
	}

	
	delete newBoard;
	delete tmpBoard;




	delete board;
}

BoardChecker::~BoardChecker()
{
}

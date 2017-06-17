#include "BoardChecker.h"
#include "windows.h"



BoardChecker::BoardChecker()
{
}


bool BoardChecker::isDebug=false;
vector<string> BoardChecker::dllVec = {};
vector<std::shared_ptr<Board>> BoardChecker::boardVec = {};


void BoardChecker::printIllegalShapeError(string illegalShips, char ch) {

	bool contains = false;

	for (size_t i = 0; i < illegalShips.length(); i++) {

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




bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}



std::shared_ptr<Board> BoardChecker::checkBoard(string boardPath) {


	std::cout << "Checking board: " << boardPath << std::endl;

	std::ifstream boardStream = ifstream(boardPath);

	int player0_ships = 0;
	int player1_ships = 0;

	int blayer0_shipSizes[4] = { 0 };
	int blayer1_shipSizes[4] = { 0 };

	string illegalShips = "";
	bool areAdjacentShips = false;

	std::shared_ptr<Board> board = std::make_shared<Board>(boardStream);

	Board newBoard = Board(board->depth(), board->rows(), board->cols());
	
	boardStream.close();

	areAdjacentShips = board->areAdjacentShapes();


	Board tmpBoard = Board(board->depth(), board->rows(), board->cols());

	for (int dpth = 0; dpth < board->depth(); dpth++) {
		for (int row = 0; row < board->rows(); row++) {
			for (int col = 0; col < board->cols(); col++) {

				char curChar = board->get(dpth, row, col);


				if (curChar != ' ' && newBoard.get(dpth, row, col) == ' ') {



					tmpBoard = Board(*board, dpth, row, col);
					newBoard.add(tmpBoard);


					if (isDebug) {
						std::cout << "adding shape: " << std::endl;

						tmpBoard.printBoard();

						std::cout << "--------------------------------------------" << std::endl;

						std::cout << "shape added: " << std::endl;

						newBoard.printBoard();

						std::cout << "--------------------------------------------" << std::endl;

					}



					int minRow = tmpBoard.minOccupiedRow();
					int minCol = tmpBoard.minOccupiedCol();
					int minDpt = tmpBoard.minOccupiedDpth();
					int maxRow = tmpBoard.maxOccupiedRow();
					int maxCol = tmpBoard.maxOccupiedCol();
					int maxDpt = tmpBoard.maxOccupiedDpth();

					if (minRow == -1 || minCol == -1 || maxRow == -1 || maxCol == -1 || minDpt == -1 || maxDpt == -1) {
						if (isDebug)
							std::cout << "error, no chars in tmpBoard" << std::endl;
						continue;
					}

					if ((minRow < maxRow && minCol < maxCol) || (minRow < maxRow && minDpt < maxDpt) || (minCol < maxCol && minDpt < maxDpt)) {

						std::cout << "minRow: " << minRow << " maxRow: " << maxRow << " minCol: " << minCol << " maxCol: " << maxCol << " minDpt: " << minDpt << " maxDpt: " << maxDpt <<  std::endl;

						illegalShips.append(1, curChar);
						continue;
					}

					if (tmpBoard.countNonEmptyCells() != Board::shipSize(curChar)) {

						if (isDebug)
							std::cout << "Illegal shape was added: " << curChar <<  " of size: " << Board::shipSize(curChar) <<  std::endl;

						illegalShips.append(1, curChar);
						continue;
					}

					if (isupper(curChar)) {
						blayer0_shipSizes[Board::shipSize(curChar) - 1]++;
						player0_ships++;
					}
					else {
						blayer1_shipSizes[Board::shipSize(curChar) - 1]++;
						player1_ships++;
					}
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

	if (player0_ships != player1_ships || 
		blayer0_shipSizes[0] != blayer1_shipSizes[0] || 
		blayer0_shipSizes[1] != blayer1_shipSizes[1] || 
		blayer0_shipSizes[2] != blayer1_shipSizes[2] ||
		blayer0_shipSizes[3] != blayer1_shipSizes[3])
	{	
		std::cout << "Imbalanced board" << std::endl;
	}
	else {
		std::cout << "Balanced board" << std::endl;
	}
	if (player0_ships == 0) {
		std::cout << "Player A has no ships" << std::endl;
	}
	if (player1_ships == 0) {
		std::cout << "Player B has no ships" << std::endl;
	}
	if (areAdjacentShips) {
		std::cout << "Adjacent Ships on Board" << std::endl;
		boardIsOk = false;
	}


	if (boardIsOk) {
		std::cout << "Legal board" << std::endl;
		return board;
	}
	else {
		std::cout << "Illegal board" << std::endl;
		return nullptr;
	}
}


bool BoardChecker::checkPath(char* path, bool& isDllFound) {

	char dir[MY_MAX_PATH];
	strcpy_s(dir, MY_MAX_PATH, path);
	dir[MY_MAX_PATH - 1] = '\0';
	// dlls handling	
	vector<string> boards_list;
	vector<string> dlls_list;

	isDllFound = false;
	bool isBoardFound = false;

	string boardName = "";
	string files_dir(dir);


	if (!dirExists(files_dir)) {
		std::cout << "Wrong path: " << dir << std::endl;
		return false;
	}
	

	// builds moves vector list
	Utils::GetFileNamesInDirectory(&boards_list, files_dir);
	dlls_list = boards_list;

	boards_list.erase(remove_if(boards_list.begin(), boards_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".sboard"); }), boards_list.end());

	std::sort(boards_list.begin(), boards_list.end());

	for (int i = 0; i < static_cast<int> (boards_list.size()); ++i) {
		isBoardFound = true;
		std::shared_ptr<Board> brd = checkBoard(boards_list[i]);

		if (brd != nullptr) {
			boardVec.push_back(brd);
		}
	}


	dlls_list.erase(remove_if(dlls_list.begin(), dlls_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".dll"); }), dlls_list.end());

	std::sort(dlls_list.begin(), dlls_list.end());

	for (int i = 0; i < static_cast<int> (dlls_list.size()); ++i) {
		isDllFound = true;
		dllVec.push_back(dlls_list[i]);
	}

	if (!isBoardFound) {
		std::cout << "Missing board file (*.sboard) looking in path: " << dir << std::endl;
	}

	if (static_cast<int> (boardVec.size()) < 1) {
		std::cout << "No legal boards found in dir: " << dir << std::endl;
		isBoardFound = false;
	}

	if (isDllFound) {
		for (size_t i = 0; i < dllVec.size(); i++) {
			std::ifstream dllStream = ifstream(dllVec[i]);
			if (!dllStream) {
				dllVec.erase(dllVec.begin() + i);
			}
			else {
				dllStream.close();
			}
		}
	}

	if (dllVec.size() < 2) {
		cout << "Missing an algorithm (dll) file looking in path: " << path << " (needs at least two)" << endl;
		isDllFound = false;
	}

	if (!isDllFound || !isBoardFound) {
		return false;
	}

	return true;
}


bool BoardChecker::checkBoard(char* path, bool& isDllFound) {
	
	return checkPath(path, isDllFound);
}

BoardChecker::~BoardChecker()
{
	if (isDebug)
		std::cout << "deleting board checker" << std::endl;
}

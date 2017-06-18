#include "BoardChecker.h"
#include "windows.h"

std::ofstream BoardChecker::log;

string BoardChecker::getTime() {
	// current date/time based on current system
	time_t now = time(0);

	char dt[1000];

	// convert now to string form
	ctime_s(dt, 1000, &now);

	string date = dt;

	for (uint16_t i = 0; i < date.length(); i++) {

		if (date[i] == '\n') {
			date[i] = ' ';
		}
	}

	return date;
}

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
			log << getTime() <<": Wrong size or shape for ship " << ch << " for player A" << std::endl;
		}
		else {
			log << getTime() << ": Wrong size or shape for ship " << ch << " for player B" << std::endl;
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


	log << getTime() << ": Checking board: " << boardPath << std::endl;

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


					int minRow = tmpBoard.minOccupiedRow();
					int minCol = tmpBoard.minOccupiedCol();
					int minDpt = tmpBoard.minOccupiedDpth();
					int maxRow = tmpBoard.maxOccupiedRow();
					int maxCol = tmpBoard.maxOccupiedCol();
					int maxDpt = tmpBoard.maxOccupiedDpth();

					if (minRow == -1 || minCol == -1 || maxRow == -1 || maxCol == -1 || minDpt == -1 || maxDpt == -1) {
						continue;
					}

					if ((minRow < maxRow && minCol < maxCol) || (minRow < maxRow && minDpt < maxDpt) || (minCol < maxCol && minDpt < maxDpt)) {

						illegalShips.append(1, curChar);
						continue;
					}

					if (tmpBoard.countNonEmptyCells() != Board::shipSize(curChar)) {


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
		log << getTime() << ": Imbalanced board" << std::endl;
	}
	else {
		log << getTime() << ": Balanced board" << std::endl;
	}
	if (player0_ships == 0) {
		log << getTime() << ": Player A has no ships" << std::endl;
	}
	if (player1_ships == 0) {
		log << getTime() << ": Player B has no ships" << std::endl;
	}
	if (areAdjacentShips) {
		log << getTime() << ": Adjacent Ships on Board" << std::endl;
		boardIsOk = false;
	}


	if (boardIsOk) {
		log << getTime() << ": Legal board" << std::endl;
		return board;
	}
	else {
		log << getTime() << ": Illegal board" << std::endl;
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


	char logPath[1000];
	strcpy_s(logPath, 1000, dir);
	strcat_s(logPath, 1000, "\\game.log\0");
	log.open(logPath, std::ios_base::app | std::ios_base::out);

	log << getTime() << ": program execution begin" << std::endl;

	// builds moves vector list
	Utils::GetFileNamesInDirectory(&boards_list, files_dir);
	dlls_list = boards_list;

	boards_list.erase(remove_if(boards_list.begin(), boards_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".sboard"); }), boards_list.end());

	std::sort(boards_list.begin(), boards_list.end());

	for (int i = 0; i < static_cast<int> (boards_list.size()); ++i) {
		
		std::shared_ptr<Board> brd = checkBoard(boards_list[i]);

		if (brd != nullptr) {
			//brd->printBoard();
			boardVec.push_back(brd);
			isBoardFound = true;
		}
	}


	dlls_list.erase(remove_if(dlls_list.begin(), dlls_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".dll"); }), dlls_list.end());

	std::sort(dlls_list.begin(), dlls_list.end());

	for (int i = 0; i < static_cast<int> (dlls_list.size()); ++i) {
		isDllFound = true;
		dllVec.push_back(dlls_list[i]);
		log << getTime() << ": dll found: " << dlls_list[i] << std::endl;
	}

	if (!isBoardFound) {
		std::cout << "No board files (*.sboard) looking in path: " << dir << std::endl;
		log << getTime() << ": No board files (*.sboard) looking in path: " << dir << std::endl;
	}

	if (isDllFound) {
		for (size_t i = 0; i < dllVec.size(); i++) {
			std::ifstream dllStream = ifstream(dllVec[i]);
			if (!dllStream) {
				dllVec.erase(dllVec.begin() + i);
				log << getTime() << ": dll could not be opened: " << dlls_list[i] << std::endl;
				continue;
			}
			else {
				dllStream.close();
			}
			string AlgoName = dllVec[i].substr(dllVec[i].find_last_of("\\") == string::npos ? 0 : dllVec[i].find_last_of("\\") + 1, dllVec[i].find_first_of('.'));
			HINSTANCE hDll = LoadLibraryA(dllVec[i].c_str());
			bool dllOk = true;
			if (!hDll) {
				dllVec.erase(dllVec.begin() + i);
				log << getTime() << ": dll could not be opened: " << dlls_list[i] << std::endl;
			}
		}
	}

	if (dllVec.size() < 2) {
		cout << "Missing algorithm (dll) files looking in path: " << path << " (needs at least two)" << endl;
		log << getTime() << ": Missing algorithm (dll) files looking in path: " << path << " (needs at least two)" << endl;
		isDllFound = false;
	}

	if (!isDllFound || !isBoardFound) {
		return false;
	}

	std::cout << "Number of legal players: " << dllVec.size() << std::endl;
	std::cout << "Number of legal boards: " << boardVec.size() << std::endl;

	log << getTime() << ": Number of legal players: " << dllVec.size() << std::endl;
	log << getTime() << ": Number of legal boards: " << boardVec.size() << std::endl;

	return true;
}


bool BoardChecker::checkBoard(char* path, bool& isDllFound) {
	
	return checkPath(path, isDllFound);
}

BoardChecker::~BoardChecker()
{
}

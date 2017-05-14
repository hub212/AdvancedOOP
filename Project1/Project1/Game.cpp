#include "Game.h"
#include <sstream>
#include <set>
#include <vector>
#include <tuple>

using namespace std;

////--------------------------
////		GameMaster
////--------------------------


GameMaster::GameMaster(char** boards, const char* players_moves, int numRows, int numCols, int delay, int quiet, char **boardCopy) :
	boards(boards), players_moves(players_moves), rows(numRows), cols(numCols),delay(delay), quiet(quiet), turn(Players::PlayerA)
{
	this->boardCopy = boardCopy;
	setBoards(const_cast<const char**>(boards), rows, cols);
	player0->init(players_moves);
	player1->init(players_moves);
	scores[0] = 0;
	scores[1] = 0;
	Utils::ShowConsoleCursor(0);
}

GameMaster::GameMaster(char ** boards, const char * players_moves, int numRows, int numCols, int delay, int quiet, vector<tuple<string, HINSTANCE, GetAlgoType>> dll_vec, char **boardCopy) :
	
	boards(boards), players_moves(players_moves), rows(numRows), cols(numCols), delay(delay), quiet(quiet), turn(Players::PlayerA)
{
	this->boardCopy = boardCopy;
	dll_vec = dll_vec;
	player0 = get<2>(dll_vec[0])();
	player1 = get<2>(dll_vec[1])();
	setBoards(const_cast<const char**>(boards), rows, cols);
	player0->init(players_moves);
	player1->init(players_moves);
	scores[0] = 0;
	scores[1] = 0;
	Utils::ShowConsoleCursor(0);
}


int GameMaster::extractBoards(const char** board, int numRows, int numCols, char**** out_board)
{
	try {
		*out_board = new char**[2];
	}
	catch(std::bad_alloc& exc)
	{
		cout << "Error: double string array allocation failed while allocating multi board;						" << exc.what() << endl;
		return 1;
	}

	if (Utils::copyBoard(board, numRows, numCols, &(*out_board)[0]) == -1){
		delete[] * out_board;
		cout << "Error: string array allocation failed for player A while allocating board;						" << endl;
		return 1;
	}

	if (Utils::copyBoard(board, numRows, numCols, &(*out_board)[1]) == -1) {
		delete[] * out_board;
		cout << "Error: string array allocation failed for player B while allocating board;						"  << endl;
		return 1;
	}
	
	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++) {
			if (lettersA.find(board[row][col]) == lettersA.end())
			{
				(*out_board)[0][row][col] =  ' ';
			}
			if (lettersB.find(board[row][col]) == lettersB.end())
			{
				(*out_board)[1][row][col] = ' ';
			}
		}
	}

	return 0;
}

void GameMaster::setBoards(const char** board, int numRows, int numCols)
{
	char*** boards = nullptr;

	if (extractBoards(board, numRows, numCols, &boards) != 0)
	{
		cout << "Error: setBoards failed due to player boards allocations					" << endl;
	}
	else {
		player0->setBoard(0,const_cast<const char**>(boards[0]), numRows, numCols);
		player1->setBoard(1,const_cast<const char**>(boards[1]), numRows, numCols);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++)
				delete[] boards[i][j];
			delete[] boards[i];
		}
		delete[] boards;
	}
}


pair<int, int> GameMaster::attack()
{
	pair<int, int> curr_move;

	switch (turn)
	{
	case(Players::PlayerA):
		curr_move = player0->attack();
		break;

	case(Players::PlayerB):
		curr_move = player1->attack();
		break;
	default:
		return make_pair(-2, -2);
	}

	if (curr_move == make_pair(-2, -2))
	{
		cout << "Error: attack() failed on " << ("%s", Players::PlayerA == turn ? "player A's " : "player B's ") << "turn					" << endl;
		return make_pair(-2, -2);
	} else if (curr_move != make_pair(-2, -2) && DEBUG)
		cout << ("%s", Players::PlayerA == turn ? "player A move:\t" : "player B move:\t") << "(" << curr_move.first+1 << "," << curr_move.second+1 << ")					" << endl;
	return curr_move;
}



int GameMaster::play()
{
	// Player A starts
	turn = Players::PlayerA;

	pair<int, int> move;
	pair<int, int> prevMove = make_pair(-3, -3);
	int player0_done = 0;
	int player1_done = 0;

	print_board(-1, -1, delay);

	while(!player0_done || !player1_done)
	{

		 prevMove = move;
		 move = attack();

		 if (move == make_pair(-2, -2))
		 {
			 // failure 
			 return -1;
		 }

		if (move == make_pair(-1, -1))
		{
			// the player has no more moves
			if (turn == Players::PlayerA) {
				turn = Players::PlayerB;
				player0_done = 1;
			}
			else {
				turn = Players::PlayerA;
				player1_done = 1;
			}
			continue;
		}
		pair<Vessel_ID, AttackResult> results = attack_results(move);
		int activePlayerIndex = turn == Players::PlayerA ? 0 : 1;

		update_state(move, results);

		print_board(move.first, move.second, delay);

		player0->notifyOnAttackResult(activePlayerIndex, move.first, move.second, results.second);
		player1->notifyOnAttackResult(activePlayerIndex, move.first, move.second, results.second);

		if (DEBUG && results.second != AttackResult::Miss)
				cout << ("%s ", turn == Players::PlayerA ? "player A move results " : "player B move results ") << ("%s", results.second == AttackResult::Hit ? "Hit " : "Sink ") << ("%s ", results.first.player == Players::PlayerA ? "player A's " : "player B's ") << ("%s", results.first.type == VesselType::Boat ? "Boat				" : results.first.type == VesselType::Missiles ? "Missiles			" : results.first.type == VesselType::Sub ? "Sub				" : "War				") << endl << "Score " << scores[0] << ":" << scores[1] << "					" << endl;
			
		if (is_defeat())
		{
			break;
		}

		if (results.second != AttackResult::Miss)
		{
			turn = (turn == Players::PlayerA) ? Players::PlayerA : Players::PlayerB;
		}
		else {
			turn = (turn == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
		}

		
	}

	print_results();

	return 0;
}


pair<Vessel_ID, AttackResult> GameMaster::attack_results(pair<int,int> move)
{
	int x = move.first - 1;
	int y = move.second - 1;
	char curr = boards[x][y];
	Vessel_ID vessel;

	if (curr == '@' || curr == ' ')
	{
		return make_pair(Vessel_ID::Vessel_ID(), AttackResult::Miss);
	}

	vessel = GameMaster::get_vessel(curr, player0, player1);


	Board *copyOfOriginalBoard = new Board(boardCopy, rows, cols);
	Board *onlyOriginalShipBoard = new Board(*copyOfOriginalBoard, x, y);

	bool isSink = Utils::is_sink(boards, x, y, curr, onlyOriginalShipBoard->getboard(), rows, cols);

	delete onlyOriginalShipBoard;
	delete copyOfOriginalBoard;

	if (isSink)
	{
		return make_pair(vessel, AttackResult::Sink);
	} else
	{
		return make_pair(vessel, AttackResult::Hit);
	}
}

 void GameMaster::update_state(pair<int,int> move, pair<Vessel_ID, AttackResult> results)
{
	 boards[move.first - 1][move.second - 1] = '@';
	if (results.second == AttackResult::Sink)
		scores[(int) (results.first.player == Players::PlayerA ? Players::PlayerB : Players::PlayerA)] += results.first.score;
}

 bool GameMaster::is_defeat()
{
	 bool boolA = false;
	 bool boolB = false;
	 for(int i = 0; i < rows ; i++)
	 {
		 for(int j = 0; j < cols && (!boolA || !boolB); j++)
		 {
			 if(lettersA.find(boards[i][j]) != lettersA.end())
			 {
				 boolA = true;
			 }

			 if (lettersB.find(boards[i][j]) != lettersB.end())
			 {
				 boolB = true;
			 }
		 }
	 }

	 return boolA^boolB;
}


 Vessel_ID GameMaster::get_vessel(char curr, IBattleshipGameAlgo* playerA, IBattleshipGameAlgo* playerB)
 {
	 Vessel_ID vessel;

	 if (lettersA.find(curr) != lettersA.end())
	 {
		 if (curr == 'B')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::Boat, Players::PlayerA);
		 }
		 else if (curr == 'P')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::Missiles, Players::PlayerA);
		 }
		 else if (curr == 'M')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::Sub, Players::PlayerA);
		 }
		 else if (curr == 'D')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::War, Players::PlayerA);
		 }
	 }
	 else if (lettersB.find(curr) != lettersB.end())
	 {
		 // it's Players B vessel
		 if (curr == 'b')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::Boat, Players::PlayerB);
		 }
		 else if (curr == 'p')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::Missiles, Players::PlayerB);
		 }
		 else if (curr == 'm')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::Sub, Players::PlayerB);
		 }
		 else if (curr == 'd')
		 {
			 vessel = Vessel_ID::Vessel_ID(VesselType::War, Players::PlayerB);
		 }
	 }

	 return vessel;
 }


 void GameMaster::print_results()
{
	if (scores[0] != scores[1])
		cout << "Player " << ("%s", scores[0] > scores[1] ? "A " : "B ") << "won					"<< endl;
	
	cout << "Points:					" << endl;
	cout << "Player A: " << ("%d", scores[0]) << "					"<< endl;
	cout << "Player B: " << ("%d", scores[1]) << "					"<< endl;

}

 void GameMaster::print_board(int x,int y,int delay)
 {
	if (!quiet) {

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
		for (int i = 0; i< rows; i++) {
			for (int j = 0; j < cols; j++) {
				Utils::gotoxy(i, j);
				if (x-1 == i && y-1 == j) {
					SetConsoleTextAttribute(hConsole, 14);
					cout << '*';
				}
				else {
					if (boards[i][j] == '@') {
						SetConsoleTextAttribute(hConsole, 8);
					}
					else {
						SetConsoleTextAttribute(hConsole, (boards[i][j] > 96 ? ((boards[i][j] % 8) + 2) : (boards[i][j] % 5) + 9));
					}
					cout << boards[i][j];
				}
			}
		}

		if (x-1>=0 && y-1>=0){
			 Sleep(delay);
			 Utils::gotoxy(x-1, y-1);
			 if (boards[x-1][y-1] == '@') {
				 SetConsoleTextAttribute(hConsole, 8);
			 }
			 else {
				 SetConsoleTextAttribute(hConsole, (boards[x - 1][y - 1] > 96 ? ((boards[x - 1][y - 1] % 8) + 2) : (boards[x - 1][y - 1] % 5) + 9));
			 }
				cout << boards[x-1][y-1];
		}
		Utils::gotoxy(rows, cols);
		cout << endl;
		Sleep(delay);
		SetConsoleTextAttribute(hConsole, 7);
	}
 }



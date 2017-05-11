#include "IBattleshipGameAlgo.h"
#include "Game.h"
#include <sstream>
#include <set>
#include <vector>

using namespace std;

////--------------------------
////		CommonAlgo
////--------------------------

// copy the relevant vessels
void CommonAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->dim = make_pair(numRows, numCols);
	this->player_num = player;
}

bool CommonAlgo::init(const std::string& path) {
	return true;
};


void CommonAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}

std::pair<int, int> CommonAlgo::attack()
{
	return make_pair(0, 0);
}

// decising letters for each player
CommonAlgo::CommonAlgo(int player_num, char* letters,const char* moves) : moves(moves), player_num(player_num), read_pos(0), line_num(0), done(0), myLetters(letters, letters + strlen(letters)) {};



CommonAlgo::~CommonAlgo() 
{
	if (board != nullptr) {
		delete[] board;
		board = nullptr;
	}
	myLetters.clear();
}



////--------------------------
////		GameMaster
////--------------------------


GameMaster::GameMaster(char** boards, vector<const char*>& players_moves, int numRows, int numCols) : playerA(0, "BPMD", players_moves[0]),
playerB(1, "bpmd", players_moves[1]), boards(boards), players_moves(players_moves), dim(make_pair(numRows, numCols))
{
	setBoards(const_cast<const char**>(boards), dim.first, dim.second);
	scores[0] = 0;
	scores[1] = 0;
	turn = Players::PlayerA;
};


int GameMaster::extractBoards(const char** board, int numRows, int numCols, char**** out_board)
{
	try {
		*out_board = new char**[2];
	}
	catch(std::bad_alloc& exc)
	{
		cout << "Error: double string array allocation failed while allocating multi board; " << exc.what() << endl;
		return 1;
	}

	if (Utils::copyBoard(board, numRows, numCols, &(*out_board)[0]) == -1){
		delete[] * out_board;
		cout << "Error: string array allocation failed for player A while allocating board; " << endl;
		return 1;
	}

	if (Utils::copyBoard(board, numRows, numCols, &(*out_board)[1]) == -1) {
		delete[] * out_board;
		cout << "Error: string array allocation failed for player B while allocating board; "  << endl;
		return 1;
	}
	
	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++) {
			if (this->playerA.myLetters.find(board[row][col]) == this->playerA.myLetters.end())
			{
				(*out_board)[0][row][col] =  ' ';
			}
			if (this->playerB.myLetters.find(board[row][col]) == this->playerB.myLetters.end())
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
		cout << "Error: setBoards failed due to player boards allocations" << endl;
	}
	else {
		playerA.setBoard(1,const_cast<const char**>(boards[0]), numRows, numCols);
		playerB.setBoard(2,const_cast<const char**>(boards[1]), numRows, numCols);
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
		curr_move = playerA.attack();
		break;

	case(Players::PlayerB):
		curr_move = playerB.attack();
		break;
	default:
		return make_pair(0, 0);
	}

	if (curr_move == make_pair(0, 0))
	{
		cout << "Error: attack() failed on " << ("%s", Players::PlayerA == turn ? "player A" : "player B") << "turn" << endl;
		return make_pair(0, 0);
	} else if (curr_move != make_pair(0, 0) && DEBUG)
		cout << ("%s", Players::PlayerA == turn ? "player A move:\t" : "player B move:\t") << "(" << curr_move.first+1 << "," << curr_move.second+1 << ")" << endl;
	return curr_move;
}


int GameMaster::play()
{
	// Player A starts
	turn = Players::PlayerA;

	pair<int, int> move;

	while(!playerA.done || !playerB.done)
	{
		 move = attack();

		 if (move == make_pair(0, 0))
		 {
			 // failure 
			 return -1;
		 }

		if (move == make_pair(-1, -1))
		{
			// the player has no more moves
			turn = (turn == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
			continue;
		}
		pair<Vessel_ID, AttackResult> results = attack_results(move);

		if (results.second != AttackResult::Miss)
		{
			update_state(move, results);
			if (DEBUG)
				cout << ("%s ", turn == Players::PlayerA ? "player A move results " : "player B move results") <<  ("%s", results.second == AttackResult::Hit ? "Hit " : "Sink ") << ("%s ", results.first.player == Players::PlayerA ? "player A's " : "player B's ") << ("%s", results.first.type == VesselType::Boat ? "Boat " : results.first.type == VesselType::Missiles ? "Missiles " : results.first.type == VesselType::Sub ? "Sub " : "War ") << endl << "Score " << scores[0] << ":" << scores[1] << endl;

			if (is_defeat())
			{
				break;
			}
			turn = (results.first.player == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
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

	vessel = Utils::get_vessel(curr, playerA, playerB);

	if (Utils::is_sink(boards ,x,y, curr))
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
	 for(int i = 0; i < dim.first ; i++)
	 {
		 for(int j = 0; j < dim.second && (!boolA || !boolB); j++)
		 {
			 if(playerA.myLetters.find(boards[i][j]) != playerA.myLetters.end())
			 {
				 boolA = true;
			 }

			 if (playerB.myLetters.find(boards[i][j]) != playerB.myLetters.end())
			 {
				 boolB = true;
			 }
		 }
	 }

	 return boolA^boolB;
}

 void GameMaster::print_results()
{
	if (scores[0] != scores[1])
		cout << "Player " << ("%s", scores[0] > scores[1] ? "A " : "B ") << "won"<< endl;
	
	cout << "Points:" << endl;
	cout << "Player A: " << ("%d", scores[0]) << endl;
	cout << "Player B: " << ("%d", scores[1]) << endl;

}



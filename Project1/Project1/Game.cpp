#include "stdafx.h"
#include "IBattleshipGameAlgo.h"
#include "Game.h"
#include <set>
#include <vector>

using namespace std;

////--------------------------
////		Vessels
////--------------------------

Vessel_ID::Vessel_ID(VesselType type, Players player) : type(type), player(player)
{
	switch(type)
	{
	case VesselType::Boat:
		score = (int) Scores::BScore;
		break;

	case VesselType::Missiles:
		score = (int)Scores::MScore;
		break;

	case VesselType::Sub:
		score = (int)Scores::SScore;
		break;

	case VesselType::War:
		score = (int)Scores::WScore;
		break;
	}
};

Vessel_ID::Vessel_ID() {};




////--------------------------
////		Player
////--------------------------

// copy the relevant vessels
void Player::setMyBoard(string* board, int numRows, int numCols)
{
	this->dim = make_pair(numRows, numCols);
}



std::pair<int, int> Player::next_move() {

	string line;
	string col;
	string row;
	int	pos;
	int col_int;
	int row_int;
	int curr_line = 0;

	// FIXME - not opennig with given path argument - replace the string with (this->moves).c_str() for checking
	ifstream inputFile = ifstream(moves);
	if (!inputFile.is_open()) {
		cout << "Error: unable to open player " << player_num << "move files from path:\n" << (this->moves).c_str() << endl <<  strerror((errno)) << endl;
		return make_pair(-1,-1);
	}

		
	while (getline(inputFile, line) ) {
		if (curr_line < read_pos) {
			curr_line++;
			continue;
		}
		read_pos += 1;
		if (line.size() && line[line.size() - 1] == '\r') {
			line = line.substr(0, line.size() - 1);
		}
			
		pos = line.find(",");
		if  (pos == (unsigned long long) (line.length() - 1)) continue;

		row = line.substr(0, pos);
		col = line.substr(pos + 1,line.length());
			
		if ((str2int(row, &row_int) != 0) || (str2int(col, &col_int)) != 0) continue;
			
		if (row_int > dim.first || col_int > dim.second) continue;

		inputFile.close();
		return make_pair(row_int - 1, col_int - 1);
	}
	done = 1;
	return make_pair(0, 0);
}


int Player::str2int(const string str, int* num)
{

	try
	{
		*num = stoi(str);
	}
	catch (invalid_argument&)
	{
		cout << "Error(Debug only): none number input; "<< str << endl;
		return -1;
	}
	catch (out_of_range&)
	{
		cout << "Error(Debug only): input number is out of range; " << str << endl;
		return -1;
	}
	return 0;
}

// decising letters for each player
Player::Player(int player_num, char* letters,const char* moves) : moves(moves), player_num(player_num), read_pos(0), line_num(0), done(0), myLetters(letters, letters + strlen(letters)) {};



Player::~Player() 
{
	if (board != nullptr)
		delete[] board;
	myLetters.clear();
}



////--------------------------
////		GameMaster
////--------------------------


GameMaster::GameMaster(char** boards, vector<const char*>& players_moves, int numRows, int numCols) : playerA(0, "BPMD", players_moves[0]),
playerB(1, "bpmd", players_moves[1]), boards(boards), players_moves(players_moves), dim(make_pair(numRows, numCols))
{
	setBoard(const_cast<const char**>(boards), dim.first, dim.second);
	scores[0] = 0;
	scores[1] = 0;
	turn = Players::PlayerA;
};


int GameMaster::extractBoards(const char** board, int numRows, int numCols, string** out_board[])
{
	try {
		*out_board = new string*[2];
	}
	catch(std::bad_alloc& exc)
	{
		cout << "Error: double string array allocation failed" << endl;
		return 1;
	}

	try {
		(*out_board)[0] = new string[numRows];
	}
	catch (std::bad_alloc& exc)
	{
		cout << "Error: string array allocation failed for player A" << endl;
		return 1;
	}

	try {
		(*out_board)[1] = new string[numRows];
	}
	catch (std::bad_alloc& exc)
	{
		cout << "Error: string array allocation failed for player B" << endl;
		return 1;
	}

	
	for (int row = 0; row < numRows; row++)
	{
		(*out_board)[0][row] = string(board[row]);
		(*out_board)[1][row] = string(board[row]);

		for (int col = 0; col < numCols; col++) {
			if (this->playerA.myLetters.find(board[row][col]) == this->playerA.myLetters.end())
			{
				(*out_board)[0][row].replace(col, 1, 1, ' ');
			}
			if (this->playerB.myLetters.find(board[row][col]) == this->playerB.myLetters.end())
			{
				(*out_board)[1][row].replace(col, 1, 1, ' ');
			}
		}
	}

	return 0;
}

void GameMaster::setBoard(const char** board, int numRows, int numCols)
{
	string** boards = nullptr;

	if (extractBoards(board, numRows, numCols, &boards) != 0)
	{
		cout << "Error: setBoard failed due to player boards allocations" << endl;
	}
	else {
		playerA.setMyBoard(boards[0], numRows, numCols);
		playerB.setMyBoard(boards[1], numRows, numCols);
		delete[] boards;
	}
}

pair<int, int> GameMaster::attack()
{
	pair<int, int> curr_move;

	switch (turn)
	{
	case(Players::PlayerA):
		curr_move = playerA.next_move();
		break;

	case(Players::PlayerB):
		curr_move = playerB.next_move();
		break;
	default:
		return make_pair(-1, -1);
	}

	if (curr_move == make_pair(-1, -1))
	{
		cout << "Error: attack() failed on " << ("%s", Players::PlayerA == turn ? "player A" : "player B") << "turn" << endl;
		return make_pair(-1, -1);
	}

	cout << ("%s move:\t", Players::PlayerA == turn ? "player A" : "player B") << "(" << curr_move.first << "," << curr_move.second << ")" << endl;
	return curr_move;
}

void GameMaster::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}

int GameMaster::play()
{
	// Player A starts
	turn = Players::PlayerA;

	pair<int, int> move;

	while(!playerA.done || !playerB.done)
	{
		 move = attack();

		 if (move == make_pair(-1, -1))
		 {
			 // failure 
			 return -1;
		 }

		if (move == make_pair(0, 0))
		{
			// the player has no more moves
			turn = (turn == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
			continue;
		}
		pair<Vessel_ID, AttackResult> results = attack_results(move);

		if (results.second != AttackResult::Miss)
		{
			update_state(move, results);
			cout << ("%s", results.second == AttackResult::Hit ? "Hit " : "Sink ") << ("%s", results.first.type == VesselType::Boat ? "Boat " : results.first.type == VesselType::Missiles ? "Missiles " : results.first.type == VesselType::Sub ? "Sub " : "War ") << endl << "Score " << scores[0] << ":" << scores[1] << endl;

			if (is_defeat())
			{
				break;
			}
			turn = (results.first.player == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
		}

		turn = (turn == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
	}

	print_results();

	return 0;
}


pair<Vessel_ID, AttackResult> GameMaster::attack_results(pair<int,int> move)
{
	int x = move.first;
	int y = move.second;
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
	 boards[move.first][move.second] = '@';
	 scores[(int)results.first.player] += results.first.score;
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

	if (scores[0] > scores[1])
	{
		cout << "The winner is " << "player A with " << scores[0] << " vs. " << scores[1] << endl;
	} 
	else if (scores[0] < scores[1])
	{
		cout << "The winner is " << "player B with " << scores[1] << " vs. " << scores[0] << endl;

	} else
	{
		cout << "No winner is declared, both players with  " << scores[0] << " points" << endl;
	}

}
////-------------------------
////		Utils
////-------------------------


Vessel_ID Utils::get_vessel(char curr, Player playerA, Player playerB)
{
	Vessel_ID vessel;

	if (playerA.myLetters.find(curr) != playerA.myLetters.end())
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
	else if (playerB.myLetters.find(curr) != playerB.myLetters.end())
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

bool Utils::is_sink(char** boards, int x, int y, int curr)
{
	int tx = x;
	int ty = y;

	bool up, down, left, right;

	up = Utils::search_up(boards, x, y, curr);
	down = Utils::search_down(boards, x, y, curr);
	right = Utils::search_right(boards, x, y, curr);
	left = Utils::search_left(boards, x, y, curr);

	return up&down&right&left;
}

bool Utils::search_up(char** boards, int x, int y, char curr)
{
	//search up
	while (++y < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_down(char** boards, int x, int y, char curr)
{
	//search down
	while (--y >= 0 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_right(char** boards, int x, int y, char curr)
{
	//search right
	while (++x < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_left(char** boards, int x, int y, char curr)
{
	//search left
	while (++x < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}


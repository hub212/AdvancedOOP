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
void Player::setBoard(const char** board, int numRows, int numCols)
{
	this->dim = make_pair(numRows, numCols);
}



std::pair<int, int> Player::attack() {

	string line;
	string col;
	string row;
	size_t	pos;
	int col_int;
	int row_int;
	int curr_line = 0;

	ifstream inputFile = ifstream(moves);
	if (!inputFile.is_open()) {
		cout << "Error: unable to open player " << player_num << "move files from path:\n" << (this->moves).c_str() << endl;
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
		if (DEBUG)
			cout << "Error(Debug only): none number input; "<< str << endl;
		return -1;
	}
	catch (out_of_range&)
	{
		if(DEBUG)
			cout << "Error(Debug only): input number is out of range; " << str << endl;
		return -1;
	}
	return 0;
}

void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}

// decising letters for each player
Player::Player(int player_num, char* letters,const char* moves) : moves(moves), player_num(player_num), read_pos(0), line_num(0), done(0), myLetters(letters, letters + strlen(letters)) {};



Player::~Player() 
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
		cout << "Error: double string array allocation failed while allocating multi board; " <<exc.what() << endl;
		return 1;
	}

	try {
		(*out_board)[0] = new char*[numRows];
	}
	catch (std::bad_alloc& exc)
	{
		delete[] *out_board;
		cout << "Error: string array allocation failed for player A while allocating board; " << exc.what() << endl;
		return 1;
	}

	try {
		(*out_board)[1] = new char*[numRows];
	}
	catch (std::bad_alloc& exc)
	{
		delete[] (*out_board)[0];
		delete[] *out_board;
		cout << "Error: string array allocation failed for player B while allocating board; " << exc.what() << endl;
		return 1;
	}

	
	for (int row = 0; row < numRows; row++)
	{
		try {
			(*out_board)[0][row] = new char[numCols + 1];
		}
		catch (std::bad_alloc& exc)
		{
			cout << "Error: string array allocation failed for player B while allocating row strings; " << exc.what() << endl;
			for (int delete_row = 0; delete_row < row; delete_row++)
				delete[](*out_board)[0][delete_row];
			delete[] (*out_board)[0];
			delete[] (*out_board)[1];
			delete[] *out_board;
			return 1;
		}
		try {
			(*out_board)[1][row] = new char[numCols + 1];
		}
		catch (std::bad_alloc& exc)
		{
			cout << "Error: string array allocation failed for player A while allocating row strings; " << exc.what() << endl;
			for (int delete_row = 0; delete_row < numRows; delete_row++)
				delete[](*out_board)[0][delete_row];
			for (int delete_row = 0; delete_row < row; delete_row++)
				delete[](*out_board)[1][delete_row];
			delete[](*out_board)[0];
			delete[](*out_board)[1];
			delete[] * out_board;
			return 1;
		}
		
		if (strcpy_s((*out_board)[0][row], (rsize_t) numCols+1, board[row])) {
			cout << "Error: copy failed failed for boards separating" << endl;
			return 1;
		}

		if (strcpy_s((*out_board)[1][row], (rsize_t) numCols+1, board[row]))
		{
			cout << "Error: memory allocation failed for boards separating" << endl;
			return 1;
		}
		

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
		playerA.setBoard(const_cast<const char**>(boards[0]), numRows, numCols);
		playerB.setBoard(const_cast<const char**>(boards[1]), numRows, numCols);
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
		return make_pair(-1, -1);
	}

	if (curr_move == make_pair(-1, -1))
	{
		cout << "Error: attack() failed on " << ("%s", Players::PlayerA == turn ? "player A" : "player B") << "turn" << endl;
		return make_pair(-1, -1);
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
	while (--x >= 0 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_down(char** boards, int x, int y, char curr)
{
	//search down
	while (++x < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_right(char** boards, int x, int y, char curr)
{
	//search right
	while (++y < 10 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}

bool Utils::search_left(char** boards, int x, int y, char curr)
{
	//search left
	while (--y >= 0 && boards[x][y] != ' ')
	{
		if (boards[x][y] == '@') continue;
		if (boards[x][y] == curr) return false;
	}
	return true;
}


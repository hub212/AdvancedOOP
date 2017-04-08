#include "stdafx.h"
#include "IBattleshipGameAlgo.h"
#include "Game.h"
#include <set>
#include <vector>

using namespace std;

	// copy the relevant vessels
	void Player::setMyBoard(const char** board, int numRows, int numCols)
	{
		this->board = new string[numRows];
		this->dim = make_pair(numRows, numCols);

		for (int i = 0; i<10; i++)
		{
			printf("%s\n", board[i]);
		}

		for(int row = 0; row<numRows; row++)
		{
			(this->board)[row] = string(board[row]);
			for (int col = 0; col < numCols; col++) {
				if (myLetters.find(board[row][col]) == myLetters.end())
				{
					(this->board)[row].replace(col, 1,1, ' ');
				}
				
			}
		}

		for (int i = 0; i<10; i++)
		{
			cout << (this->board)[i] << endl;
		}
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
		ifstream inputFile = ifstream("C:\\Users\\Shlomi\\Source\\Repos\\advancedOOP\\ex1\\clean_movesA.attack-a");
		if (!inputFile.is_open()) {
			cout << "Error: unable to open players " << player_num << ":" << (this->moves).c_str() << strerror((errno)) << endl;
			return make_pair(-1,-1);
		}

		
		while (getline(inputFile, line) ) {
			curr_line++;
			if (curr_line < read_pos) continue;
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
			return make_pair(row_int, col_int);
		}
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
	Player::Player(int player_num, char* letters,const char* moves) : myLetters(letters, letters + strlen(letters)), player_num(player_num), read_pos(0), line_num(0), moves(moves) {};



	Player::~Player() {
		if (board != nullptr)
			delete[] board;
		myLetters.clear();
	}


	GameMaster::GameMaster(const char** boards, vector<const char*>& players_moves, int numRows, int numCols)
{
	playerA = Player(0, "BPMD", players_moves[0]);
	playerB = Player(1, "bpmd", players_moves[1]);
	this->boards = nullptr;
	this->players_moves = players_moves;
	this->dim = make_pair(numRows, numCols);
}


void GameMaster::setBoard(const char** board, int numRows, int numCols)
	{
		playerA.setMyBoard(board, numRows, numCols);
		playerB.setMyBoard(board, numRows, numCols);
	}

pair<int,int> GameMaster::attack()
	{
		pair<int, int> tmp;
		// FIXME - debug leftovers
		while ((tmp = playerA.next_move()) != make_pair(0, 0))
			cout << tmp.first << tmp.second << "\n" << endl;
		return make_pair(-1, -1);
	}

	void GameMaster::notifyOnAttackResult(int player, int row, int col, AttackResult result)
	{
		
	}

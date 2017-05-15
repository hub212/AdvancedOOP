#ifndef PRE_ALGO
#define PRE_ALGO
#include "ex2.h"
#include "CommonAlgo.h"
#include "windows.h"
#include <vector>
#include <iostream>
#include <algorithm>


using namespace std;

// this class is the read from file algo implemetation
class PreMovesAlgo : public CommonAlgo {
public: 
	// read file variables 
	int				read_pos;	// heler for readinf method
	int				line_num;	// indication for actual current line
	std::string		moves_path;  


	PreMovesAlgo(int player_num, char* letters, const  char* moves);
	PreMovesAlgo();
	std::pair<int, int> attack();
	
	// Overrides the Common's function
	bool init(const std::string& path);
	// Overrides the Common's function
	void setBoard(int player, const char** board, int numRows, int numCols);
	void notifyOnAttackResult(int player, int row, int col, AttackResult result);
	int str2int(const string str, int* num);
};

#endif
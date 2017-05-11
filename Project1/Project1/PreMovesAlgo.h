#ifndef PRE_ALGO
#define PRE_ALGO
#include "ex1.h"
#include "CommonAlgo.h"


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
	int str2int(const string str, int* num);
};

#endif
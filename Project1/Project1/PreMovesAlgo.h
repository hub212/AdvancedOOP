#ifndef PRE_ALGO
#define PRE_ALGO
#include "ex1.h"
#include "CommonAlgo.h"


using namespace std;

class PreMovesAlgo : public CommonAlgo {
public: 
	// read file variables
	int				read_pos;
	int				line_num;
	std::string		moves;


	PreMovesAlgo(int player_num, char* letters, const  char* moves);
	PreMovesAlgo();
	std::pair<int, int> attack();
	int str2int(const string str, int* num);
};

#endif
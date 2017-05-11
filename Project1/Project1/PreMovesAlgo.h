#ifndef PRE_ALGO
#define PRE_ALGO
#include "ex1.h"
#include "CommonAlgo.h"


using namespace std;

class PreMovesAlgo : public CommonAlgo {
public: 
	PreMovesAlgo(int player_num, char* letters, const  char* moves);
	PreMovesAlgo();
	std::pair<int, int> attack();
	int str2int(const string str, int* num);
};

#endif
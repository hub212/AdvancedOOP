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
class NaiiveAlgo : public CommonAlgo {
public:

	//michael 12/5/17 08:19 added start
	void				setNextAttack();


	void notifyOnAttackResult(int player, int row, int col, AttackResult result);

	bool init(const std::string& path);

	NaiiveAlgo() = default;

};

#endif
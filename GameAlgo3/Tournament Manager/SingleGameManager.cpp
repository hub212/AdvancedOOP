#include "main.h"
#include "SingleGameManager.h"
#include "Types.h"
#include <iostream>
#include <set>
#include <vector>
#include <tuple>

using namespace std;

////--------------------------
////		SingleGameManager
////--------------------------

SingleGameManager::SingleGameManager(MatchHard match) : match(match), board(get<2>(match)) , origBoard(board), board0(make_shared<Board>(board)) , board1(make_shared<Board>(board)), dims(board.rows(), board.cols(), board.depth()) {
	dll0 = { get<0>(match)->name,get<0>(match)->hdll, get<0>(match)->getAlgo }; 
	dll1 = { get<1>(match)->name,get<1>(match)->hdll, get<1>(match)->getAlgo };
	player0 = get<2>(dll0)();
	player1 = get<2>(dll1)();
	setPlayers();
	setBoards();
	scores[0] = 0;
	scores[1] = 0;
}


void SingleGameManager::setBoards()
{
	player0->setBoard(board0);
	player1->setBoard(board1);
}

void SingleGameManager::setPlayers()
{
	player0->setPlayer(0);
	player1->setPlayer(1);
}

vector<int> SingleGameManager::getScores() {
	return {scores[0], scores[1]};
}

Coordinate SingleGameManager::attack()
{
	Coordinate curr_move = { -1,-1,-1 };

	if (DEBUG) {
		cout << "SingleGameManager::attack()" << endl;
	}
	switch (turn)
	{
		case(Players::PlayerA):
			if (DEBUG) {
				cout << "calling Smart3DPlayer::attack() player 0" << endl;
			}
			curr_move = player0->attack();
			break;

		case(Players::PlayerB):
			if (DEBUG) {
				cout << "calling Smart3DPlayer::attack() player 1" << endl;
			}
			curr_move = player1->attack();
			break;
		default:
			return{ -2, -2,-2 };
	}

	if (curr_move == Coordinate(-2, -2, -2))
	{
		cout << "Error: attack() failed on " << ("%s", Players::PlayerA == turn ? "player A's " : "player B's ") << "turn" << endl;
		return{ -2,-2,-2 };
	}
	else if (curr_move != Coordinate(-2,-2,-2) && DEBUG)
		cout << ("%s", Players::PlayerA == turn ? "player A move:\t" : "player B move:\t") << curr_move << endl;
	return curr_move;
}



int SingleGameManager::play()
{
	// Player A starts
	turn = Players::PlayerA;

	Coordinate move = {-3,-3,-3};
	Coordinate prevMove = { -3,-3,-3 };
	int player0_done = 0;
	int player1_done = 0;
	int conditional = 0;

	while (!player0_done || !player1_done)
	{

		cout << "round: " << conditional << endl;

		board.printBoard();
		
		prevMove = move;
		move = attack();


		// No more moves
		if (move == Coordinate(-1, -1, -1))
		{
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

		if (DEBUG) {
			cout << "board updated" << endl;
			cout << "move coor = " << move << endl;
		}

		player0->notifyOnAttackResult(activePlayerIndex, move, results.second);
		player1->notifyOnAttackResult(activePlayerIndex, move, results.second);

		if (DEBUG) {
			cout << "players notified" << endl;
		}

		if (DEBUG) {
			cout << ("%s ", turn == Players::PlayerA ? "player A move results " : "player B move results ");
			if (results.second != AttackResult::Miss)
				cout << ("%s", results.second == AttackResult::Hit ? "Hit " : "Sink ") << ("%s ", results.first.player == Players::PlayerA ? "player A's " : "player B's ") << ("%s", results.first.type == VesselType::Boat ? "Boat " : results.first.type == VesselType::Missiles ? "Missiles " : results.first.type == VesselType::Sub ? "Sub " : "War ") << endl;
			else
				cout << "Miss" << endl;
			cout << "Score " << scores[0] << ":" << scores[1] << endl;
		}

		if (anyWinner())
		{
			break;
		}

		if (results.second != AttackResult::Miss && turn != results.first.player)
		{
			turn = (turn == Players::PlayerA) ? Players::PlayerA : Players::PlayerB;
		}
		else {
			turn = (turn == Players::PlayerA) ? Players::PlayerB : Players::PlayerA;
		}

		conditional++;
	}

	print_results();

	return 0;
}


pair<Vessel_ID, AttackResult> SingleGameManager::attack_results(Coordinate move)
{
	move = move + Coordinate(-1, -1, -1);
	char shipSign = board.get(move);
	Vessel_ID vessel;

	if (shipSign == '\0') 
	{
		cout << "index out of bounds; move = " << move << endl;
	}

	if (shipSign == '@' || shipSign == ' ')
	{
		return{ Vessel_ID::Vessel_ID(), AttackResult::Miss };
	}

	vessel = SingleGameManager::get_vessel(shipSign, player0, player1);

	bool isSink = Utils::is_sink(board, move, origBoard, dims);


	if (isSink)
	{
		return{ vessel, AttackResult::Sink };
	}
	else
	{
		return{ vessel, AttackResult::Hit };
	}
}

void SingleGameManager::update_state(Coordinate move, pair<Vessel_ID, AttackResult> results)
{
	move = move + Coordinate(-1, -1, -1);
	if (DEBUG) {
		cout << "update state on move" << move << endl;
	}

	board.set(move, '@');

	if (results.second == AttackResult::Sink)
		scores[static_cast<int>((results.first.player == Players::PlayerA ? Players::PlayerB : Players::PlayerA))] += results.first.score;
}

bool SingleGameManager::anyWinner()
{
	bool boolA = false;
	bool boolB = false;
	for (int d = 0; d < dims.depth; d++) {
		for (int i = 0; i < dims.row; i++)
		{
			for (int j = 0; j < dims.col && (!boolA || !boolB); j++)
			{
				if (lettersA.find(board.get(d,i,j)) != lettersA.end())
				{
					boolA = true;
				}

				if (lettersB.find(board.get(d, i, j)) != lettersB.end())
				{
					boolB = true;
				}
			}
		}
	}

	return boolA^boolB;
}


Vessel_ID SingleGameManager::get_vessel(char curr, IBattleshipGameAlgo* playerA, IBattleshipGameAlgo* playerB)
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


void SingleGameManager::print_results()
{
	if (scores[0] != scores[1])
		cout << "Player " << ("%s", scores[0] > scores[1] ? "A " : "B ") << "won" << endl;

	cout << "Points: " << endl;
	cout << "Player A: " << ("%d", scores[0])  << endl;
	cout << "Player B: " << ("%d", scores[1]) << endl;

}

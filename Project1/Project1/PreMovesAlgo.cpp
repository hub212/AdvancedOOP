#include "PreMovesAlgo.h"
#include "utils.h"


std::pair<int, int> PreMovesAlgo::attack() {

		string line;
		string col;
		string row;
		size_t	pos;
		int col_int;
		int row_int;
		int curr_line = 0;

		ifstream inputFile = ifstream(moves_path);
		if (!inputFile.is_open()) {
			cout << "Error: unable to open player " << player_num << "move files from path:\n" << (this->moves_path).c_str() << endl;
			return make_pair(0, 0);
		}


		while (getline(inputFile, line)) {
			if (curr_line < read_pos) {
				curr_line++;
				continue;
			}
			read_pos += 1;
			if (line.size() && line[line.size() - 1] == '\r') {
				line = line.substr(0, line.size() - 1);
			}

			pos = line.find(",");
			if (pos == (unsigned long long) (line.length() - 1)) continue;

			row = line.substr(0, pos);
			col = line.substr(pos + 1, line.length());


			if ((str2int(row, &row_int) != 0) || (str2int(col, &col_int)) != 0) continue;

			if (row_int > rows || col_int > cols) continue;

			inputFile.close();
			return make_pair(row_int , col_int );
		}
		done = 1;
		return make_pair(-1, -1);
	}



bool PreMovesAlgo::init(const std::string& path) {

	char pwd[MY_MAX_PATH];
	string moves_dir;
	vector<string> moves_list;

	// when using get_algo() we nned to specify the letters of each player expicitly
	if (myLetters.empty()) {
		if (player_num == 0) {
			char* letters = "BPMD";
			myLetters.insert(letters, letters + strlen(letters));
		}
		else {
			char* letters = "bpmd";
			myLetters.insert(letters, letters + strlen(letters));
		}
	}
	// getting paths to moves dir
	GetCurrentDirectoryA(MY_MAX_PATH, pwd);
	moves_dir = pwd;


	// builds moves vector list
	Utils::GetFileNamesInDirectory(&moves_list, moves_dir);

	// FIXME - replace attack-a with attack 
	// removing not '*.attack files from list
	moves_list.erase(remove_if(moves_list.begin(), moves_list.end(), [](string str) { return !Utils::string_has_suffix(str, ".attack-a"); }), moves_list.end());

	std::sort(moves_list.begin(), moves_list.end());
	
	if (static_cast<int> (moves_list.size()) ==  0){
		cout << "Algorithm initializatoin failed for dll: TODO add full path for this dll" << endl;
		return false;
	}

	if (static_cast<int>(moves_list.size()) == 1) {
		moves_path = moves_list[0];
		return true;
	}

	moves_path = moves_list[player_num];

	return true;
}


void PreMovesAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->player_num = player;
	rows = numRows;
	cols = numCols;
}


void PreMovesAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{

}


int PreMovesAlgo::str2int(const string str, int* num)
{

	try
	{
		*num = stoi(str);
	}
	catch (invalid_argument&)
	{
		if (DEBUG)
			cout << "Error(Debug only): none number input; " << str << endl;
		return -1;
	}
	catch (out_of_range&)
	{
		if (DEBUG)
			cout << "Error(Debug only): input number is out of range; " << str << endl;
		return -1;
	}
	return 0;
}

void PreMovesAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result) {}

PreMovesAlgo::PreMovesAlgo(int player_num, char* letters, const  char* moves) : CommonAlgo(player_num, letters), moves_path(moves), read_pos(0), line_num(0) {}

PreMovesAlgo::PreMovesAlgo() : read_pos(0), line_num(0) {};


IBattleshipGameAlgo* GetAlgorithm() {
	return (IBattleshipGameAlgo*) new PreMovesAlgo();
}


#include "PreMovesAlgo.h"


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

			if (row_int > dim.first || col_int > dim.second) continue;

			inputFile.close();
			return make_pair(row_int , col_int );
		}
		done = 1;
		return make_pair(-1, -1);
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

PreMovesAlgo::PreMovesAlgo(int player_num, char* letters, const  char* moves) : CommonAlgo(player_num, letters), moves_path(moves), read_pos(0), line_num(0) {}

PreMovesAlgo::PreMovesAlgo() : CommonAlgo() {};
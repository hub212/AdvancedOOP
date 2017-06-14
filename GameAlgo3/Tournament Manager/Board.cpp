#include "Board.h"

int Board::shipSize(char ch) {
	if (tolower(ch) == tolower('B')) {
		return 1;
	}
	if (tolower(ch) == tolower('P')) {
		return 2;
	}
	if (tolower(ch) == tolower('M')) {
		return 3;
	}
	if (tolower(ch) == tolower('D')) {
		return 4;
	}
	return -1;
}

int Board::countNonEmptyCells() const {
	return num_taken_cells;
}

char Board::get(int depth, int row, int col) const{
	if (depth < 0 || depth >= this->num_depth || row < 0 || row >= num_rows || col < 0 || col >= num_cols) {
		return '\0';
	}
	else {
		return (*board)[depth][row][col];
	}
}

void Board::set(int depth, int row, int col, char ch) {
	if (depth < 0 || depth >= this->num_depth || row < 0 || row >= num_rows || col < 0 || col >= num_cols) {
		return;
	}
	else {
		(*board)[depth][row][col] = ch;
	}
}

char Board::get(Coordinate coor)const {
	return Board::get(coor.depth, coor.row, coor.col);
}

void Board::set(Coordinate coor, char ch) {
	Board::set(coor.depth, coor.row, coor.col, ch);
}

std::istream& Board::safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

v3dp Board::newBoard(int depth, int rows, int cols) {

	return std::make_shared<v3d>(depth, v2d(rows, v1d(cols, ' ')));
}

Board::Board(v3dp other, int depth, int num_rows, int num_cols) {

	this->num_depth = depth;
	this->num_rows = num_rows;
	this->num_cols = num_cols;
	this->num_taken_cells = 0;

	board = newBoard(depth, num_rows, num_cols);

	if (isDebug)
		std::cout << "creating a board from a pointer" << std::endl;

	for (int dpth = 0; dpth < depth; dpth++) {
		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			for (int col_index = 0; col_index < num_cols; col_index++) {
				(*board)[dpth][row_index][col_index] = (*other)[dpth][row_index][col_index];
			}
		}
	}
}

bool Board::isDebug = false;

Board::Board(std::ifstream& file) {


	int depth, num_rows, num_cols;

	std::string str;
	bool lineExists = false;
	if (!safeGetline(file, str).eof()) {
		lineExists = true;
	}
	else {
		std::cout << "board is empty" << std::endl;
	}
	if (!std::regex_match(str, std::regex("\\d+x\\d+x\\d+"))) {
		std::cout << "first board line is not in format '<cols>x<rows>x<depth>'" << std::endl;
	}

	for (size_t i = 0; i < str.length(); i++) {
		if (str.at(i) == 'x') {
			str.at(i) = ' ';
		}
	}
	sscanf_s(str.c_str(), "%d %d %d", &num_cols, &num_rows, &depth);

	if (!safeGetline(file, str).eof()) {
		if (strcmp(str.c_str(), "") && strcmp(str.c_str(), "\r")) {
			std::cout << "board's second line is not empty" << std::endl;
		}
	}
	else {
		std::cout << "board is empty after first line" << std::endl;
	}

	bool afterEmptyLine = true;

	this->num_depth = depth;
	this->num_rows = num_rows;
	this->num_cols = num_cols;
	this->num_taken_cells = 0;

	if (isDebug)
		std::cout << "creating a board from file" << std::endl;

	board = newBoard(depth, num_rows, num_cols);

	for (int dpth = 0; dpth < depth; dpth++) {


		while (!afterEmptyLine) {
			if (safeGetline(file, str).eof() || !strcmp(str.c_str(), "") || !strcmp(str.c_str(), "\r")) {
				break;
			}
		}
		afterEmptyLine = false;

		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			for (int col_index = 0; col_index < num_cols; col_index++) {
				(*board)[dpth][row_index][col_index] = ' ';
			}
			lineExists = false;
			if (!afterEmptyLine && !safeGetline(file, str).eof()) {
				if (!strcmp(str.c_str(), "") || !strcmp(str.c_str(), "\r")) {
					afterEmptyLine = true;
				}
				lineExists = true;
			}

			for (size_t col_index = 0; lineExists && col_index < num_cols && col_index < str.length() && !afterEmptyLine; col_index++) {
				char inputChar = str.at(col_index);
				bool charIsLegal = false;
				if (inputChar == ' ' || shipSize(inputChar) > 0) {
					charIsLegal = true;
				}

				if (charIsLegal) {
					this->num_taken_cells++;
					(*board)[dpth][row_index][col_index] = inputChar;
				}
			}
		}
	}
}

void Board::printBoard() const {
	std::cout << num_cols << "x" << num_rows << "x" << num_depth << std::endl;
	for (int dpth = 0; dpth < num_depth; dpth++) {
		std::cout << std::endl;
		for (int row_index = 0; row_index < num_rows; row_index++) {
			std::string rowString((*board)[dpth][row_index].begin(), (*board)[dpth][row_index].end());
			if (rowString.length() > num_cols) {
				rowString = rowString.substr(0, num_cols);
			}
			if (row_index == 0) {
				std::cout << " 1234567890" << std::endl;
			}
			std::cout << (row_index+1)%10 << rowString << std::endl;
		}
	}
	std::cout << std::endl;
}

int Board::randomGen(int min, int max) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(min, max);

	int random_integer = uni(rng);
	return random_integer;
}

void Board::printBoard(const std::string filePath) const {

	std::ofstream myfile;
	myfile.open(filePath);

	myfile << num_cols << "x" << num_rows << "x" << num_depth << std::endl;
	for (int dpth = 0; dpth < num_depth; dpth++) {
		myfile << std::endl;
		for (int row_index = 0; row_index < num_rows; row_index++) {
			std::string rowString((*board)[dpth][row_index].begin(), (*board)[dpth][row_index].end());
			if (rowString.length() > num_cols) {
				rowString = rowString.substr(0, num_cols);
			}
			myfile << rowString << std::endl;
		}
	}
	myfile << std::endl;
	myfile.close();
}

Board::Board(int dpth, int num_rows, int num_cols) : num_depth(dpth), num_rows(num_rows), num_cols(num_cols), num_taken_cells(0)
{
	board = newBoard(dpth, num_rows, num_cols);

	if (isDebug)
		std::cout << "creating a fresh board" << std::endl;

}

Board::Board(const Board &other) : num_depth(other.num_depth), num_rows(other.num_rows), num_cols(other.num_cols)
{
	board = newBoard(num_depth, num_rows, num_cols);

	if (isDebug)
		std::cout << "creating a board with copy constructor" << std::endl;

	for (int dpth = 0; dpth < num_depth; dpth++) {
		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			for (int col_index = 0; col_index < num_cols; col_index++) {
				(*board)[dpth][row_index][col_index] = (*other.board)[dpth][row_index][col_index];
			}
		}
	}
	num_taken_cells = other.num_taken_cells;
}

//constructs a new board of the same size that contains only the shape that contains the cell (dpth, row, col)
Board::Board(const Board &other, int dpth, int row, int col) : num_depth(other.num_depth), num_rows(other.num_rows), num_cols(other.num_cols)
{
	num_taken_cells = 0;
	board = newBoard(num_depth, num_rows, num_cols);

	if (isDebug)
		std::cout << "creating a board with a new shape" << std::endl;

	for (int dpth = 0; dpth < num_depth; dpth++) {
		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			for (int col_index = 0; col_index < num_cols; col_index++) {
				(*board)[dpth][row_index][col_index] = ' ';
			}
		}
	}
	char currChar = (*other.board)[dpth][row][col];
	if (currChar != ' ') {
		(*board)[dpth][row][col] = currChar;
		num_taken_cells++;
		fillShape(dpth, row, col, other, currChar);
	}
}

void Board::genRandomBoard(int ships) {

	int i = 0;

	for (; i < ships;) {


		int ship = randomGen(1, 4);
		char shipChar = ship == 1 ? 'b' : (ship == 2 ? 'p' : (ship == 3 ? 'm' : 'd'));

		int j = 0;
		bool shipGenerated = false;

		for (; j < 100 && !shipGenerated; j++) {
			shipGenerated = genRandomShip(shipChar, ship);
		}
		if (!shipGenerated) {
			continue;
		}


		i++;
	}

}

bool Board::genRandomShip(char c, int size) {
	int dir = randomGen(1, 3);

	int maxColIdx = num_cols - 1 - (dir == 1 ? size : 0);
	int maxRowIdx = num_rows - 1 - (dir == 2 ? size : 0);
	int maxDepthIdx = num_depth - 1 - (dir == 3 ? size : 0);

	if (maxColIdx < 0 || maxRowIdx < 0 || maxDepthIdx < 0) {
		return false;
	}


	int row = randomGen(0, maxRowIdx);
	int col = randomGen(0, maxColIdx);
	int dpt = randomGen(0, maxDepthIdx);


	int i = 0;
	if (dir == 1) {
		for (i = 0; i < size; i++) {
			if ((*board)[dpt][row][col + i] != ' ') {
				return false;
			}
		}
	}
	if (dir == 2) {
		for (i = 0; i < size; i++) {
			if ((*board)[dpt][row + i][col] != ' ') {
				return false;
			}
		}
	}
	if (dir == 3) {
		for (i = 0; i < size; i++) {
			if ((*board)[dpt + i][row][col] != ' ') {
				return false;
			}
		}
	}
	i = 0;

	Board tmpBoard = Board(*this);

	if (dir == 1) {
		for (i = 0; i < size; i++) {
			(*tmpBoard.board)[dpt][row][col + i] = 'z';
		}
	}
	if (dir == 2) {
		for (i = 0; i < size; i++) {
			(*tmpBoard.board)[dpt][row + i][col] = 'z';
		}
	}
	if (dir == 3) {
		for (i = 0; i < size; i++) {
			(*tmpBoard.board)[dpt + i][row][col] = 'z';
		}
	}
	if (tmpBoard.areAdjacentShapes()) {
		return false;
	}







	int dir2 = randomGen(1, 3);

	maxColIdx = num_cols - 1 - (dir2 == 1 ? size : 0);
	maxRowIdx = num_rows - 1 - (dir2 == 2 ? size : 0);
	maxDepthIdx = num_depth - 1 - (dir2 == 3 ? size : 0);

	if (maxColIdx < 0 || maxRowIdx < 0 || maxDepthIdx < 0) {
		return false;
	}

	int row2 = randomGen(0, maxRowIdx);
	int col2 = randomGen(0, maxColIdx);
	int dpt2 = randomGen(0, maxDepthIdx);

	if (dir2 == 1) {
		for (i = 0; i < size; i++) {

			if ((*tmpBoard.board)[dpt2][row2][col2 + i] != ' ') {
				return false;
			}
		}
	}
	if (dir2 == 2) {
		for (i = 0; i < size; i++) {
			if ((*tmpBoard.board)[dpt2][row2 + i][col2] != ' ') {
				return false;
			}
		}
	}
	if (dir2 == 3) {
		for (i = 0; i < size; i++) {
			if ((*tmpBoard.board)[dpt2 + i][row2][col2] != ' ') {
				return false;
			}
		}
	}
	i = 0;
	if (dir2 == 1) {
		for (i = 0; i < size; i++) {
			(*tmpBoard.board)[dpt2][row2][col2 + i] = 'y';
		}
	}
	if (dir2 == 2) {
		for (i = 0; i < size; i++) {
			(*tmpBoard.board)[dpt2][row2 + i][col2] = 'y';
		}
	}
	if (dir2 == 3) {
		for (i = 0; i < size; i++) {
			(*tmpBoard.board)[dpt2 + i][row2][col2] = 'y';
		}
	}
	if (tmpBoard.areAdjacentShapes()) {
		return false;
	}















	if (dir == 1) {
		for (i = 0; i < size; i++) {
			(*board)[dpt][row][col + i] = c;
		}
	}
	if (dir == 2) {
		for (i = 0; i < size; i++) {
			(*board)[dpt][row + i][col] = c;
		}
	}
	if (dir == 3) {
		for (i = 0; i < size; i++) {
			(*board)[dpt + i][row][col] = c;
		}
	}



	if (dir2 == 1) {
		for (i = 0; i < size; i++) {
			(*board)[dpt2][row2][col2 + i] = toupper(c);
		}
	}
	if (dir2 == 2) {
		for (i = 0; i < size; i++) {
			(*board)[dpt2][row2 + i][col2] = toupper(c);
		}
	}
	if (dir2 == 3) {
		for (i = 0; i < size; i++) {
			(*board)[dpt2 + i][row2][col2] = toupper(c);
		}
	}
	return true;
}



void Board::fillShape(int dpth, int row, int col, const Board &other, char currChar)
{
	if (dpth - 1 >= 0 && (*board)[dpth - 1][row][col] == ' ' && (*other.board)[dpth - 1][row][col] == currChar) {
		(*board)[dpth - 1][row][col] = currChar;
		num_taken_cells++;
		fillShape(dpth - 1, row, col, other, currChar);
	}
	if (dpth + 1 < num_depth && (*board)[dpth + 1][row][col] == ' ' && (*other.board)[dpth + 1][row][col] == currChar) {
		(*board)[dpth + 1][row][col] = currChar;
		num_taken_cells++;
		fillShape(dpth + 1, row, col, other, currChar);
	}
	if (row - 1 >= 0 && (*board)[dpth][row - 1][col] == ' ' && (*other.board)[dpth][row - 1][col] == currChar) {
		(*board)[dpth][row - 1][col] = currChar;
		num_taken_cells++;
		fillShape(dpth, row - 1, col, other, currChar);
	}
	if (col + 1 < num_cols && (*board)[dpth][row][col + 1] == ' ' && (*other.board)[dpth][row][col + 1] == currChar) {
		(*board)[dpth][row][col + 1] = currChar;
		num_taken_cells++;
		fillShape(dpth, row, col + 1, other, currChar);
	}
	if (row + 1 < num_rows && (*board)[dpth][row + 1][col] == ' ' && (*other.board)[dpth][row + 1][col] == currChar) {
		(*board)[dpth][row + 1][col] = currChar;
		num_taken_cells++;
		fillShape(dpth, row + 1, col, other, currChar);
	}
	if (col - 1 >= 0 && (*board)[dpth][row][col - 1] == ' ' && (*other.board)[dpth][row][col - 1] == currChar) {
		(*board)[dpth][row][col - 1] = currChar;
		num_taken_cells++;
		fillShape(dpth, row, col - 1, other, currChar);
	}
}

bool Board::areAdjacentShapes() const {
	for (int dpth = 0; dpth < num_depth; dpth++) {
		for (int row = 0; row < num_rows; row++) {
			for (int col = 0; col < num_cols; col++) {
				if ((*board)[dpth][row][col] != ' ') {
					if (row + 1 < num_rows && (*board)[dpth][row + 1][col] != ' ' && (*board)[dpth][row][col] != (*board)[dpth][row + 1][col]) {
						return true;
					}
					if (col + 1 < num_cols && (*board)[dpth][row][col + 1] != ' ' && (*board)[dpth][row][col] != (*board)[dpth][row][col + 1]) {
						return true;
					}
					if (dpth + 1 < num_depth && (*board)[dpth + 1][row][col] != ' ' && (*board)[dpth][row][col] != (*board)[dpth + 1][row][col]) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

int Board::minOccupiedRow() const {
	for (int row = 0; row < num_rows; row++) {
		for (int col = 0; col < num_cols; col++) {
			for (int dpth = 0; dpth < num_depth; dpth++) {
				if ((*board)[dpth][row][col] != ' ') {
					return row;
				}
			}
		}
	}
	return -1;
}

int Board::minOccupiedCol() const {
	for (int col = 0; col < num_cols; col++) {
		for (int row = 0; row < num_rows; row++) {
			for (int dpth = 0; dpth < num_depth; dpth++) {
				if ((*board)[dpth][row][col] != ' ') {
					return col;
				}
			}
		}
	}
	return -1;
}

int Board::maxOccupiedRow() const {
	for (int row = num_rows - 1; row >= 0; row--) {
		for (int col = 0; col < num_cols; col++) {
			for (int dpth = 0; dpth < num_depth; dpth++) {
				if ((*board)[dpth][row][col] != ' ') {
					return row;
				}
			}
		}
	}
	return -1;
}

int Board::maxOccupiedCol() const {
	for (int col = num_cols - 1; col >= 0; col--) {
		for (int row = 0; row < num_rows; row++) {
			for (int dpth = 0; dpth < num_depth; dpth++) {
				if ((*board)[dpth][row][col] != ' ') {
					return col;
				}
			}
		}
	}
	return -1;
}

int Board::minOccupiedDpth() const {
	for (int dpth = 0; dpth < num_depth; dpth++) {
		for (int row = 0; row < num_rows; row++) {
			for (int col = 0; col < num_cols; col++) {
				if ((*board)[dpth][row][col] != ' ') {
					return dpth;
				}
			}
		}
	}
	return -1;
}

int Board::maxOccupiedDpth() const {
	for (int dpth = num_depth - 1; dpth >= 0; dpth--) {
		for (int row = 0; row < num_rows; row++) {
			for (int col = 0; col < num_cols; col++) {
				if ((*board)[dpth][row][col] != ' ') {
					return dpth;
				}
			}
		}
	}
	return -1;
}

void Board::add(const Board &other) {
	for (int row_index = 0; row_index < num_rows && row_index < other.num_rows; row_index++) {
		for (int col_index = 0; col_index < num_cols && col_index < other.num_cols; col_index++) {
			for (int dpth = 0; dpth < num_depth && dpth < other.num_depth; dpth++) {
				if ((*other.board)[dpth][row_index][col_index] != ' ') {
					if ((*board)[dpth][row_index][col_index] == ' ') {
						num_taken_cells++;
					}
					(*board)[dpth][row_index][col_index] = (*other.board)[dpth][row_index][col_index];
				}
			}
		}
	}
}

Board::~Board()
{
	if (isDebug) {
		std::cout << "deleting board" << std::endl;
	}

}

bool Board::isNull() const{
	if (board == nullptr) {
		return true;
	}
	return false;
}

int Board::rows() const {
	return num_rows;
}
int Board::cols() const {
	return num_cols;
}
int Board::depth() const {
	return num_depth;
}

char Player0Board::charAt(Coordinate c) const {
	char ch;
	if (brd->isNull() || c.row < 1 || c.col < 1 || c.depth < 1 || c.row > rows() || c.col > cols() || c.depth > depth()) {
		return ' ';
	}
	ch = brd->get(c.depth-1, c.row-1, c.col-1);

	if (ch == ' ' || !isupper(ch)) {
		return ' ';
	}

	return ch;
}

char Player1Board::charAt(Coordinate c) const {
	char ch;
	if (brd->isNull() || c.row < 1 || c.col < 1 || c.depth < 1 || c.row > rows() || c.col > cols() || c.depth > depth()) {
		return ' ';
	}
	ch = brd->get(c.depth - 1, c.row - 1, c.col - 1);

	if (ch == ' ' || !islower(ch)) {
		return ' ';
	}

	return ch;
}

Player0Board::Player0Board(const std::shared_ptr<const Board> &board) : brd(board) {
	_rows = brd->rows();
	_cols = brd->cols();
	_depth = brd->depth();
}

Player1Board::Player1Board(const std::shared_ptr<const Board> &board) : brd(board) {
	_rows = brd->rows();
	_cols = brd->cols();
	_depth = brd->depth();
}


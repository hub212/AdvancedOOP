#include "Board.h"

int Board::countNonEmptyCells() {
	return num_taken_cells;
}

char Board::get(int row, int col) {
	if (row < 0 || row >= num_rows || col < 0 || col >= num_cols) {
		return '\0';
	}
	else {
		return board[row][col];
	}
}

void Board::set(int row, int col, char ch) {
	if (row < 0 || row >= num_rows || col < 0 || col >= num_cols) {
		return;
	}
	else {
		board[row][col] = ch;
	}
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

Board::Board(char** arr, int num_rows, int num_cols) {
	
	this->num_rows = num_rows;
	this->num_cols = num_cols;
	this->num_taken_cells = 0;
	
	
	board = new char*[num_rows];

	if (BoardChecker::isDebug)
		std::cout << "creating a fresh board" << std::endl;

	for (int row_index = 0; row_index < num_rows; row_index++)
	{
		board[row_index] = new char[num_cols];
		for (int col_index = 0; col_index < num_cols; col_index++) {
			board[row_index][col_index] = arr[row_index][col_index];
		}
	}
}

Board::Board(std::ifstream& file, int num_rows, int num_cols) {
	
	this->num_rows = num_rows;
	this->num_cols = num_cols;
	this->num_taken_cells = 0;

	if(BoardChecker::isDebug)
		std::cout << "creating a board from file" << std::endl;

	board = new char*[num_rows];
	std::string str;
	int row_index;
	for (row_index = 0; row_index<num_rows; row_index++)
	{
		board[row_index] = new char[num_cols];
		for (int col_index = 0; col_index < num_cols; col_index++) {
			board[row_index][col_index] = ' ';
		}
		bool lineExists = false;
		if (!safeGetline(file, str).eof()) {
			lineExists = true;
		}

		for (uint16_t col_index = 0; lineExists && col_index < num_cols && col_index < str.length(); col_index++) {
			char inputChar = str.at(col_index);
			bool charIsLegal = false;
			if (inputChar == ' ' || BoardChecker::shipSize(inputChar) > 0) {
				charIsLegal = true;
			}

			if (charIsLegal) {
				this->num_taken_cells++;
				board[row_index][col_index] = inputChar;
			}
		}
	}
}

void Board::printBoard() {
	for (uint16_t row_index = 0; row_index < num_rows; row_index++) {
		std::string rowString(board[row_index]);
		if (rowString.length() > static_cast<std::size_t>(num_cols)) {
			rowString = rowString.substr(0, num_cols);
		}
		std::cout << rowString << std::endl;
	}
}

Board::Board(int num_rows, int num_cols) : num_rows(num_rows), num_cols(num_cols), num_taken_cells(0)
{
	board = new char*[num_rows];

	if (BoardChecker::isDebug)
		std::cout << "creating a fresh board" << std::endl;

	for (int row_index = 0; row_index < num_rows; row_index++)
	{
		board[row_index] = new char[num_cols];
		for (int col_index = 0; col_index < num_cols; col_index++) {
			board[row_index][col_index] = ' ';
		}
	}
}

Board::Board(const Board &other) : num_rows(other.num_rows), num_cols(other.num_cols)
{
	board = new char*[num_rows];
	
	if (BoardChecker::isDebug)
		std::cout << "creating a board with copy constructor" << std::endl;

	for (int row_index = 0; row_index < num_rows; row_index++)
	{
		board[row_index] = new char[num_cols];
		for (int col_index = 0; col_index < num_cols; col_index++) {
			board[row_index][col_index] = other.board[row_index][col_index];
		}
	}
	num_taken_cells = other.num_taken_cells;
}

//constructs a new board of the same size that contains only the shape that contains the cell (row, col)
Board::Board(const Board &other, int row, int col) : num_rows(other.num_rows), num_cols(other.num_cols)
{
	num_taken_cells = 0;
	board = new char*[num_rows];

	if (BoardChecker::isDebug)
		std::cout << "creating a board with a new shape" << std::endl;

	for (int row_index = 0; row_index < num_rows; row_index++)
	{
		board[row_index] = new char[num_cols];
		for (int col_index = 0; col_index < num_cols; col_index++) {
			board[row_index][col_index] = ' ';
		}
	}
	char currChar = other.board[row][col];
	if (currChar != ' ') {
		board[row][col] = currChar;
		num_taken_cells++;
		fillShape(row, col, other, currChar);
	}
}

void Board::fillShape(int row, int col, const Board &other, char currChar)
{
	if (row - 1 >= 0 && board[row - 1][col] == ' ' && other.board[row - 1][col] == currChar) {
		board[row - 1][col] = currChar;
		num_taken_cells++;
		fillShape(row - 1, col, other, currChar);
	}
	if (col + 1 < num_cols && board[row][col + 1] == ' ' && other.board[row][col + 1] == currChar) {
		board[row][col + 1] = currChar;
		num_taken_cells++;
		fillShape(row, col + 1, other, currChar);
	}
	if (row + 1 < num_rows && board[row + 1][col] == ' ' && other.board[row + 1][col] == currChar) {
		board[row + 1][col] = currChar;
		num_taken_cells++;
		fillShape(row + 1, col, other, currChar);
	}
	if (col - 1 >= 0 && board[row][col - 1] == ' ' && other.board[row][col - 1] == currChar) {
		board[row][col - 1] = currChar;
		num_taken_cells++;
		fillShape(row, col - 1, other, currChar);
	}
}

bool Board::areAdjacentShapes() {
	for (int row = 0; row < num_rows - 1; row++) {
		for (int col = 0; col < num_cols - 1; col++) {
			if (board[row][col] != ' ') {
				if (board[row + 1][col] != ' ' && board[row][col] != board[row + 1][col]) {
					return true;
				}
				if (board[row][col + 1] != ' ' && board[row][col] != board[row][col + 1]) {
					return true;
				}
			}
		}
	}
	return false;
}

int Board::minOccupiedRow() {
	for (int row = 0; row < num_rows; row++) {
		for (int col = 0; col < num_cols; col++) {
			if (board[row][col] != ' ') {
				return row;
			}
		}
	}
	return -1;
}

int Board::minOccupiedCol() {
	for (int col = 0; col < num_cols; col++) {
		for (int row = 0; row < num_rows; row++) {
			if (board[row][col] != ' ') {
				return col;
			}
		}
	}
	return -1;
}

int Board::maxOccupiedRow() {
	for (int row = num_rows - 1; row >= 0; row--) {
		for (int col = 0; col < num_cols; col++) {
			if (board[row][col] != ' ') {
				return row;
			}
		}
	}
	return -1;
}

int Board::maxOccupiedCol() {
	for (int col = num_cols - 1; col >= 0; col--) {
		for (int row = 0; row < num_rows; row++) {
			if (board[row][col] != ' ') {
				return col;
			}
		}
	}
	return -1;
}

void Board::add(const Board &other) {
	for (int row_index = 0; row_index < num_rows && row_index < other.num_rows; row_index++)
	{
		for (int col_index = 0; col_index < num_cols && col_index < other.num_cols; col_index++) {
			if (other.board[row_index][col_index] != ' ') {
				if (board[row_index][col_index] == ' ') {
					num_taken_cells++;
				}
				board[row_index][col_index] = other.board[row_index][col_index];
			}
		}
	}
}

Board::~Board()
{
	if (BoardChecker::isDebug) {
		std::cout << "deleting board" << std::endl;
	}
	if (board != nullptr) {
		for (int row_index = 0; row_index < num_rows; row_index++)
		{
			if (board[row_index] != nullptr) {
				delete[] board[row_index];
				board[row_index] = nullptr;
			}
		}
		delete[] board;
		board = nullptr;
	}
}

int Board::numRows() {
	return num_rows;
}
int Board::numCols() {
	return num_cols;
}
char** Board::getboard() {
	return board;
}
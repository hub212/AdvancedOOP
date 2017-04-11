#pragma once
#include "stdafx.h"
#include "BoardChecker.h"
class Board
{
	int num_rows;
	int num_cols;
	int num_taken_cells;
	char** board;

public:
	int numRows();
	int numCols();
	char** getboard();
	int countNonEmptyCells();
	char get(int row, int col);
	void set(int row, int col, char ch);
	void printBoard();
	static std::istream& safeGetline(std::istream& is, std::string& t);
	Board(std::ifstream& file, int num_rows, int num_cols);
	Board(const Board &other, int row, int col);
	Board(const Board &other);
	Board(int num_rows, int num_cols);
	void add(const Board &other);
	void fillShape(int row, int col, const Board &other, char currChar);
	bool areAdjacentShapes();
	int minOccupiedRow();
	int minOccupiedCol();
	int maxOccupiedRow();
	int maxOccupiedCol();
	~Board();
};


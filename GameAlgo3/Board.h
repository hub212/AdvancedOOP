#ifndef BOARD_H
#define BOARD_H
#include <algorithm>
#include <regex>
#include <random>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "IBattleshipGameAlgo.h"

typedef std::vector<char> v1d;
typedef std::vector<v1d> v2d;
typedef std::vector<v2d> v3d;
typedef std::shared_ptr<v3d> v3dp;

class Board 
{

	v3dp board;

	int num_depth;
	int num_rows;
	int num_cols;
	int num_taken_cells;
	
	v3dp newBoard(int depth, int rows, int cols);
	bool genRandomShip(char c, int size);
	static int randomGen(int min, int max);
	static std::istream& safeGetline(std::istream& is, std::string& t);
	void fillShape(int dpth, int row, int col, const Board &other, char currChar);

public:

	static bool isDebug;
	
	static int shipSize(char ch);
	void add(const Board &other);
	void genRandomBoard(int ships);
	Board(std::ifstream& file);
	Board(const Board &other, int depth, int row, int col);
	Board(const Board &other);
	Board(int depth, int num_rows, int num_cols);
	Board(v3dp other, int depth, int num_rows, int num_cols);
	char get(int dpth, int row, int col) const;
	void set(int dpth, int row, int col, char ch);
	char get(Coordinate coor) const;
	void set(Coordinate coor, char ch);
	int countNonEmptyCells() const;
	void printBoard() const;
	void printBoard(const std::string filePath) const;
	bool areAdjacentShapes() const;
	int minOccupiedRow() const;
	int maxOccupiedRow() const;
	int minOccupiedCol() const;
	int maxOccupiedCol() const;
	int minOccupiedDpth() const;
	int maxOccupiedDpth() const;
	bool isNull() const;
	int rows() const;
	int cols() const;
	int depth() const;
	~Board();
};

class Player0Board : public BoardData {
	const std::shared_ptr<const Board> &brd;
public:
	Player0Board(const std::shared_ptr<const Board> &board);
	virtual char charAt(Coordinate c) const;
};

class Player1Board : public BoardData {
	const std::shared_ptr<const Board> &brd;
public:
	Player1Board(const std::shared_ptr<const Board>& board);
	virtual char charAt(Coordinate c) const;
};



#endif
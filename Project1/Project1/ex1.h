#pragma once
#ifndef ex1_h
#define ex1_h

#define NUM_ROWS 10
#define NUM_COLS 10
#define MY_MAX_PATH 1024

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

/*
	@pre : none;
	This function read the files in the dir path given, or the working directory if not provided.
	Looks for the game files, where first appearance is the final one saved.
	This function prints the err msgs in-order as in spec;
	@post : returns NULL on any errors as in spec;
		Else, return string array of size 3 where each string in index is : 
			0 - sboard file path;
			1 - attack-a file path;
			2 - attack-b file path;
*/
string** extract_input_files(int argc, char** argv);



#endif // !ex1_h
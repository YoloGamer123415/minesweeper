#include "argumentParser.h"

#include <iostream>
#include <string.h>

void Arguments::parse(int argc, char const *argv[])
{
	if (argc <= 1) this->error("Not enough arguments");

	if ( strcmp(argv[1], "easy") == 0 )
	{
		this->mode = (GameMode) {
			.name = "easy",
			.size = 9,
		};
	}
	else if ( strcmp(argv[1], "normal") == 0 )
	{
		this->mode = (GameMode) {
			.name = "normal",
			.size = 14,
		};
	}
	else if ( strcmp(argv[1], "hard") == 0 )
	{
		this->mode = (GameMode) {
			.name = "hard",
			.size = 18,
		};
	}
	else
	{
		this->error("First argument must be one of easy, normal or hard");
	}
}

void Arguments::error(std::string error)
{
	std::cerr << error << std::endl;
	exit(1);
}

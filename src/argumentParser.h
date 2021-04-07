#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <iostream>
#include <string.h>

#include "game.h"

struct Arguments
{
public:
	GameMode mode;
	void parse(int argc, char const *argv[]);

private:
	void error(std::string error);
};

#endif /* ARGUMENT_PARSER_H */

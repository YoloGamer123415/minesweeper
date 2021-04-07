#include "argumentParser.h"
#include "game.h"

int main(int argc, char const *argv[])
{
	Arguments arguments;
	arguments.parse(argc, argv);

	Game game(arguments.mode);

	game.run();

	return 0;
}

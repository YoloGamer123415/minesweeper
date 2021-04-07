#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <ncurses.h>

struct GameMode
{
	std::string name;
	short int size;
};

struct GameCell
{
	static const short int WIDTH = 3;
	static const short int HEIGHT = 1;

	bool isBomb;
	bool isFlagged;
	bool isOpened;
	unsigned short int bombsAround;
	WINDOW *win;
};

class Game
{
	public:
		Game(GameMode difficulty);
		~Game();

		void run();

	private:
		GameMode difficulty;
		int maxWidth, maxHeight;
		int selectedCellX, selectedCellY;

		std::vector< std::vector<GameCell> > field;

		unsigned short int getBombCountAroundCell(short int cellX, short int cellY);
		void fillField();

		void updateCell(short int cellX, short int cellY);
		void updateField();
};

#endif /* GAME_H */

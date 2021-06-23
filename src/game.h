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
	bool isRevealed;
	unsigned short int bombsAround;
};

class Game
{
	public:
		Game(GameMode difficulty);
		~Game();

		void run();

	private:
		GameMode difficulty;
		unsigned short revealedCount;
		const unsigned short toReveal;
		int maxWidth, maxHeight;
		int selectedCellX, selectedCellY;
		WINDOW *window, *infoWin;
		bool playing;

		std::vector< std::vector<GameCell*> > field;

		void gameWon();
		void gameOver();

		void clearInfo();
		void showInfo(const char* message, int color = 0);

		unsigned short int getBombCountAroundCell(short int cellX, short int cellY);
		void fillField();

		void updateCell(short int cellX, short int cellY);
		void updateField();

		void moveCursor(short int dx, short int dy);

		void flagCell();
		void revealCell();
		void revealCell(short int cellX, short int cellY);
};

#endif /* GAME_H */

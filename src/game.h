#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>
#include <ncurses.h>

struct GameSize
{
	unsigned short width;
	unsigned short height;
	unsigned short bombCount;
	std::string name;
};

struct GameCell
{
	static const short int WIDTH = 3;
	static const short int HEIGHT = 1;

	GameCell()
		: isBomb(false)
		, isFlagged(false)
		, isRevealed(false)
		, bombsAround(0)
	{};

	bool isBomb;
	bool isFlagged;
	bool isRevealed;
	unsigned short int bombsAround;
};

class Game
{
	public:
		Game();
		~Game();

		static const std::vector<GameSize> SIZES;

		void run();

	private:
		const GameSize *size;

		unsigned short revealedCount;
		unsigned short toReveal;
		bool quit, playing;

		int maxWidth, maxHeight;
		WINDOW *gameWin, *difficultyWin, *infoWin;
		int selectedCellX, selectedCellY;

		std::vector< std::vector<GameCell*> > field;

		void showDifficultySelector();
		void setup();

		void gameWon();
		void gameOver();

		void clearInfo();
		void showInfo(const char* message, int color = 0);

		unsigned short int getBombCountAroundCell(short int cellX, short int cellY);
		void clearField();
		void fillField();

		void updateCell(short int cellX, short int cellY);
		void updateField();

		void moveCursor(short int dx, short int dy);

		void flagCell();
		void revealCell();
		void revealCell(short int cellX, short int cellY);
};

#endif /* GAME_H */

#include "game.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <ncurses.h>

#define GAME_COLOR_BOMB 1
#define GAME_COLOR_CURSOR 2

// constructor
Game::Game(GameMode difficulty)
{
	this->difficulty = difficulty;

	initscr();
	start_color();
	use_default_colors();
	curs_set(0); // hide the cursor
	cbreak();
	noecho();

	init_pair(GAME_COLOR_BOMB, COLOR_RED, -1);
	init_pair(GAME_COLOR_CURSOR, COLOR_BLACK, COLOR_WHITE);

	getmaxyx(stdscr, this->maxHeight, this->maxWidth);


	// place cursor in the middle of the game
	// this->selectedCellX = (windowWidth / 2) / GameCell::WIDTH;
	// this->selectedCellY = (windowHeight / 2) / GameCell::HEIGHT;
	this->selectedCellX = this->selectedCellY = 0;
}

// deconstructor
Game::~Game()
{
	for (auto &row : this->field)
	{
		for (auto &cell : row)
		{
			delwin(cell.win);
			cell.win = nullptr;
		}
	}

	endwin();
}

// public methods
void Game::run()
{
	bool isRunning = true;

	this->fillField();

	// while (isRunning)
	// {
		this->updateField();
		// char input = getch();
	// }
}

// private methods
unsigned short int Game::getBombCountAroundCell(short int cellX, short int cellY)
{
	unsigned short int count = 0;

	for (short int y = -1; y <= 1; ++y)
	{
		for (short int x = -1; x <= 1; ++x)
		{
			short int fieldX = cellX + x;
			short int fieldY = cellY + y;

			if (
					fieldX >= 0 && fieldX < this->difficulty.size
					&& fieldY >= 0 && fieldY < this->difficulty.size
					&& this->field[fieldY][fieldX].isBomb
			)
			{
				++count;
			}
		}
	}

	return count;
}

void Game::fillField()
{
	// add 2 for the border
	short int windowWidth = this->difficulty.size * (GameCell::WIDTH + 2);
	short int windowHeight = this->difficulty.size * (GameCell::HEIGHT + 2);
	short int xOffset = (this->maxWidth - windowWidth) / 2.0;
	short int yOffset = (this->maxHeight - windowHeight) / 2.0;

	// fill the field with empty cells
	for (short int y = 0; y < this->difficulty.size; ++y)
	{
		std::vector<GameCell> row(this->difficulty.size);

		for (short int x = 0; x < this->difficulty.size; ++x)
		{
			GameCell cell;

			cell.win = newwin(
				GameCell::HEIGHT + 2, GameCell::WIDTH + 2,
				yOffset + (GameCell::HEIGHT + 2) * y,
				xOffset + (GameCell::WIDTH + 2) * x
			);
			box(cell.win, 0, 0);
			wrefresh(cell.win);

			row.push_back(cell);
		}

		this->field.push_back(row);
	}

	// generate `this->difficulty.size` random bomb positions
	std::set<short int> positions;

	while ( positions.size() < this->difficulty.size )
	{
		positions.insert( rand() % (this->difficulty.size * this->difficulty.size) );
	}

	for (const auto &position : positions)
	{
		short int y = position / this->difficulty.size;
		short int x = position % this->difficulty.size;

		this->field[y][x].isBomb = true;
	}

	// set all the bomb counts
	for (short int y = 0; y < this->difficulty.size; ++y)
	{
		for (short int x = 0; x < this->difficulty.size; ++x)
		{
			this->field[y][x].bombsAround = this->getBombCountAroundCell(x, y);
		}
	}
}

void Game::updateCell(short int cellX, short int cellY)
{
	const GameCell cell = this->field[cellY][cellX];
	const unsigned short int bombCount = cell.bombsAround;
	const bool isSelected = cellX == this->selectedCellX
		&& cellY == this->selectedCellY;

	// add '0' to bombCount, to go from number to character
	const char cellChar = cell.isBomb ? 'b' : bombCount + '0';

	if (isSelected)
		wattron( cell.win, COLOR_PAIR(GAME_COLOR_CURSOR) );

	if (cell.isBomb)
		wattron( cell.win, A_BOLD | COLOR_PAIR(GAME_COLOR_BOMB) );

	mvwaddch(
		cell.win,
		GameCell::WIDTH / 2, GameCell::HEIGHT / 2,
		cellChar
	);

	wattroff( cell.win, COLOR_PAIR(GAME_COLOR_CURSOR) );
	wattroff( cell.win, A_BOLD | COLOR_PAIR(GAME_COLOR_BOMB) );

	wrefresh(cell.win);
}

void Game::updateField()
{
	for (short int y = 0; y < this->difficulty.size; ++y)
	{
		for (short int x = 0; x < this->difficulty.size; ++x)
		{
			this->updateCell(x, y);
		}
	}
}

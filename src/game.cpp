#include "game.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <ncurses.h>

#define GAME_COLOR_BG 1
#define GAME_COLOR_BG_DIMMED 2
#define GAME_COLOR_BOMB 3
#define GAME_COLOR_CURSOR 4

// constructor
Game::Game(GameMode difficulty)
{
	this->difficulty = difficulty;

	srand( time(NULL) );

	initscr();
	start_color();
	use_default_colors();
	curs_set(0); // hide the cursor
	cbreak();
	noecho();

	init_pair(GAME_COLOR_BG, -1, -1);
	init_pair(GAME_COLOR_BG_DIMMED, -1, COLOR_BLACK);
	init_pair(GAME_COLOR_BOMB, COLOR_RED, -1);
	init_pair(GAME_COLOR_CURSOR, COLOR_BLACK, COLOR_WHITE);

	getmaxyx(stdscr, this->maxHeight, this->maxWidth);

	short int windowWidth = this->difficulty.size * GameCell::WIDTH;
	short int windowHeight = this->difficulty.size * GameCell::HEIGHT;
	short int xOffset = (this->maxWidth - windowWidth) / 2;
	short int yOffset = (this->maxHeight - windowHeight) / 2;

	this->window = newwin(
		windowHeight, windowWidth,
		yOffset, xOffset
	);
	wrefresh(this->window);

	// place cursor in the middle of the game
	this->selectedCellX = (windowWidth / 2) / GameCell::WIDTH;
	this->selectedCellY = (windowHeight / 2) / GameCell::HEIGHT;
}

// deconstructor
Game::~Game()
{
	delwin(this->window);
	this->window = nullptr;

	endwin();
}

// public methods
void Game::run()
{
	bool isRunning = true;

	this->fillField();
	this->updateField();

	while (isRunning)
	{
		char input = getchar();

		switch (input)
		{
			case 'q':
			{
				isRunning = false;
			} break;

			// cursor movement
			case 'w':
			case 'k':
			{
				this->moveCursor(0, -1); // up (-1 on y-axis)
			} break;
			case 'a':
			case 'h':
			{
				this->moveCursor(-1, 0); // left (-1 on x-axis)
			} break;
			case 's':
			case 'j':
			{
				this->moveCursor(0, 1); // down (+1 on y-axis)
			} break;
			case 'd':
			case 'l':
			{
				this->moveCursor(1, 0); // right (+1 on x-axis)
			} break;

			// flag current cell
			case 'f':
			{
				this->flagCell();
			} break;

			// reveal a cell
			case ' ':
			{
				this->revealCell();
			} break;
		}
	}
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
	// fill the field with empty cells
	for (short int y = 0; y < this->difficulty.size; ++y)
	{
		std::vector<GameCell> row(this->difficulty.size);

		for (short int x = 0; x < this->difficulty.size; ++x)
		{
			GameCell cell;
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
	const GameCell *cell = &this->field[cellY][cellX];
	const unsigned short int bombCount = cell->bombsAround;
	const bool isSelected = cellX == this->selectedCellX
		&& cellY == this->selectedCellY;

	const short int halfCellWidth = GameCell::WIDTH / 2;
	const short int halfCellHeight = GameCell::HEIGHT / 2;
	const short int cellStartX = cellX * GameCell::WIDTH + halfCellWidth;
	const short int cellStartY = cellY * GameCell::HEIGHT + halfCellHeight;

	const int color = (cellY * this->difficulty.size + cellX) % 2 == 1
		? GAME_COLOR_BG
		: GAME_COLOR_BG_DIMMED;
	char cellChar = ' ';

	if (cell->isFlagged) cellChar = '#';
	// add '0' to bombCount, to go from number to character
	else if (cell->isRevealed) cellChar = bombCount == 0 ? '-' : bombCount + '0';

	if (isSelected) wattron( this->window, COLOR_PAIR(GAME_COLOR_CURSOR) );
	else wattron( this->window, COLOR_PAIR(color) );

	for (short int yOffset = -halfCellHeight; yOffset <= halfCellHeight; ++yOffset)
	{
		for (short int xOffset = -halfCellWidth; xOffset <= halfCellWidth; ++xOffset)
		{
			if (xOffset == 0 && yOffset == 0)
			{
				mvwaddch(
					this->window,
					cellStartY + yOffset, cellStartX + xOffset,
					cellChar
				);
			}
			else
			{
				mvwaddch(
					this->window,
					cellStartY + yOffset, cellStartX + xOffset,
					' '
				);
			}
		}
	}

	wattroff( this->window, COLOR_PAIR(GAME_COLOR_CURSOR) );
	wattroff( this->window, COLOR_PAIR(color) );
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

	wrefresh(this->window);
}

void Game::moveCursor(short int dx, short int dy)
{
	const short int oldX = this->selectedCellX;
	const short int oldY = this->selectedCellY;
	const short int newX = oldX + dx;
	const short int newY = oldY + dy;

	if (
		newX >= 0 && newX < this->difficulty.size
		&& newY >= 0 && newY < this->difficulty.size
	)
	{
		this->selectedCellX = newX;
		this->selectedCellY = newY;

		this->updateCell(oldX, oldY);
		this->updateCell(newX, newY);
		wrefresh(this->window);
	}
}

void Game::flagCell()
{
	GameCell *cell = &this->field[this->selectedCellY][this->selectedCellX];

	if (!cell->isRevealed)
	{
		cell->isFlagged = !cell->isFlagged;

		this->updateCell(this->selectedCellX, this->selectedCellY);
		wrefresh(this->window);
	}
}

void Game::revealCell()
{
	this->revealCell(this->selectedCellX, this->selectedCellY);
}

void Game::revealCell(short int cellX, short int cellY)
{
	GameCell *cell = &this->field[cellY][cellX];

	if (!cell->isFlagged && !cell->isRevealed)
	{
		cell->isRevealed = true;

		this->updateCell(cellX, cellY);
		wrefresh(this->window);

		if (cell->bombsAround == 0)
		{
			for (short int yOffset = -1; yOffset <= 1; ++yOffset)
			{
				for (short int xOffset = -1; xOffset <= 1; ++xOffset)
				{
					const short int newX = cellX + xOffset;
					const short int newY = cellY + yOffset;

					if (
						!(xOffset == 0 && yOffset == 0)
						&& newX >= 0 && newX < this->difficulty.size
						&& newY >= 0 && newY < this->difficulty.size
					)
					{
						this->revealCell(newX, newY);
					}
				}
			}
		}
	}
}

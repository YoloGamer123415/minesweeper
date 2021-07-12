#include "game.h"

#include <cstring>
#include <iostream>
#include <algorithm>
#include <set>
#include <ncurses.h>

#define GAME_COLOR_BG 1
#define GAME_COLOR_BG_DIMMED 2
#define GAME_COLOR_FLAGGED 3
#define GAME_COLOR_CURSOR 4
#define GAME_COLOR_GOOD 5
#define GAME_COLOR_BAD 6

const std::vector<GameSize> Game::SIZES = {
	{ 9, 9, 10, "easy" },
	{ 15, 15, 40, "normal" },
	{ 15, 29, 99, "hard" },
};

// constructor
Game::Game()
	: size(NULL)
	, revealedCount(0)
	, toReveal(0)
	, quit(false)
	, playing(false)
{
	srand( time(NULL) );

	initscr();
	start_color();
	use_default_colors();
	curs_set(0); // hide the cursor
	cbreak();
	noecho();

	init_pair(GAME_COLOR_BG, -1, -1);
	init_pair(GAME_COLOR_BG_DIMMED, -1, COLOR_BLACK);
	init_pair(GAME_COLOR_FLAGGED, COLOR_GREEN, -1);
	init_pair(GAME_COLOR_CURSOR, COLOR_BLACK, COLOR_WHITE);
	init_pair(GAME_COLOR_GOOD, COLOR_GREEN, -1);
	init_pair(GAME_COLOR_BAD, COLOR_RED, -1);

	getmaxyx(stdscr, this->maxHeight, this->maxWidth);

}

// deconstructor
Game::~Game()
{
	this->clearField();

	if (this->infoWin != NULL)
	{
		delwin(this->infoWin);
		this->infoWin = nullptr;
	}

	if (this->difficultyWin != NULL)
	{
		delwin(this->difficultyWin);
		this->difficultyWin = nullptr;
	}

	if (this->gameWin != NULL)
	{
		delwin(this->gameWin);
		this->gameWin = nullptr;
	}

	endwin();
}

// public methods
void Game::run()
{
	this->showDifficultySelector();

	int input;

	this->setup();

	while ( !quit )
	{
		input = getchar();

		if (this->playing)
		{
			switch (input)
			{
				// quit
				case 'q':
				{
					this->quit = true;
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
		else if (input == 'r')
		{
			// TODO: Restart game
		}
	}
}

// private methods
void Game::showDifficultySelector()
{
	size_t count = Game::SIZES.size();
	size_t tabSize = this->maxWidth / (count + 1);
	size_t selected = 0;
	int input;

	this->difficultyWin = newwin(0, this->maxWidth, 0, 0);
	wrefresh(this->difficultyWin);

	do
	{
		for (size_t i = 0; i < count; ++i)
		{
			size_t msgWidth = Game::SIZES[i].name.size();
			size_t xOffset = (tabSize - msgWidth) / 2;

			if (selected == i) wattron(this->difficultyWin, A_REVERSE);

			wattron( this->difficultyWin, COLOR_PAIR(GAME_COLOR_BG) );

			for (size_t j = 0; j < tabSize; ++j)
				mvwaddch(this->difficultyWin, 0, tabSize * i + j, ' ');

			mvwaddstr(
				this->difficultyWin,
				0, tabSize * i + xOffset,
				Game::SIZES[i].name.c_str()
			);

			wattroff(this->difficultyWin, A_REVERSE);
			wattroff( this->difficultyWin, COLOR_PAIR(GAME_COLOR_BG) );
		}

		if (selected == count) wattron(this->difficultyWin, A_REVERSE);

		wattron( this->difficultyWin, COLOR_PAIR(GAME_COLOR_BG) );

		for (size_t j = 0; j < tabSize; ++j)
			mvwaddch(this->difficultyWin, 0, tabSize * count + j, ' ');

		mvwaddstr(
			this->difficultyWin,
			0, (tabSize * count) + ( (tabSize - 4) / 2 ),
			"quit"
		);

		wattroff(this->difficultyWin, A_REVERSE);
		wattroff( this->difficultyWin, COLOR_PAIR(GAME_COLOR_BG) );

		wrefresh(this->difficultyWin);

		input = getchar();

		switch (input)
		{
			case 'a':
			case 'h':
			{
				if (selected > 0) --selected;
			} break;
			case 'd':
			case 'l':
			{
				if (selected < count) ++selected;
			} break;
		}
	}
	while( input != ' ' );

	delwin(this->difficultyWin);
	this->difficultyWin = nullptr;

	if (selected > count - 1)
	{
		this->quit = true;
		return;
	}

	this->size = &Game::SIZES[selected];
}

void Game::setup()
{
	short int windowWidth = this->size->width * GameCell::WIDTH;
	short int windowHeight = this->size->height * GameCell::HEIGHT;
	short int xOffset = (this->maxWidth - windowWidth) / 2;
	short int yOffset = (this->maxHeight - windowHeight) / 2;

	this->gameWin = newwin(
		windowHeight, windowWidth,
		yOffset, xOffset
	);
	wrefresh(this->gameWin);

	this->infoWin = newwin(1, windowWidth, yOffset - 2, xOffset);
	wrefresh(this->infoWin);

	// place cursor in the middle of the game
	this->selectedCellX = (windowWidth / 2) / GameCell::WIDTH;
	this->selectedCellY = (windowHeight / 2) / GameCell::HEIGHT;

	this->clearField();
	this->fillField();
	this->updateField();
	this->playing = true;
}

void Game::gameWon()
{
	this->playing = false;
	this->showInfo("You have won!", GAME_COLOR_GOOD);
}

void Game::gameOver()
{
	this->playing = false;
	this->showInfo("You have lost", GAME_COLOR_BAD);
}

void Game::clearInfo()
{
	wclrtoeol(this->infoWin);
}

void Game::showInfo(const char* message, int color)
{
	this->clearInfo();

	unsigned short windowWidth = this->size->width * GameCell::WIDTH;
	unsigned short messageWidth = std::strlen(message);
	unsigned short xOffset = (windowWidth - messageWidth) / 2;

	wattron( this->infoWin, COLOR_PAIR(color) );
	mvwaddstr(this->infoWin, 0, xOffset, message);
	wrefresh(this->infoWin);
}

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
					fieldX >= 0 && fieldX < this->size->width
					&& fieldY >= 0 && fieldY < this->size->height
					&& this->field[fieldY][fieldX]->isBomb
			)
			{
				++count;
			}
		}
	}

	return count;
}

void Game::clearField()
{
	for (unsigned int y = 0; y < this->field.size(); ++y)
	{
		for (unsigned int x = 0; x < this->field[y].size(); ++x)
		{
			delete this->field[y][x];
			this->field[y][x] = nullptr;
		}
	}
}

void Game::fillField()
{
	// generate `this->difficulty.size` random bomb positions
	std::set<short int> positions;

	while ( positions.size() < this->size->bombCount )
	{
		positions.insert( rand() % (this->size->width * this->size->height) );
	}

	// fill the playing field
	for (short int y = 0; y < this->size->height; ++y)
	{
		std::vector<GameCell*> row;

		for (short int x = 0; x < this->size->width; ++x)
		{
			GameCell *cell = new GameCell;
			short int position = y * this->size->height + x;

			if ( positions.find(position) != positions.end() )
				cell->isBomb = true;

			row.push_back(cell);
		}

		this->field.push_back(row);
	}

	// set all the bomb counts
	for (short int y = 0; y < this->size->height; ++y)
	{
		for (short int x = 0; x < this->size->width; ++x)
		{
			this->field[y][x]->bombsAround = this->getBombCountAroundCell(x, y);
		}
	}
}

void Game::updateCell(short int cellX, short int cellY)
{
	const GameCell *cell = this->field[cellY][cellX];
	const unsigned short int bombCount = cell->bombsAround;
	const bool isSelected = cellX == this->selectedCellX
		&& cellY == this->selectedCellY;

	const short int halfCellWidth = GameCell::WIDTH / 2;
	const short int halfCellHeight = GameCell::HEIGHT / 2;
	const short int cellStartX = cellX * GameCell::WIDTH + halfCellWidth;
	const short int cellStartY = cellY * GameCell::HEIGHT + halfCellHeight;

	const int color = (cellY * this->size->width + cellX) & 1
		? GAME_COLOR_BG
		: GAME_COLOR_BG_DIMMED;
	char cellChar = ' ';

	if (cell->isFlagged) cellChar = '#';
	else if (cell->isRevealed && cell->isBomb) cellChar = '%';
	// add '0' to bombCount, to go from number to character
	else if (cell->isRevealed) cellChar = bombCount == 0 ? '-' : bombCount + '0';

	if (isSelected)
		wattron( this->gameWin, A_REVERSE );

	if (cell->isFlagged)
	{
		wattron( this->gameWin, COLOR_PAIR(color) );
		wattron( this->gameWin, COLOR_PAIR(GAME_COLOR_FLAGGED) );
	}
	else
		wattron( this->gameWin, COLOR_PAIR(color) );

	for (short int yOffset = -halfCellHeight; yOffset <= halfCellHeight; ++yOffset)
	{
		for (short int xOffset = -halfCellWidth; xOffset <= halfCellWidth; ++xOffset)
		{
			if (xOffset == 0 && yOffset == 0)
			{
				mvwaddch(
					this->gameWin,
					cellStartY + yOffset, cellStartX + xOffset,
					cellChar
				);
			}
			else
			{
				mvwaddch(
					this->gameWin,
					cellStartY + yOffset, cellStartX + xOffset,
					' '
				);
			}
		}
	}

	wattroff(this->gameWin, A_REVERSE);
	wattroff( this->gameWin, COLOR_PAIR(GAME_COLOR_FLAGGED) );
	wattroff( this->gameWin, COLOR_PAIR(color) );
}

void Game::updateField()
{
	for (short int y = 0; y < this->size->height; ++y)
	{
		for (short int x = 0; x < this->size->width; ++x)
		{
			this->updateCell(x, y);
		}
	}

	wrefresh(this->gameWin);
}

void Game::moveCursor(short int dx, short int dy)
{
	const short int oldX = this->selectedCellX;
	const short int oldY = this->selectedCellY;
	const short int newX = oldX + dx;
	const short int newY = oldY + dy;

	if (
		newX >= 0 && newX < this->size->width
		&& newY >= 0 && newY < this->size->height
	)
	{
		this->selectedCellX = newX;
		this->selectedCellY = newY;

		this->updateCell(oldX, oldY);
		this->updateCell(newX, newY);
		wrefresh(this->gameWin);
	}
}

void Game::flagCell()
{
	GameCell *cell = this->field[this->selectedCellY][this->selectedCellX];

	if (!cell->isRevealed)
	{
		cell->isFlagged = !cell->isFlagged;

		this->updateCell(this->selectedCellX, this->selectedCellY);
		wrefresh(this->gameWin);
	}
}

void Game::revealCell()
{
	this->revealCell(this->selectedCellX, this->selectedCellY);
}

void Game::revealCell(short int cellX, short int cellY)
{
	GameCell *cell = this->field[cellY][cellX];

	if (!cell->isFlagged && !cell->isRevealed)
	{
		cell->isRevealed = true;
		++this->revealedCount;

		this->updateCell(cellX, cellY);
		wrefresh(this->gameWin);

		if (cell->isBomb)
		{
			this->gameOver();
		}
		else if ( this->revealedCount >= this->toReveal )
		{
			this->gameWon();
		}
		else
		{
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
								&& newX >= 0 && newX < this->size->width
								&& newY >= 0 && newY < this->size->height
						   )
						{
							this->revealCell(newX, newY);
						}
					}
				}
			}
		}
	}
}

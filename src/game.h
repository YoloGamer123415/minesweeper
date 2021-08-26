#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>
#include <ncurses.h>

/**
 * A struct with information about the size of the playing field, the amount of
 * bombs in the playing field and the name of the difficulty.
 */
struct GameSize
{
	/// Width in cells of the playing field.
	unsigned short width;
	/// Height in cells of the playing field.
	unsigned short height;
	/// Number of bombs in the playing field.
	unsigned short bombCount;
	/// Name of the difficulty to show in the difficulty selector.
	std::string name;
};

/**
 * A cell placed in the playing field. Holds information about the cell, like
 * how many bombs are around this cell, if the cell is a bomb and if the cell
 * is revealed or flagged.
 */
struct GameCell
{
	/// Width of a cell in spaces.
	static const short int WIDTH = 3;
	/// Height of a cell in spaces.
	static const short int HEIGHT = 1;

	GameCell()
		: isBomb(false)
		, isFlagged(false)
		, isRevealed(false)
		, bombsAround(0)
	{};

	/// Whether or not this cell is a bomb.
	bool isBomb;
	/// Whether or not this cell is flagged.
	bool isFlagged;
	/// Whether or not this cell is revealed.
	bool isRevealed;
	/// The amount of bombs around this cell.
	unsigned short int bombsAround;
};

/**
 * The main class.
 */
class Game
{
	public:
		Game();
		~Game();

		/// All the available sizes.
		static const std::vector<GameSize> SIZES;

		/**
		 * 
		 */
		void run();

	private:
		/// The current selected game size (difficulty).
		const GameSize *size;

		/// The amount of revealed cells.
		unsigned short revealedCount;
		/// The amount of cells that the user has to reveal to win the game.
		unsigned short toReveal;
		/// If `true`, we should quit the program.
		bool quit;
		/// If `false`, the player has won or lost a game.
		bool playing;
		/// Whether or not this is the first move of the game
		bool firstMove;

		/// The width of the terminal window in characters.
		int maxWidth;
		/// The height of the terminal window in characters.
		int maxHeight;
		/// The window the difficulty selector is placed.
		WINDOW *difficultyWin;
		/// The window the `GameCell`s are drawn in.
		WINDOW *gameWin;
		/// The window a small informational text can be placed.
		WINDOW *infoWin;
		/// The selected x position in the playing `field`.
		int selectedCellX;
		/// The selected y position in the playing `field`.
		int selectedCellY;

		/// The playing field.
		std::vector< std::vector<GameCell*> > field;

		/**
		 * Shows a difficulty selector at the top of the screen. After the user
		 * has selected a difficulty, it gets stored in `Game::size`.
		 */
		void showDifficultySelector();
		/**
		 * Initializes `Game::gameWin` and `Game::infoWin`. Also resets the
		 * cursor position, the revealed cell count and updates the playing
		 * field.
		 */
		void setup();
		/**
		 * Clears the `field` and deletes all the windows.
		 */
		void clean();

		/**
		 * Shows the message "You have won!" in green above the playing field.
		 */
		void gameWon();
		/**
		 * Shows the message "You have lost" in red above the playing field.
		 */
		void gameOver();

		/**
		 * Clears the `infoWin`.
		 */
		void clearInfo();
		/**
		 * Shows `message` in the color `color` above the playing field.
		 *
		 * @param message - The message string to show.
		 * @param color - The ncurses color pair number for to use for the
		 *                text color.
		 */
		void showInfo(const char* message, int color = 0);

		/**
		 * Counts how many bombs are around the `GameCell` on position
		 * (`cellX`, `cellY`).
		 *
		 * @param cellX, cellY - The coordinates of the `GameCell`.
		 * @returns The amount of bombs around the `GameCell`.
		 */
		unsigned short int getBombCountAroundCell(short int cellX, short int cellY);
		/**
		 * Clears the playing `field`. Deletes all the `GameCell`s in the
		 * `field`.
		 */
		void clearField();
		/**
		 * Create the field in memory for drawing, but don't assign bombs to
		 * locations yet (you can use `Game::fillField(short, short)` for that)
		 */
		void createField();
		/**
		 * Mark random locations in the `field` as bombs. Also mark how many
		 * bombs are around every cell
		 */
		void fillField(short startX, short startY);

		/**
		 * Redraws a given `GameCell`.
		 *
		 * @param cellX, cellY - The coordinates of the `GameCell`.
		 */
		void updateCell(short int cellX, short int cellY);
		/**
		 * Redraws the entire game `field`.
		 */
		void updateField();

		/**
		 * Moves the cursor to.
		 *
		 * @param dx - Delta x. Adds `x` to `Game::selectedCellX` to get the
		 *             new x position.
		 * @param dy - Delta y. Adds `y` to `Game::selectedCellY` to get the
		 *             new y position.
		 */
		void moveCursor(short int dx, short int dy);

		/**
		 * Marks the currently selected `GameCell` as flagged if it hasn't been
		 * revealed yet. If this `GameCell` already is flagged, it is the flag
		 * is removed.
		 */
		void flagCell();
		/**
		 * Calls `Game::revealCell(short, short)` with the currently selected
		 * `GameCell` position.
		 */
		void revealCell();
		/**
		 * Reveals the `GameCell` at (`cellX`, `cellY`) if it hasn't been
		 * revealed or flagged. It also checks if the player has won or lost
		 * the game. If both are not the case and there are 0 bombs around the
		 * revealed `GameCell`, the neighbouring `GameCell`s are also revealed.
		 *
		 * @param cellX, cellY - The coordinates of the `GameCell` to reveal.
		 */
		void revealCell(short int cellX, short int cellY);
};

#endif /* GAME_H */

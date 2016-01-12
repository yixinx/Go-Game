//TODO: what does a string mean?
//TODO: how does this code analyze liberty
//TODO: write the code for MonteCarlo

#include "gtp.h"
#include "new_board.h"
#include <cstdio>
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <process.h>
#include <windows.h>
#include <iostream>
#include <shellapi.h>
#include <math.h>
#include "game2.h"
/* Board represented by a 1D array. The first board_size*board_size
* elements are used. Vertices are indexed row by row, starting with 0
* in the upper left corner.
*/
//int main_board[BOARD_SIZE * BOARD_SIZE];
/* the pointers to the block list for each point on the board*/
//block * all_blocks[BOARD_SIZE*BOARD_SIZE];
/* the list for all the blocks */
//std::list<block> block_list;



/* Stones are linked together in a circular list for each string. */
//static int next_stone[BOARD_SIZE * BOARD_SIZE];

/* Storage for final status computations. */
//static int final_status[BOARD_SIZE * BOARD_SIZE];

/* Point which would be an illegal ko recapture. */
//static int ko_i, ko_j;

/* Offsets for the four directly adjacent neighbors. Used for looping. */


/* Macros to convert between 1D and 2D coordinates. The 2D coordinate
* (i, j) points to row i and column j, starting with (0,0) in the
* upper left corner.
*/


#include <list>

using namespace std;

simple_board_c::simple_board_c()
{
	komi = 0;
	board_size = BOARD_SIZE;
	memset(main_board, 0, sizeof(main_board));
	memset(next_stone, 0, sizeof(next_stone));
	memset(final_status, 0, sizeof(final_status));
}

simple_board_c::~simple_board_c()
{}

void
simple_board_c::init_brown(int size)
{
	//int board[BOARD_SIZE*BOARD_SIZE]
	int k;
	int i, j;

	/* The GTP specification leaves the initial board configuration as
	* well as the board configuration after a boardsize command to the
	* discretion of the engine. We choose to start with up to 20 random
	* stones on the board.
	*/
	board_size = size;
	clear_board();
	for (k = 0; k < 20; k++) {
		int color = rand() % 2 ? BLACK : WHITE;
		generate_move(&i, &j, color, (unsigned long)time(NULL));
		play_move(i, j, color);
	}
}

void
simple_board_c::clear_board()
{
	memset(main_board, 0, sizeof(main_board));
}

int
simple_board_c::board_empty()
{
	int i;
	for (i = 0; i < board_size * board_size; i++)
		if (main_board[i] != EMPTY)
			return 0;
	return 1;
}

int
simple_board_c::get_board(int i, int j)
{
	return main_board[i * board_size + j];
}

/* Get the stones of a string. stonei and stonej must point to arrays
* sufficiently large to hold any string on the board. The number of
* stones in the string is returned.
*/
int
simple_board_c::get_string(int i, int j, int *stonei, int *stonej)
{
	int num_stones = 0;
	int pos = POS(i, j);
	do {
		stonei[num_stones] = I(pos);
		stonej[num_stones] = J(pos);
		num_stones++;
		pos = next_stone[pos];
	} while (pos != POS(i, j));

	return num_stones;
}

int
simple_board_c::pass_move(int i, int j)
{
	return i == -1 && j == -1;
}

int
simple_board_c::on_board(int i, int j)
{
	return i >= 0 && i < board_size && j >= 0 && j < board_size;
}

int
simple_board_c::legal_move(int i, int j, int color)
{
	int other = OTHER_COLOR(color);

	/* Pass is always legal. */
	if (pass_move(i, j))
		return 1;

	/* Already occupied. */
	if (get_board(i, j) != EMPTY)
		return 0;

	/* Illegal ko recapture. It is not illegal to fill the ko so we must
	* check the color of at least one neighbor.
	*/
	if (i == ko_i && j == ko_j
		&& ((on_board(i - 1, j) && get_board(i - 1, j) == other)
		|| (on_board(i + 1, j) && get_board(i + 1, j) == other)))
		return 0;

	return 1;
}

/* Does the string at (i, j) have any more liberty than the one at
* (libi, libj)?
*/
int
simple_board_c::has_additional_liberty(int i, int j, int libi, int libj)
{
	int pos = POS(i, j);
	do {
		int ai = I(pos);
		int aj = J(pos);
		int k;
		for (k = 0; k < 4; k++) {
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY
				&& (bi != libi || bj != libj))
				return 1;
		}

		pos = next_stone[pos];
	} while (pos != POS(i, j));

	return 0;
}

/* Does (ai, aj) provide a liberty for a stone at (i, j)? */
int
simple_board_c::provides_liberty(int ai, int aj, int i, int j, int color)
{
	/* A vertex off the board does not provide a liberty. */
	if (!on_board(ai, aj))
		return 0;

	/* An empty vertex IS a liberty. */
	if (get_board(ai, aj) == EMPTY)
		return 1;

	/* A friendly string provides a liberty to (i, j) if it currently
	* has more liberties than the one at (i, j).
	*/
	if (get_board(ai, aj) == color)
		return has_additional_liberty(ai, aj, i, j);

	/* An unfriendly string provides a liberty if and only if it is
	* captured, i.e. if it currently only has the liberty at (i, j).
	*/
	return !has_additional_liberty(ai, aj, i, j);
}

/* Is a move at (i, j) suicide for color? */
int
simple_board_c::suicide(int i, int j, int color)
{
	int k;
	for (k = 0; k < 4; k++)
		if (provides_liberty(i + deltai[k], j + deltaj[k], i, j, color))
			return 0;

	return 1;
}

/* Remove a string from the board array. There is no need to modify
* the next_stone array since this only matters where there are
* stones present and the entire string is removed.
*/
int
simple_board_c::remove_string(int i, int j)
{
	int pos = POS(i, j);
	int removed = 0;
	do {
		main_board[pos] = EMPTY;
		removed++;
		pos = next_stone[pos];
	} while (pos != POS(i, j));

	return removed;
}

/* Do two vertices belong to the same string. It is required that both
* pos1 and pos2 point to vertices with stones.
*/
int
simple_board_c::same_string(int pos1, int pos2)
{
	int pos = pos1;
	do {
		if (pos == pos2)
			return 1;
		pos = next_stone[pos];
	} while (pos != pos1);

	return 0;
}

/* Play at (i, j) for color. No legality check is done here. We need
* to properly update the board array, the next_stone array, and the
* ko point.
*/
void simple_board_c::play_move(int i, int j, int color)
{
	int pos = POS(i, j);
	int captured_stones = 0;
	int k;

	/* Reset the ko point. */
	ko_i = -1;
	ko_j = -1;

	/* Nothing more happens if the move was a pass. */
	if (pass_move(i, j))
		return;

	/* If the move is a suicide we only need to remove the adjacent
	* friendly stones.
	*/
	if (suicide(i, j, color)) {
		for (k = 0; k < 4; k++) {
			int ai = i + deltai[k];
			int aj = j + deltaj[k];
			if (on_board(ai, aj)
				&& get_board(ai, aj) == color)
				remove_string(ai, aj);
		}
		return;
	}

	/* Not suicide. Remove captured opponent strings. */
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (on_board(ai, aj)
			&& get_board(ai, aj) == OTHER_COLOR(color)
			&& !has_additional_liberty(ai, aj, i, j))
			captured_stones += remove_string(ai, aj);
	}

	/* Put down the new stone. Initially build a single stone string by
	* setting next_stone[pos] pointing to itself.
	*/
	main_board[pos] = color;
	next_stone[pos] = pos;

	/* If we have friendly neighbor strings we need to link the strings
	* together.
	*/
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);
		/* Make sure that the stones are not already linked together. This
		* may happen if the same string neighbors the new stone in more
		* than one direction.
		*/
		if (on_board(ai, aj) && main_board[pos2] == color && !same_string(pos, pos2)) {
			/* The strings are linked together simply by swapping the the
			* next_stone pointers.
			*/
			int tmp = next_stone[pos2];
			next_stone[pos2] = next_stone[pos];
			next_stone[pos] = tmp;
		}
	}

	/* If we have captured exactly one stone and the new string is a
	* single stone it may have been a ko capture.
	*/
	if (captured_stones == 1 && next_stone[pos] == pos) {
		int ai, aj;
		/* Check whether the new string has exactly one liberty. If so it
		* would be an illegal ko capture to play there immediately. We
		* know that there must be a liberty immediately adjacent to the
		* new stone since we captured one stone.
		*/
		for (k = 0; k < 4; k++) {
			ai = i + deltai[k];
			aj = j + deltaj[k];
			if (on_board(ai, aj) && get_board(ai, aj) == EMPTY)
				break;
		}

		if (!has_additional_liberty(i, j, ai, aj)) {
			ko_i = ai;
			ko_j = aj;
		}
	}
}

/* get an array of available moves*/
void simple_board_c::get_available_moves(int color, int &num_moves, int(&moves)[BOARD_SIZE * BOARD_SIZE])
{
	int ai, aj;
	int k;
	for (ai = 0; ai < board_size; ai++)
		for (aj = 0; aj < board_size; aj++) {
			/* Consider moving at (ai, aj) if it is legal and not suicide. */
			if (legal_move(ai, aj, color)
				&& !suicide(ai, aj, color)) {
				/* Further require the move not to be suicide for the opponent... */
				if (!suicide(ai, aj, OTHER_COLOR(color)))
					moves[num_moves++] = POS(ai, aj);
				else {
					/* ...however, if the move captures at least one stone,
					* consider it anyway.
					*/
					for (k = 0; k < 4; k++) {
						int bi = ai + deltai[k];
						int bj = aj + deltaj[k];
						if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color)) {
							moves[num_moves++] = POS(ai, aj);
							break;
						}
					}
				}
			}
		}
}

void simple_board_c::evalue_MC(MC_msg_t *MC_msg, int color, int length)
{
	int wins_max = 0;
	unsigned long sd = (unsigned long)time(NULL);
	srand(sd);
	int *threads_move = new int[length];
	msgpasser_t *msg = new msgpasser_t[length];
	HANDLE *flag = new HANDLE[length];
	if (length > 0) {
		int t = 0;
		for (t = 0; t < length; t++)
		{
			msg[t].i = MC_msg[t].i;
			msg[t].j = MC_msg[t].j;
			msg[t].color = color;
			msg[t].wins = 0;
			msg[t].seed = rand() % 77 + 31;
			msg[t].board_here = this;
		}
		//printf("CHECK ::%d::\n", t);
		for (int ii = 0; ii < length; ++ii)
		{
			//printf("%c%d, \n", 65 + msg[ii].j, msg[ii].board_here->board_size - msg[ii].i);
			flag[ii] = (HANDLE)_beginthread(MtC, 0, &(msg[ii]));
		}
		//gtp_mprintf(" ###### \n");
		WaitForMultipleObjects(length, flag, true, INFINITE);
		for (int t = 0; t < length; t++)
		{
			MC_msg[t].wins = msg[t].wins;
			//printf(" MAAAAAAAAAAAX \n");
			//printf("cur max tester has pos j+1 = %c, size-j = %d, wins = %d,,, \n", 65 + (char)msg[t].j, msg[t].board_here->board_size - msg[t].i, msg[t].wins);
		}
	}
	else {
		/* But pass if no move was considered. */

	}
	delete[] msg;
	delete[] flag;
	delete[] threads_move;
}

/* Generate a move. */
void simple_board_c::generate_move_out(int *i, int *j, int color)
{
	int moves[BOARD_SIZE * BOARD_SIZE];
	int num_moves = 0;
	int wins_max = 0;
	unsigned long sd = (unsigned long)time(NULL); //1419834385; 9*9 //E7 F5 B4 C2 H8
	srand(sd);
	printf("master seed: %u ## \n", sd);
	static int count_r = 0;
	//initial randomly
	if (count_r < this->board_size * 0.6)
	{
		sd = generate_move(i, j, color, sd);
		count_r++;
		return;
	}

	memset(moves, 0, sizeof(moves));
	get_available_moves(color, num_moves, moves);
	int *threads_move = new int[min(num_moves, NUM_THREADS)];
	msgpasser_t *msg = new msgpasser_t[min(num_moves, NUM_THREADS)];
	HANDLE *flag = new HANDLE[min(num_moves, NUM_THREADS)];
	if (num_moves > 0) {
		int t = 0;
		for (t = 0; t < min(num_moves, NUM_THREADS); t++)
		{
			threads_move[t] = moves[rand() % num_moves];
		}
		for (t = 0; t < min(num_moves, NUM_THREADS); t++)
		{
			msg[t].i = I(threads_move[t]);
			msg[t].j = J(threads_move[t]);
			msg[t].color = color;
			msg[t].wins = 0;
			msg[t].seed = rand() % 77 + 31;
			msg[t].board_here = this;
		}
		//printf("CHECK ::%d::\n", t);
		for (int ii = 0; ii < min(num_moves, NUM_THREADS); ++ii)
		{
			//printf("%c%d, \n", 65 + msg[ii].j, msg[ii].board_here->board_size - msg[ii].i);
			flag[ii] = (HANDLE)_beginthread(MtC, 0, &(msg[ii]));
		}
		//gtp_mprintf(" ###### \n");
		WaitForMultipleObjects(min(num_moves, NUM_THREADS), flag, true, INFINITE);
		for (t = 0; t < min(num_moves, NUM_THREADS); t++)
		{
			if (wins_max <= msg[t].wins)
			{
				wins_max = msg[t].wins;
				*i = msg[t].i;
				*j = msg[t].j;
				gtp_mprintf(" MAAAAAAAAAAAX \n");
			}
			printf("cur max tester has pos j+1 = %c, size-j = %d, wins = %d,,, \n", 65 + (char)msg[t].j, msg[t].board_here->board_size - msg[t].i, msg[t].wins);
		}
	}
	else {
		/* But pass if no move was considered. */
		*i = -1;
		*j = -1;
	}
	delete[] msg;
	delete[] flag;
	delete[] threads_move;
}

unsigned long simple_board_c::generate_move(int *i, int *j, int color, unsigned long last_rand)
{
	int moves[BOARD_SIZE * BOARD_SIZE];
	int num_moves = 0;
	int move;
	unsigned long next_seed = (695194069 * last_rand + 7746251 + rand()) % 42977424967296;
	memset(moves, 0, sizeof(moves));
	get_available_moves(color, num_moves, moves);

	/* Choose one of the considered moves randomly with uniform
	* distribution. (Strictly speaking the moves with smaller 1D
	* coordinates tend to have a very slightly higher probability to be
	* chosen, but for all practical purposes we get a uniform
	* distribution.)
	*/
	if (num_moves > 0) {
		move = moves[next_seed % num_moves];
		*i = I(move);
		*j = J(move);
	}
	else {
		/* But pass if no move was considered. */
		*i = -1;
		*j = -1;
	}
	return next_seed;
}

/* Set a final status value for an entire string. */
void
simple_board_c::set_final_status_string(int pos, int status, int * out_final_arr = NULL)
{
	int pos2 = pos;
	if (out_final_arr == NULL)
	{
		do {
		final_status[pos2] = status;
		pos2 = next_stone[pos2];
		} while (pos2 != pos);
	}
	else
	{
		do {
		out_final_arr[pos2] = status;
		pos2 = next_stone[pos2];
		} while (pos2 != pos);
	}
}

/* Compute final status. This function is only valid to call in a
* position where generate_move() would return pass for at least one
* color.
*
* Due to the nature of the move generation algorithm, the final
* status of stones can be determined by a very simple algorithm:
*
* 1. Stones with two or more liberties are alive with territory.
* 2. Stones in atari are dead.
*
* Moreover alive stones are unconditionally alive even if the
* opponent is allowed an arbitrary number of consecutive moves.
* Similarly dead stones cannot be brought alive even by an arbitrary
* number of consecutive moves.
*
* Seki is not an option. The move generation algorithm would never
* leave a seki on the board.
*
* Comment: This algorithm doesn't work properly if the game ends with
*          an unfilled ko. If three passes are required for game end,
*          that will not happen.
*/
void
simple_board_c::compute_final_status(void)
{
	int i, j;
	int pos;
	int k;

	for (pos = 0; pos < board_size * board_size; pos++)
		final_status[pos] = UNKNOWN;

	for (i = 0; i < board_size; i++)
		for (j = 0; j < board_size; j++)
			if (get_board(i, j) == EMPTY)
				for (k = 0; k < 4; k++) {
					int ai = i + deltai[k];
					int aj = j + deltaj[k];
					if (!on_board(ai, aj))
						continue;
					/* When the game is finished, we know for sure that (ai, aj)
					* contains a stone. The move generation algorithm would
					* never leave two adjacent empty vertices. Check the number
					* of liberties to decide its status, unless it's known
					* already.
					*
					* If we should be called in a non-final position, just make
					* sure we don't call set_final_status_string() on an empty
					* vertex.
					*/
					pos = POS(ai, aj);
					if (final_status[pos] == UNKNOWN) {
						if (get_board(ai, aj) != EMPTY) {
							if (has_additional_liberty(ai, aj, i, j))
								set_final_status_string(pos, ALIVE);
							else
								set_final_status_string(pos, DEAD);
						}
					}
					/* Set the final status of the (i, j) vertex to either black
					* or white territory.
					*/
					if (final_status[POS(i, j)] == UNKNOWN) {
						if ((final_status[pos] == ALIVE) ^ (get_board(ai, aj) == WHITE))
							final_status[POS(i, j)] = BLACK_TERRITORY;
						else
							final_status[POS(i, j)] = WHITE_TERRITORY;
					}
				}
}

int
simple_board_c::get_final_status(int i, int j)
{
	return final_status[POS(i, j)];
}

void
simple_board_c::set_final_status(int i, int j, int status)
{
	final_status[POS(i, j)] = status;
}

/* Valid number of stones for fixed placement handicaps. These are
* compatible with the GTP fixed handicap placement rules.
*/
int
simple_board_c::valid_fixed_handicap(int handicap)
{
	if (handicap < 2 || handicap > 9)
		return 0;
	if (board_size % 2 == 0 && handicap > 4)
		return 0;
	if (board_size == 7 && handicap > 4)
		return 0;
	if (board_size < 7 && handicap > 0)
		return 0;

	return 1;
}

/* Put fixed placement handicap stones on the board. The placement is
* compatible with the GTP fixed handicap placement rules.
*/
void
simple_board_c::place_fixed_handicap(int handicap)
{
	int low = board_size >= 13 ? 3 : 2;
	int mid = board_size / 2;
	int high = board_size - 1 - low;

	if (handicap >= 2) {
		play_move(high, low, BLACK);   /* bottom left corner */
		play_move(low, high, BLACK);   /* top right corner */
	}

	if (handicap >= 3)
		play_move(low, low, BLACK);    /* top left corner */

	if (handicap >= 4)
		play_move(high, high, BLACK);  /* bottom right corner */

	if (handicap >= 5 && handicap % 2 == 1)
		play_move(mid, mid, BLACK);    /* tengen */

	if (handicap >= 6) {
		play_move(mid, low, BLACK);    /* left edge */
		play_move(mid, high, BLACK);   /* right edge */
	}

	if (handicap >= 8) {
		play_move(low, mid, BLACK);    /* top edge */
		play_move(high, mid, BLACK);   /* bottom edge */
	}
}

/* Put free placement handicap stones on the board. We do this simply
* by generating successive black moves.
*/
void
simple_board_c::place_free_handicap(int handicap)
{
	int k;
	int i, j;

	for (k = 0; k < handicap; k++) {
		generate_move(&i, &j, BLACK, (unsigned long)time(NULL));
		play_move(i, j, BLACK);
	}
}


/*
* Local Variables:
* tab-width: 8
* c-basic-offset: 2
* End:
*/


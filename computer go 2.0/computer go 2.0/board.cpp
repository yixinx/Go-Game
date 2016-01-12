//TODO: what does a string mean?
//TODO: how does this code analyze liberty
//TODO: write the code for MonteCarlo


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

using namespace std;

/* Board represented by a 1D array. The first board_size*board_size
* elements are used. Vertices are indexed row by row, starting with 0
* in the upper left corner.
*/
//int main_board[BOARD_SIZE * BOARD_SIZE];
/* the pointers to the block list for each point on the board*/
//block * all_blocks[BOARD_SIZE*BOARD_SIZE];
/* the list for all the blocks */
//std::list<block> block_list;

int deltai[4] = {-1,1,0,0};
int deltaj[4] = {0,0,-1,1};

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



void
board_t::clear_board()
{
	list<block *>::iterator iter = blocks.begin();
	while (iter != blocks.end())
	{
		delete *(iter++);
	}
	ko_i = -1;
	ko_j = -1;
}

int
board_t::board_empty()
{
	int i;
	for (i = 0; i < board_size * board_size; i++)
		// TODO: check this by looking at if main_board[i] is NULL
		if (blocks.empty())
			return 1;
	return 0;
}

stone * 
board_t::get_board(int i, int j)
{
	return main_board[i * board_size + j];
}

/* Get the stones of a string. stonei and stonej must point to arrays
* sufficiently large to hold any string on the board. 
*/
chain *   board_t::get_chain(int i, int j)
{
	int pos = POS(i, j);
	return main_board[pos]->stone_chain;
}


int
board_t::pass_move(int i, int j)
{
	return i == -1 && j == -1;
}

int
board_t::on_board(int i, int j)
{
	return i >= 0 && i < board_size && j >= 0 && j < board_size;
}

/* decide if there is some stone already occupying the place or illegal ko capture */
int
board_t::legal_move(int i, int j, int color)
{
	int other = OTHER_COLOR(color);

	/* Pass is always legal. */
	if (pass_move(i, j))
		return 1;

	/* Already occupied. */
	/* */
	if (get_board(i, j)->color != 0)
		return 0;

	/* Illegal ko recapture. It is not illegal to fill the ko so we must
	* check the color of at least one neighbor.
	*/
	if (i == ko_i && j == ko_j
		&& ((on_board(i - 1, j) && get_board(i - 1, j)->color == other)
		|| (on_board(i + 1, j) && get_board(i + 1, j)->color == other)))
		return 0;

	return 1;
}

/* Does the string at (i, j) have any more liberty than the one at
* (libi, libj)?
* return 1 if there are
*/
int
board_t::has_additional_liberty(int i, int j, int libi, int libj)
{
	int pos = POS(i, j);
	int num_of_extra_liberties = 0;
    chain * curr_chain =  main_board[pos]->stone_chain;
	list<stone *>::iterator iter = curr_chain->stones.begin();

	do {
		for (int k = 0; k < 4; k++) {
			int bi = (*iter)->row + deltai[k];
			int bj = (*iter)->col + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj)->color == EMPTY
				&& (bi != libi || bj != libj))
					return 1;
		}

		
		if (++iter == curr_chain->stones.end()) break;
	} while (1);

	return 0;
}

/* Does (ai, aj) provide a liberty for a stone at (i, j)? */
int
board_t::provides_liberty(int ai, int aj, int i, int j, int color)
{
	/* A vertex off the board does not provide a liberty. */
	if (!on_board(ai, aj))
		return 0;

	/* An empty vertex IS a liberty. */
	if (get_board(ai, aj)->color == EMPTY)
		return 1;

	/* A friendly string provides a liberty to (i, j) if it currently
	* has more liberties than the one at (i, j).
	*/
	if (get_board(ai, aj)->color == color)
		return has_additional_liberty(ai, aj, i, j);

	/* An unfriendly string provides a liberty if and only if it is
	* captured, i.e. if it currently only has the liberty at (i, j).
	*/
	return !has_additional_liberty(ai, aj, i, j);
}

/* Is a move at (i, j) suicide for color? */
int
board_t::suicide(int i, int j, int color)
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
board_t::remove_chain(int i, int j)
{		
	delete (get_chain(i, j));
	return 0;
	
}



/* Do two vertices belong to the same string. It is required that both
* pos1 and pos2 point to vertices with stones.
*/
// TODO: change to chain type
int
board_t::same_chain(int pos1, int pos2)
{
	int pos = pos1;
	return main_board[pos1]->stone_chain == main_board[pos2]->stone_chain ;
}

/* Play at (i, j) for color. No legality check is done here. We need
* to properly update the board array, the next_stone array, and the
* ko point.



  TODO: place at (i,j)
		if some chains dead
			delete dead ones
		add chain
		add block
		update h()
		update empty ones
*/
void board_t::evaluating_influence(){
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
	{
		main_board[i]->s_influence = 0;
	}
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
	{
		if (main_board[i]->color != 0)
			main_board[i]->stone_influence();//逐一更新每个点对周围的influence，只有空点会有influence，有子点不会有
	}
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
	{
		if (main_board[i]->s_influence > INFLUENCE_UPPER_BOUND)
			main_board[i]->s_influence = INFLUENCE_UPPER_BOUND;//设定影响力上界
		else if (main_board[i]->s_influence < -INFLUENCE_UPPER_BOUND)
			main_board[i]->s_influence = -INFLUENCE_UPPER_BOUND;//设定影响力上界
	}
}
void board_t::play_move(int i, int j, int color)
{
	//printf("\n--------------------------------------------------------\ncurrent play move: %d, %d, %d-----------------------------------------------------------\n", i, j,color);
	int pos = POS(i, j);
	chain * captured_chain = NULL;
	int k = 0;
	int captured_length = 0, captured_pos = -1;

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
			if (on_board(ai, aj) && get_board(ai, aj)->color == color){
				captured_pos = POS(ai, aj);
				captured_chain = main_board[captured_pos]->stone_chain;
				block * temp = main_board[captured_pos]->stone_block;
				delete captured_chain;
				if (temp->chains.size() == 0)
					delete temp;
				return;
			}
		}
	}
	/* place the stone */
	main_board[pos]->color = color;
	main_board[pos]->type = EMPTY_P;
	if (main_board[pos]->stone_block != NULL){
		main_board[pos]->stone_block->empty.remove(main_board[pos]);
	}

	main_board[pos]->add_to_chain(*this);
	/* Not suicide. Remove captured opponent strings. */

	/*********吃子*********/
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (on_board(ai, aj)
			&& get_board(ai, aj)->color == OTHER_COLOR(get_board(i, j)->color)
			// (i,j) provide liberty for (ai,aj)
			&& !has_additional_liberty(ai, aj, i, j))
		{

			captured_pos = POS(ai, aj);
			captured_chain = main_board[captured_pos]->stone_chain;
			captured_length = captured_chain->length;
			block * temp = main_board[captured_pos]->stone_block;
			// TODO: delete the chain and the block
			delete captured_chain;
			if (temp->chains.size() == 0)
				delete temp;
		}
	}
	evaluating_influence();//影响力重新计算
	explore_surrounding_block(main_board[pos]);//与己方棋块融合
	decompose_opponent_blocks(this, main_board[pos]);//截断对方棋块
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)//空点重新归属
	{
		if (main_board[i]->color == 0)
			main_board[i]->empty_redistribution();
	}
	/*计算每个棋块的影响力和全局影响力*/
	for (list<block*>::iterator iter = blocks.begin(); iter != blocks.end(); iter++){
		(*iter)->influence = 0;
	}
	total_black_influence = 0;
	total_white_influence = 0;
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
	{
		if (main_board[i]->color == 0 && main_board[i]->stone_block != NULL){
			main_board[i]->stone_block->influence += main_board[i]->s_influence;
			if (main_board[i]->stone_block->color == BLACK){
				total_black_influence += main_board[i]->s_influence;
			}
			else
				total_white_influence += main_board[i]->s_influence;
		}
	}

	/*调用评估函数评价每个block*/
	int black_live_eye_arr[BOARD_SIZE*BOARD_SIZE] = { 0 };
	int white_live_eye_arr[BOARD_SIZE*BOARD_SIZE] = { 0 };
	for (list<block*>::iterator iter = blocks.begin(); iter != blocks.end(); iter++)
	{
		(*iter)->evaluation(black_live_eye_arr, white_live_eye_arr);
	}
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
		if (black_live_eye_arr[i] == 2 || white_live_eye_arr[i] == 2) 
			main_board[i]->type = LIVE_EYE;
	}
	/*print_all_block_or_chain(0);
	print_all_block_or_chain(1);
	print_all_block_or_chain(2);
	print_all_block_or_chain(3);*/
	/* reset the ko location: If we have captured exactly one stone and the new string is a
	* single stone it may have been a ko capture.
	*/
	if (captured_length == 1 && main_board[pos]->stone_chain->length == 1) {
		int ai, aj;
		/* Check whether the new string has exactly one liberty. If so it
		* would be an illegal ko capture to play there immediately. We
		* know that there must be a liberty immediately adjacent to the
		* new stone since we captured one stone.
		*/
		for (k = 0; k < 4; k++) {
			ai = i + deltai[k];
			aj = j + deltaj[k];
			if (on_board(ai, aj) && get_board(ai, aj)->color == EMPTY)
				break;
		}
		if (!has_additional_liberty(i, j, ai, aj))
		{
			ko_i = ai;
			ko_j = aj;
		}
	}
}


/* get an array of available moves*/
void board_t::get_available_moves(int color, int &num_moves, int(&moves)[BOARD_SIZE * BOARD_SIZE])
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
						if (on_board(bi, bj) && get_board(bi, bj)->color == OTHER_COLOR(color)) {
							moves[num_moves++] = POS(ai, aj);
							break;
						}
					}
				}
			}
		}
}

unsigned long board_t::generate_move(int *i, int *j, int color, unsigned long last_rand)
{
	int moves[BOARD_SIZE * BOARD_SIZE];
	int num_moves = 0;
	int move;
	unsigned long next_seed = (695194069 * last_rand + 7746251) % 42977424967296;
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

/* Set a final status value for an entire chain. */
void
board_t::set_final_status_string(int pos, int status, int * out_final_arr = NULL)
{
	int pos2 = pos;
	if (out_final_arr == NULL)
	{
		/*
		do {
		final_status[pos2] = status;
		pos2 = next_stone[pos2];
		} while (pos2 != pos);
		*/
	}
	else
	{
	
		chain * curr_chain = get_chain(I(pos), J(pos));
		list<stone *>::iterator iter = curr_chain->stones.begin();
		do {
			out_final_arr[pos2] = status;
			iter++;
			pos2 = POS((*iter)->row, (*iter)->col);
		} while (pos2 != pos);
	}
}

void
board_t::compute_final_status(int * out_final_arr)
{
	int i, j;
	int pos;
	int k;

	for (pos = 0; pos < board_size * board_size; pos++)
		out_final_arr[pos] = UNKNOWN;

	for (i = 0; i < board_size; i++)
		for (j = 0; j < board_size; j++)
			if (get_board(i, j)->color == EMPTY)
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
					if (out_final_arr[pos] == UNKNOWN) {
						if (get_board(ai, aj)->color != EMPTY) {
							if (has_additional_liberty(ai, aj, i, j))
								set_final_status_string(pos, ALIVE, out_final_arr);
							else
								set_final_status_string(pos, DEAD, out_final_arr);
						}
					}
					/* Set the final status of the (i, j) vertex to either black
					* or white territory.
					*/
					if (out_final_arr[POS(i, j)] == UNKNOWN) {
						if ((out_final_arr[pos] == ALIVE) ^ (get_board(ai, aj)->color == WHITE))
							out_final_arr[POS(i, j)] = BLACK_TERRITORY;
						else
							out_final_arr[POS(i, j)] = WHITE_TERRITORY;
					}
				}
}


int
board_t::get_final_status(int i, int j, int(&final_status)[BOARD_SIZE * BOARD_SIZE])
{
	return final_status[POS(i, j)];
}

void
board_t::set_final_status(int i, int j, int status, int(&final_status)[BOARD_SIZE * BOARD_SIZE])
{
	final_status[POS(i, j)] = status;
}

/* Valid number of stones for fixed placement handicaps. These are
* compatible with the GTP fixed handicap placement rules.
*/
int
board_t::valid_fixed_handicap(int handicap)
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
board_t::place_fixed_handicap(int handicap)
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
board_t::place_free_handicap(int handicap)
{
	int k;
	int i, j;

	for (k = 0; k < handicap; k++) {
		generate_move(&i, &j, BLACK, (unsigned long)time(NULL));
		play_move(i, j, BLACK);
	}
}


board_t::board_t()
{
	ko_i = -1;
	ko_j = -1;
	set_init_board();
}

void board_t::set_init_board()
{
	board_size = BOARD_SIZE;
	int center = BOARD_SIZE / 2;

	for (int pos = 0; pos < BOARD_SIZE*BOARD_SIZE; pos++)
	{
		main_board[pos] = new stone(I(pos), J(pos), this);
		range[pos] = abs(center - I(pos)) + abs(center - J(pos) );
	}
	

}

board_t::board_t(int size = BOARD_SIZE)
{
	komi = -3.14;
	board_size = size;
	int center = BOARD_SIZE / 2;

	for (int pos = 0; pos < size*size; pos++)
	{
		main_board[pos] = new stone(I(pos), J(pos),this);
		range[pos] = abs(center - I(pos)) + abs(center - J(pos));
	}
}

void board_t::copy_board(const board_t & board2)
{
	board_size = board2.board_size;
	komi = board2.komi;
	ko_i = board2.ko_i;
	ko_j = board2.ko_j;
	our_color = board2.our_color;
	total_black_influence = board2.total_black_influence;
	total_white_influence = board2.total_white_influence;
	
	/* copy main_board: stones*/
	for (int iter = 0; iter != board_size * board_size; iter++)
	{
		main_board[iter] = new stone((board2.main_board[iter]),this);
	}

	/* copy all blocks */
	list<block *>::const_iterator it_block = board2.blocks.begin();
	for (it_block; it_block != board2.blocks.end(); it_block++)
	{
		block * new_block = new block(**it_block, *this);
		blocks.push_back(new_block);
	}

}

board_t::board_t(const board_t & old_board)
{
	int center = BOARD_SIZE / 2;

	for (int pos = 0; pos < BOARD_SIZE*BOARD_SIZE; pos++)
	{
		range[pos] = abs(center - I(pos)) + abs(center - J(pos));
	}
	copy_board(old_board);
}
/*???*/
board_t & board_t::operator = (const board_t& old_board)
{
//	clear_board();
	copy_board(old_board);
	return *this;
}

board_t::~board_t()
{
	boardDel();
}

void board_t::boardDel()
{
	/* copy all blocks */
	list<block *>::iterator iter = blocks.begin();
	while(iter != blocks.end())
	{
		block * block = *iter;
		blocks.erase(iter++);
		delete block;
		
	}
	for (int i = 0; i < board_size*board_size; i++){
		delete main_board[i];
	}
}

void board_t::print_all_block_or_chain(int which)
{
	if (which == 0)
		printf("\nprint all chains:\n");
	else if (which == 1)
		printf("\nprint all blocks:\n");
	else if (which == 2)
		printf("\nprint all s_influences:\n");
	else if (which == 3)
		printf("\nprint all stones:\n");
	printf("\n     ");
	for (int i = 0; i < board_size; i++)
	{
		printf("%5d", i);
	}
	printf("\n     ");
	for (int i = 0; i < board_size; i++)
	{
		printf("%5c", 'A'+i);
	}
	printf("\n-------------------------------------\n");
	for (int i = 0; i != board_size; i++)
	{
		printf("%2d|%2d", i,BOARD_SIZE-i);
		for (int j = 0; j != board_size; j++)
		{
			if (which == 0)
				if (main_board[POS(i, j)]->stone_chain != NULL)
					printf("%5d", ((uintptr_t)(main_board[POS(i, j)]->stone_chain)) % 97);
				else printf("%5C", '.');
			else if (which == 1)
				if (main_board[POS(i, j)]->stone_block != NULL)
					printf("%5d", ((uintptr_t)(main_board[POS(i, j)]->stone_block)) % 97);

				else printf("%5C", '.');
			else if (which == 2){
				if (main_board[POS(i, j)]->stone_block != NULL)
				{
					printf("%5d", main_board[POS(i, j)]->stone_block->influence);
				}
				else printf("%5C", '.');
			}
			else if (which == 3)
				if (this->main_board[POS(i, j)]->color == BLACK)
					printf("%5C", 'X');
				else if (this->main_board[POS(i, j)]->color == WHITE)
					printf("%5C", 'O');
				else if (this->main_board[POS(i, j)]->type == LIVE_EYE)
					printf("%5C", '@');
				else if (this->main_board[POS(i, j)]->type == TRUE_EYE_P)
					printf("%5C", 'T');
				else if (this->main_board[POS(i, j)]->type == FAKE_EYE_P)
					printf("%5C", 'F');
				else if (this->main_board[POS(i, j)]->type == TIGER_P)
					printf("%5C", 'G');
				else if (this->main_board[POS(i, j)]->type == SHARED_P)
					printf("%5C", 'S');
				else if (this->main_board[POS(i, j)]->type == FREEDOM_P)
					printf("%5C", 'M');
				else if (this->main_board[POS(i, j)]->type == LIBERTY_P)
					printf("%5C", 'L');
				else if (this->main_board[POS(i, j)]->type == EMPTY_P)
					printf("%5C", '.');
		}
		printf("\n");
	}
	printf("\n############################################\n");
}




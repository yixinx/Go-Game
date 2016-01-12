#ifndef __NEW_BOARD_H__
#define __NEW_BOARD_H__

#include <list>
#include <cstdio>
#include <memory>
#include <queue>
using namespace std;

#define BOARD_SIZE 13
#define influence_distance 4
#define INFLUENCE_CONNECT_BLOCK 100
#define INFLUENCE_UPPER_BOUND 600
#define VERSION_STRING "0.7"

#define MIN_BOARD 2
/* These must agree with the corresponding defines in gtp.c. */
#define EMPTY 0
#define WHITE 1
#define BLACK 2

/* Used in the final_status[] array. */
#define DEAD 0
#define ALIVE 1
#define SEKI 2
#define WHITE_TERRITORY 3
#define BLACK_TERRITORY 4
#define UNKNOWN 5

#define POS(i, j) ((i) * BOARD_SIZE + (j))
#define I(pos) ((pos) / BOARD_SIZE)
#define J(pos) ((pos) % BOARD_SIZE)
/* three give ups from each side means the chess game finishes */
#define THREE_GIVEUPS 3

/* Macro to find the opposite color. */
#define OTHER_COLOR(color) (WHITE + BLACK - (color))

extern int deltai[4];
extern int deltaj[4];
extern int x; 
extern int y;
extern int other_color;
extern int stone_count;
enum block_attr
{
	TRUE_EYE,
	FAKE_EYE,
	TIGER,
	WEAK,
	FREEDOM,
	SHARED,
	MontC
};

enum stone_type
{
	EMPTY_P,
	TRUE_EYE_P,
	FAKE_EYE_P,
	TIGER_P,
	WEAK_P,
	SHARED_P,
	FREEDOM_P,
	LIBERTY_P,
	LIVE_EYE
};

class chain;
class board_t;
class block;

class stone
{
	//private:
public:
	int row;
	int col;
	block * stone_block;
	chain * stone_chain;
	board_t * stone_board;
	int color;
	int s_influence;
	stone_type type;
	chain* shared[4];
	/* allocated at the beginning of the game */
	stone(int r, int c, board_t * new_board);

	/* copy an old stone. allocate a new memory location for this new stone, copy all hte stone information */
	stone(stone * stone2, board_t * new_board);


	/* the destructor will be called only at the end of the game */
	~stone();

	/* remove itself means change the stone_block, stone_chain,color, type to empty value, but will not destroy the stone object.
	Called in the destructor of the "chain" */
	void remove_itself();

	/* change the type of the stone*/
	stone_type change_type(stone_type the_type);

	/* called after a new stone is placed on the board. We first
	check if there are chains nearby. If there are, add this stone to the chain, and also
	TODO: add it to the chain's block. If there is no chain nearby, generate a new chain */
	void add_to_chain(board_t &curr_board );
	void stone_influence();
	bool empty_redistribution();

	void true_fake_eyes();
	void evaluating_empty(int *arr);
	void freedom_or_liberty();
	void evaluating_type(int *arr, int **matrix, int size);
	bool save_shared_info(int *arr, int **matrix, int size);
	bool eat_this_chain(chain* itor);
	bool can_live(chain* itor);
};

/* adding or removing stones should start from the chain, then update block and stne */
class chain
{
private:
	//int remove_itself();

public:
	list<stone *>  stones;
	block* chain_block;
	int length;
	int num;
	int live;
	//int index;

	/* copy constructor, used when copy this chain to a new board */
	chain(chain & old_chain, block & new_block);


	/* constructor: generate a new chain from the stone. the stone will be added to the list "stones" in this chain. the chain will also call the constructor of block and generate a new block. This function will call "absorb_a_stone" */
	chain(stone & new_stone, board_t &board);
	~chain();

	// TODO: called by absorb_a_stone. combine two adjacent chains, link the head of chain2 to the tail of chain1. If the two chains are in different blocks. also combine the two blocks
	void combine_chain(chain * chain2);

	// add a stone to the chain, and also to the block.
	void absorb_a_stone(stone * the_stone);

	// zero out all the attribute of this chain, but don't release  the stones of the chain
	void empty_without_removing();

	void remove_chain_influence(bool(changed_arr)[BOARD_SIZE*BOARD_SIZE]);

	// add the chain to the block
	//void add_to_block(block &block);
	bool evaluating_chain(int black_live_eye_arr[], int white_live_eye_arr[]);
};

class block
{
private:
	/* this function is called by the constructor */
	void init_basic(int the_color);

public:
	board_t * block_board;
	int color;
	int weight[MontC + 1];
	int freedom;
	int liberties;
	int influence;
	//int safety;

	int  true_eyes;
	int  fake_eyes;
	int  tigers;
	int  weaks;
	int  shared;

	list<stone *>  empty;

	list<chain *>  chains;

	/* copy the content of and old block to a new board. the pointes will be different, but others will be the same */
	block(block &old_block, board_t & new_board);

	block(stone & new_stone, board_t & board);

	// TODO: copy constructor
	//block(block &new_block, board_t & board);

	// TODO: called when a new chain is generated, or after updating the influence, we find that we need to split the block
	block(chain & new_chain, board_t & board);

	/* TODO: combine two blocks. Called when two chains are combined */
	// TOOD: update the empty list, update the influence, safe ... after combining
	void combine_block(block & block2);

	/* TODO: update the influence, safety, and freedom when block has changed. (i,j) are the recent location of the stone */
	//int evaluating_influence(int i, int j);


	/* TODO: called after the  evaluating_influence() */
	/* called when a block is splitted*/
	void remove_chain_without_deleting(chain & the_chain);
	// TODO: add a stone to the chain, and also to the block.
	void absorb_a_stone(stone * the_stone);

	// TODO: add a empty to the block
	void absorb_an_empty(int i, int j);

	// TODO: delete an empty to the block
	void remove_an_empty(int i, int j);

	void update_all_empty(stone & the_stone);


	// absorb the chain to the block, remove it from other's block
	void absorb_a_chain(chain & chain);

	// TODO: remove the chain from the block, but doesn't modify the chain
	~block();

	void evaluation(int black_live_eye_arr[], int white_live_eye_arr[]);
	void shared_or_weaks(int **matrix, int size);
	
};


class board_t
{
private:
	void copy_board(const board_t & board2);



	
public:
	/* attributes, originally global variables */
	int board_size;
	double komi;
	int ko_i, ko_j;

	/* if play_with_other_AI is true, our genmove function should manage the data structure for the opponent after it finishes */
	// int play_with_other_AI;

	// main_board: allocated at the beginning of the game, only deleted in destructor
	stone * main_board[BOARD_SIZE * BOARD_SIZE];
	/* the list for all blocks */
	list<block *>  blocks;
	int range[BOARD_SIZE*BOARD_SIZE];					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/* choose the color of us*/
	int our_color;
	int total_black_influence;
	int total_white_influence;


	board_t();
	board_t(int size);

	/* called for copying a board. when we copy a board, only all the pointers in all the objects will change. Other will remain the same */
	board_t(const board_t & old_board);

	/* called when copying a board */
	board_t & operator =(const board_t& old_board);
	~board_t();




	/* original static functions */

	/* pass_move: whether we should pass the move this time? */
	int  pass_move(int i, int j);

	/* whether there is a stone on the board location */
	int  on_board(int i, int j);

	/* Does the chain at (i, j) have any more liberty than the one at
	* (libi, libj)?
	*/
	int  has_additional_liberty(int i, int j, int libi, int libj);

	/* Does (ai, aj) provide a liberty for a stone at (i, j)? */
	int provides_liberty(int ai, int aj, int i, int j, int color);



	/* Are the stone at pos1 and pos2 in the same chain? */
	int same_chain(int pos1, int pos2);

	/* Are the stone at pos1 and pos2 in the same block? */
	int same_block(int pos1, int pos2);

	/* save the empty locations on the board. these locations will not cause suicides */
	void get_available_moves(int color, int &num_moves, int(&moves)[BOARD_SIZE * BOARD_SIZE]);

	/* called  at the end of the game to show all the live or dead stones */
	void set_final_status_string(int pos, int status, int * out_final_arr);

	/* original public functions */
	//void init_brown(int size);
	void set_init_board();

	void clear_board();
	int board_empty();

	stone *  get_board(int i, int j);
	chain *  get_chain(int i, int j);
	int remove_chain(int i, int j);
	/*====================================*/
	/* decide if there is some stone already occupying the place or illegal ko capture */
	int legal_move(int i, int j, int color);

	/* directly place the stone on board, add it to chains or blocks, remove certain chains. No legality check is done here. We need to properly update the board array, the next_stone array, and the ko point.
	TODO: place at (i,j)
	delete dead ones
	add chain
	add block
	update h()*/
	void play_move(int i, int j, int color);
	/* does the place will lead to suicide? */
	int suicide(int i, int j, int color);
	/*====================================*/

	/* this will use our random algorithm to get the suitable move, return a seed */
	unsigned long generate_move(int *i, int *j, int color, unsigned long last_rand);

	//void compute_final_status(void);
	void compute_final_status(int * out_final_arr);
	int get_final_status(int i, int j, int(&final_status)[BOARD_SIZE*BOARD_SIZE]);
	void set_final_status(int i, int j, int status, int(&final_status)[BOARD_SIZE*BOARD_SIZE]);
	int valid_fixed_handicap(int handicap);
	void place_fixed_handicap(int handicap);
	void place_free_handicap(int handicap);
	//void get_avaislable_moves(int color, int &num_moves, int(&moves)[BOARD_SIZE * BOARD_SIZE]);
	//void generate_move_out(int *i, int *j, int color);
	// for Mont
	int count_stones(int color, int *out_final_arr);

	void evaluating_influence();
	// for debug use, 0 prints chain, 1 prints block
	void print_all_block_or_chain(int which);
	/* called by the destructor */
	void boardDel();
	
};

extern board_t board;

void explore_surrounding_block(stone * st);
void decompose_opponent_blocks(board_t *board, stone * the_stone);

void DECOMPOSE_BLOCKS_BFS(board_t * board, block* A, block* B, int color, queue<int> &q);

void COMBINE_BLOCKS_BFS(board_t * board, block* blk, int color, queue<int> &q);
void explore_surrounding_block(stone* st);

void block_decompose(block* A);
void free_list_without_delete_elem(list<stone *> & the_list);
void free_list_with_delete_elem(list<chain *> & the_list);
#endif
#ifndef __SNEW_BOARD_H__
#define __SNEW_BOARD_H__

#include "new_board.h"
#define NUM_THREADS 31
struct MC_msg_t
{
	int i;
	int j;
	int wins;
};

class simple_board_c
{
public:
	/* attributes, originally global variables */
	int board_size;
	double komi;
	int ko_i, ko_j;
	int main_board[BOARD_SIZE * BOARD_SIZE];
	int next_stone[BOARD_SIZE * BOARD_SIZE];
	int final_status[BOARD_SIZE * BOARD_SIZE];
	simple_board_c();
	//simple_board_c(const board_t Big_board);
	~simple_board_c();
	/* board functions */
	/* original static functions */
	int  pass_move(int i, int j);
	int  on_board(int i, int j);
	int  has_additional_liberty(int i, int j, int libi, int libj);
	int provides_liberty(int ai, int aj, int i, int j, int color);
	int suicide(int i, int j, int color);
	int remove_string(int i, int j);
	int same_string(int pos1, int pos2);
	void get_available_moves(int color, int &num_moves, int(&moves)[BOARD_SIZE * BOARD_SIZE]);
	virtual void set_final_status_string(int pos, int status, int * out_final_arr);

	/* original public functions */
	void init_brown(int size);
	void clear_board();
	int board_empty();
	int get_board(int i, int j);
	int get_string(int i, int j, int *stonei, int *stonej);
	int legal_move(int i, int j, int color);
	void play_move(int i, int j, int color);
	unsigned long generate_move(int *i, int *j, int color, unsigned long last_rand);
	void compute_final_status(void);
	//void compute_final_status(int * out_final_arr);
	int get_final_status(int i, int j);
	void set_final_status(int i, int j, int status);
	int valid_fixed_handicap(int handicap);
	void place_fixed_handicap(int handicap);
	void place_free_handicap(int handicap);
	//void get_available_moves(int color, int &num_moves, int(&moves)[BOARD_SIZE * BOARD_SIZE]);
	// for Mont
	virtual int count_stones(int color, int *out_final_arr);


	// for using :::
	void evalue_MC(MC_msg_t *MC_msg, int color, int length);
		/* evaluating MC at position i j. Length is the length for MC_msg, which carry in and out message to 
		this function. Color is current playing color, and after MC evaluation,
		ths winning times are recordend in MC_msg[i].wins */
	void generate_move_out(int *i, int *j, int color);
		/* randomly take up to NUM_THREADS points to ran MC and return the best position in i and j.*/
};

extern simple_board_c simboard;

void MtC(void *);

struct msgpasser_t
{
	int i;
	int j;
	int color;
	int wins;
	unsigned long seed;
	simple_board_c * board_here;
};

#endif
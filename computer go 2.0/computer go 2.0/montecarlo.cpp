#include "new_board.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "game2.h"
#define SIMULATION_TIMES 100

int simple_board_c::count_stones(int color, int *out_final_arr = NULL)
{
	int count = 0;
	if (out_final_arr == NULL)
	{
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
		{
			if (final_status[i] == ALIVE && this->
				main_board[i] == color || (final_status[i] == color + 2)) count++;
		}
	}
	else
	{
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
		{
			if (out_final_arr[i] == ALIVE && this->main_board[i] == color || (out_final_arr[i] == color + 2)) count++;
		}
	}
	return count;
}


int test_showboard(simple_board_c SBoard)
{
	int i, j;
	int symbols[3] = { '.', 'O', 'X' };
	printf("\n");
	for (i = 0; i < SBoard.board_size; i++) {
		printf("\n%2d", SBoard.board_size - i);

		for (j = 0; j < SBoard.board_size; j++)
			printf(" %c", symbols[SBoard.get_board(i, j)]);

		printf(" %d", SBoard.board_size - i);
	}

	printf("\n\n###################################\n");
	return 0;
}

static int test_show_final(simple_board_c board, int * out_final_arr = NULL)
{
	int i, j;
	int symbols[3] = { '.', 'O', 'X' };
	printf("\n");
	if (out_final_arr == NULL)
	{
		for (i = 0; i < board.board_size; i++) {
			printf("\n%2d", board.board_size - i);

			for (j = 0; j < board.board_size; j++)
				//printf(" %d", board.final_status[POS(i, j)]);

			printf(" %d", board.board_size - i);
		}
	}
	else
	{
		for (i = 0; i < board.board_size; i++) {
			printf("\n%2d", board.board_size - i);

			for (j = 0; j < board.board_size; j++)
				printf(" %d", out_final_arr[POS(i, j)]);

			printf(" %d", board.board_size - i);
		}
	}
	printf("\n\n###################################\n");
	return 0;
}

/* place the stone in (i,j), and then start   MonteCarlo
* return -1 if the stone cannot be placed there, else return the total number of ways to win the game starting from (i,j)
*/
void MtC(void * msgpasser)
{
	msgpasser_t *msg = (msgpasser_t *)msgpasser;
	int i = msg->i;
	int j = msg->j;
	int color = msg->color;
	unsigned long seed = msg->seed;
	simple_board_c board_here = *(msg->board_here);
	int index = POS(i, j);
	// the guess place for MC
	int i_new, j_new, num_moves = 0;
	msg->wins = 0;
	//test_showboard(board_save);
	// return -1 if POS(i,j) is illegal 
	if (!board_here.legal_move(i, j, color))
	{
		//printf("i=%d, j=%d, color=%d\n", i, j, color);
		msg->wins = -1;
		return;
	}
	printf("%c%d, \n", 65 + j, msg->board_here->board_size - i);
	board_here.play_move(i, j, color);
	int out_final_arr[BOARD_SIZE * BOARD_SIZE];
	for (int t = 0; t < SIMULATION_TIMES; t++)
	{
		int temp_color = color;
		int is_final = 0;
		int stack = 0;
		while (true)
		{
			seed = board_here.generate_move(&i_new, &j_new, temp_color, seed);
			if (i_new == -1 && j_new == -1)
			{
				is_final++;
				if (is_final == THREE_GIVEUPS * 2) break;
				temp_color = OTHER_COLOR(temp_color);
				continue;
			}
			board_here.play_move(i_new, j_new, temp_color);
			temp_color = OTHER_COLOR(temp_color);
			stack++;
			if (stack >= msg->board_here->board_size * msg->board_here->board_size * 5)
			{
				printf("KK-%c%d-KK, \n", 65 + j, msg->board_here->board_size - i);
				break;
			}
		}
		board_here.compute_final_status();
		//test_showboard(board_here);
		//test_show_final(board_here, out_final_arr);
		//printf("$$$ %d $$$",board_here.count_stones(color, out_final_arr));
		if (board_here.count_stones(color)< board_here.board_size*board_here.board_size / 5)
			msg->wins += 0;
		else if (board_here.count_stones(color)< board_here.board_size*board_here.board_size / 2)
			msg->wins += 40;
		else if (board_here.count_stones(color)< board_here.board_size*board_here.board_size * 4 / 5)
			msg->wins += 50;
		else
			msg->wins += 60;
		//printf("%d\t%d\n", board_here.count_stones(color, out_final_arr), msg->wins);
		// re-init the chess board 
		board_here = *(msg->board_here);
	}

	/* copy back the saved board */
	return;
}



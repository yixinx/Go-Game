#include"alpha-beta-search.h"
#include "new_board.h"
void train_model(int *i, int *j, int color);
#define EVAL_FUNC_NUM 7
#define call_times 1
void train_evaluating_function(int i, block* blk, int aim);
void select(int grade[]);
void explore(board_t* trainboard, int color);
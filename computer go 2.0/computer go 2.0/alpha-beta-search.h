#ifndef __ALPHA_BETA_SEARCH__
#define __ALPHA_BETA_SEARCH__
#include "new_board.h"
#define EVAL_POINTS_NUM 5//每次搜索选出几个候选点
#define INF 10000
extern list<stone*> try_position;	//记录待试下的空点的list
extern int good_points[EVAL_POINTS_NUM]; //候选点的位置
extern int points_grade[EVAL_POINTS_NUM]; //候选点的评分
int evaluate(board_t* board, int evaluate_color);
int evaluate(board_t* board, int evaluating_function);
int evaluation(board_t* board, int evaluating_function, int aim, int old_evaluation_value);
bool test_move(board_t* test_board, int empty_slot, int color);
void select(int grade[], int position[], int size);
int max_value(board_t* old_test_board, int color, int alpha, int beta, int evaluating_function, int call_times, int old_evaluation_value, int evaluate_color);
int min_value(board_t* old_test_board, int color, int alpha, int beta, int evaluating_function, int call_times, int old_evaluation_value, int evaluate_color);
void alpha_beta_search(board_t* trainboard, int color, int x, int y, int evaluating_function, int call_times);
#endif
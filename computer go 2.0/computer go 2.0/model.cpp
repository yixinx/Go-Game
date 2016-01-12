#include "alpha-beta-search.h"
#include "model.h"
#include "new_board.h"
//#include "training.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
using namespace std;
queue<int> q;
list<stone*> try_position;
int safety(block* blk){
	return (blk->fake_eyes + blk->freedom + blk->influence + blk->liberties + blk->shared + blk->tigers +
		blk->true_eyes + blk->weaks) / (blk->chains.size());
}
bool BFS(board_t* trainboard, int * a, int color, int x, int y){
	int S [BOARD_SIZE*BOARD_SIZE];
	memset(S, 0, sizeof(S));
	S[POS(x, y)] = 1;
	while (!q.empty()){
		int node = q.front();
		q.pop();
		//如果node不是一个空点，且它所在block跟color颜色不同，视为color进攻node所在block
		stone* st = trainboard->get_board(I(node), J(node));
		if (st->color == OTHER_COLOR(color)){
			*a = node;
			while (!q.empty()) q.pop(); //找到了nearest block，清空q
			return true;
		}
		//如果node是一个空点，或者它所在block跟color颜色相同，我们都对node的子节点进行扩展
		for (int k = 0; k < 4; k++) { //node的所有子节点
			int a = I(node) + deltai[k];
			int b = J(node) + deltaj[k];
			if (abs(I(node) - x) + abs(J(node) - y) > influence_distance) {
				while (!q.empty()) q.pop(); //距离为4的范围内没有找到nearest block，清空q，返回false;
				return false;
			}
			if (trainboard->on_board(a, b) && S[POS(a, b)] != 1){//如果node的子节点在棋盘上且不在搜索集中,扩展它，加入队列
				q.push(POS(a, b));
				S[POS(a,b)] = 1; //把node添加进入搜索集中
			}
		}
	}
	return false;
}
int influence_position[EVAL_POINTS_NUM];
int influence_grade[EVAL_POINTS_NUM];
int defense_position[EVAL_POINTS_NUM];
void train_model(int *i, int *j, int color){
	printf("begin to explore\n");
	for (int i = 0; i < EVAL_POINTS_NUM; i++){
		influence_grade[i] = 0;
	}
	explore(&board, color); //MYCOLOR应该在棋盘初始化的时候被赋值
	if (influence_grade[0] == -INF) {
		*i = -1;
		*j = -1;
		printf("(%d, %d, %d)\n", *i, *j, influence_grade[0]);
		return;
	}
	*i = I(influence_position[0]);
	*j = J(influence_position[0]);
}
	/*for (int a = 0; a < BOARD_SIZE*BOARD_SIZE; a++){
		*i = I(a);
		*j = J(a);
		if (board.legal_move(*i, *j, color) && !board.suicide(*i, *j, color)){
		printf("(%d %d %d)\n", *i, *j, color);
		return;
		}
		}*/
	//_CrtMemState s1, s2, s3;
	//_CrtMemCheckpoint(&s1);
	//explore(&board, color); //MYCOLOR应该在棋盘初始化的时候被赋值
	// memory allocations take place here
	//_CrtMemCheckpoint(&s2);

	//if (_CrtMemDifference(&s3, &s1, &s2))
	//	_CrtMemDumpStatistics(&s3);//内存泄露
	//else
	//;//无泄露
	/*if (other_color == 0){
		if (influence_grade[0] == -INF) {
			*i = -1;
			*j = -1;
			printf("(%d, %d, %d)\n", *i, *j, influence_grade[0]);
			return;
		}
		*i = I(influence_position[0]);
		*j = J(influence_position[0]);
	}*/
	/*block* nearestblock = 0;
	int a = 0;
	q.push(POS(x, y));
	bool rtv = BFS(&board, &a, other_color, x, y);
	if (a != 0) nearestblock = board.main_board[a]->stone_block;
	if (rtv == false){ //color这一步棋没有进攻，不需要应对，直接进攻对方
		if (influence_grade[0] == -INF) {
			*i = -1;
			*j = -1;
			printf("(%d, %d, %d)\n", *i, *j, influence_grade[0]);
			return;
		}
		*i = I(influence_position[0]);
		*j = J(influence_position[0]);
	}*/
	//else{//color这一步棋进攻了我方nearestblock
		//防守
		/*for (std::list<stone*>::iterator itor = nearestblock->empty.begin(); itor != nearestblock->empty.end(); itor++){
			try_position.push_back(*itor);
		}*/
		/*if (other_color != 0){
			for (int m = 0; m < board.board_size; m++){//敌方下棋位置周围3圈
			for (int n = 0; n < board.board_size; n++){
			if ((abs(m - x) + abs(n - y) <= influence_distance) && (board.get_board(m, n)->color == EMPTY)){
			//&&(board.get_board(m, n)->stone_block != nearestblock)){
			try_position.push_back(board.get_board(m, n));
			}
			}
			}
			}*/
		/*for (int i = 0; i < EVAL_POINTS_NUM; i++){
			list<stone* >::iterator findposition = find(try_position.begin(), try_position.end(), board.main_board[influence_position[i]]);
			if (findposition == try_position.end())
				try_position.push_back(board.main_board[influence_position[i]]);
		}
		printf("begin ab search\n");
		alpha_beta_search(&board, color, x, y, 10, call_times);//防守敌方进犯我方的block
		try_position.clear();*/
		/*if (points_grade[0] == -INF) {
			*i = -1;
			*j = -1;
			printf("(%d, %d, %d)\n", *i, *j, points_grade[0]);
			return;
		}
		*i = I(good_points[0]);
		*j = J(good_points[0]);*/
		//printf("ab search %d %d grade %d", *i, *j, points_grade[0]);
	/*board.print_all_block_or_chain(0);
	board.print_all_block_or_chain(1);
	board.print_all_block_or_chain(2);
	board.print_all_block_or_chain(3);*/
	//getchar();
	/*board.play_move(12, 12, 2);
	board.print_all_block_or_chain(0);
	board.print_all_block_or_chain(1);
	board.print_all_block_or_chain(2);
	board.print_all_block_or_chain(3);
	printf("get char\n");
	getchar();*/
	//printf("train_model begin, last place is %d, %d", x, y);
		
//进攻
	//board_t* trainboard, int color, int x, int y){//颜色为color的一方下了(x,y)
	//generate_move_out(int *i, int *j, int color)
	//是否预测正确
	/*for (int i = 0; i < EVAL_FUNC_NUM; i++){
		for (int j = 0; j < EVAL_POINTS_NUM; j++){
			if (defense_position[i][j] == POS(x, y) || offense_position[i][j] == POS(x, y)){
				evaluation_array[i]++;
				break;
			}
		}
	}*/
	//board.print_all_block_or_chain(0);
	//for (int j = 0; j < EVAL_POINTS_NUM; j++){
		//printf("(%d,%d) ", I(influence_position[j]), J(influence_position[j]));
		/*if (influence_position[j] == POS(x, y)){
			evaluation_array[EVAL_FUNC_NUM-1]++;
			break;
		}*/
	//}
	//printf("\n");
			//*i = I(influence_position[0]);
			//*j = J(influence_position[0]);
			//进攻
			/*block* offense_block = 0;
			int temp_safety = INF;
			for (list<block*>::iterator itor = trainboard->blocks.begin(); itor != trainboard->blocks.end(); itor++){
				if ((*itor)->color == color && safety(*itor) < temp_safety) {
					temp_safety = safety(*itor);
					offense_block = *itor;
				}
			}
			//攻击敌方块的空点
			for (std::list<stone*>::iterator itor = offense_block->empty.begin(); itor != offense_block->empty.end(); itor++){
				try_position.push_back(*itor);
			}
			alpha_beta_search(trainboard, OTHER_COLOR(color), x, y, i, call_times);//进犯敌方的block
			for (int j = 0; j < EVAL_POINTS_NUM-1; j++) {
				offense_position[i][j] = good_points[j];
				offense_grade[i][j] = points_grade[j];
				defense_position[i][j] = 0;//注意defense_position应该被清零，防止上次赋值的影响
				defense_grade[i][j] = 0;
			}
			for (int j = 0; j < EVAL_POINTS_NUM; j++){
				printf("(%d,%d) ", I(offense_position[i][j]), J(offense_position[i][j]));
				printf("%d", offense_grade[i][j]);
			}
			printf("\n");
			try_position.clear();*/
		/*}
		else{//color这一步棋进攻了我方nearestblock
			//防守
			//敌方攻击我方块的空点
			for (std::list<stone*>::iterator itor = nearestblock->empty.begin(); itor != nearestblock->empty.end(); itor++){
				try_position.push_back(*itor);
			}
			for (int m = 0; m < trainboard->board_size; m++){//敌方下棋位置周围3圈
				for (int n = 0; n < trainboard->board_size; n++){
					if ((abs(m - x) + abs(n - y) <= influence_distance) && (trainboard->get_board(m, n)->color == EMPTY) &&
						(trainboard->get_board(m, n)->stone_block != nearestblock))
						try_position.push_back(trainboard->get_board(m, n));
				}
			}
			alpha_beta_search(trainboard, OTHER_COLOR(color), x, y, 10, call_times);//防守敌方进犯我方的block
			for (int j = 0; j < EVAL_POINTS_NUM - 1; j++) {
				defense_position[j] = good_points[j];
				//defense_grade[j] = points_grade[j];

			}
			try_position.clear();
		}		*/	//进攻
			/*block* offense_block = 0;
			int temp_safety = INF;
			for (list<block*>::iterator itor = trainboard->blocks.begin(); itor != trainboard->blocks.end(); itor++){
				if ((*itor)->color == color && safety(*itor) < temp_safety) {
					temp_safety = safety(*itor);
					offense_block = *itor;
				}
			}
			//攻击敌方块的空点
			for (std::list<stone*>::iterator itor = offense_block->empty.begin(); itor != offense_block->empty.end(); itor++){
				try_position.push_back(*itor);
			}
			alpha_beta_search(trainboard, OTHER_COLOR(color), x, y, i, call_times);//防守敌方进犯我方的block
			for (int j = 0; j < EVAL_POINTS_NUM; j++) {
				offense_position[i][j] = good_points[j];
				offense_grade[i][j] = points_grade[j];
			}
			try_position.clear();
			for (int j = 0; j < EVAL_POINTS_NUM; j++){
				printf("(%d,%d) ", I(offense_position[i][j]), J(offense_position[i][j]));
				printf("%d", offense_grade[i][j]);
			}
			printf("\n");
			for (int j = 0; j < EVAL_POINTS_NUM; j++){
				printf("(%d,%d) ", I(defense_position[i][j]), J(defense_position[i][j]));
				printf("%d", defense_grade[i][j]);
			}
			printf("\n");*/
	//}
	//依据影响力模型下空点

void select(int grade[]){
	int temp_grade, temp_i;
	for (int j = 0; j < EVAL_POINTS_NUM; j++){
		temp_grade = -INF;
		temp_i = 0;
		for (int i = j; i < BOARD_SIZE*BOARD_SIZE; i++){
			if (grade[i] > temp_grade) {
				temp_grade = grade[i];
				temp_i = i;
			}
		}
		influence_position[j] = temp_i;
		influence_grade[j] = temp_grade;
		//printf("explore %d %d, grade %d\n", I(influence_position[j]), J(influence_position[j]), temp_grade);
		//influence_grade[j] = temp_grade;
		grade[temp_i] = -INF;
	}

}
void explore(board_t* trainboard, int color){
	board_t *test_board; //deep_copy旧测试棋盘到新的测试棋盘中(包括棋盘下的所有数据结构)
	int influence[BOARD_SIZE*BOARD_SIZE];
	printf("[explore]\n");
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);

	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
		influence[i] = 0;
		
		test_board = new board_t(*trainboard);
		if (!test_move(test_board, i, color)) {
			influence[i] = -INF;
			continue;
		}
		influence[i] = test_board->total_black_influence + test_board->total_white_influence;
		if (color == WHITE) influence[i] = -influence[i];
		delete test_board;
		//memory allocations take place here
		/*
		_CrtMemCheckpoint(&s2);
		if (_CrtMemDifference(&s3, &s1, &s2)){
			_CrtMemDumpStatistics(&s3);//内存泄露
			printf("memory leak!\n");
			exit(-1);
		}
		else printf(" no mem left!\n")
			;//无泄露
			*/
	}

	_CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&s3, &s1, &s2)){
		_CrtMemDumpStatistics(&s3);//内存泄露
		printf("memory leak!\n");
		exit(-1);
	}
	else printf(" no mem left!\n")
		;//无泄露


	select(influence);
}
/*
void explore(board_t* trainboard, int color){
	board_t *test_board; //deep_copy旧测试棋盘到新的测试棋盘中(包括棋盘下的所有数据结构)
	int influence[BOARD_SIZE*BOARD_SIZE];
	int black[BOARD_SIZE*BOARD_SIZE];
	int white[BOARD_SIZE*BOARD_SIZE];
	//memset(influence, 0, sizeof(influence));
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
		influence[i] = 0;
		black[i] = 0;
		white[i] = 0;
		_CrtMemState s1, s2, s3;
		_CrtMemCheckpoint(&s1);
		test_board = new board_t(*trainboard);
		if (!test_move(test_board, i, BLACK)) {
			influence[i] = -INF;
			continue;
		}
		float black_eval = 0;
		float white_eval = 0;
		for (std::list<block*>::iterator itor = test_board->blocks.begin(); itor != test_board->blocks.end(); itor++){//所有待下点保存在try_position中
			if ((*itor)->color == BLACK) {
				black_eval -= 0.3 * (*itor)->weaks;
				black_eval += 0.04 * (*itor)->shared;
				black_eval += 0.08 * (*itor)->tigers;
				black_eval += 0.16 * (*itor)->fake_eyes;
				black_eval += 0.32 * (*itor)->true_eyes;
				list<chain*>::iterator iter = (*itor)->chains.begin();
				while (iter != (*itor)->chains.end()){
					if ((*iter)->stones.size() > 32)
						black_eval += (*iter)->stones.size() / 10.0;
					else if ((*iter)->stones.size() > 16)
						black_eval += (*iter)->stones.size() / 12.0;
					else if ((*iter)->stones.size() > 8)
						black_eval += (*iter)->stones.size() / 14.0;
					else if ((*iter)->stones.size() > 4)
						black_eval += (*iter)->stones.size() / 16.0;
					else if ((*iter)->stones.size() > 2)
						black_eval += (*iter)->stones.size() / 18.0;
					else
						black_eval += (*iter)->stones.size() / 20.0;
					iter++;
				}
				list<stone*>::iterator it = (*itor)->empty.begin();
				while (it != (*itor)->empty.end()){
					if ((*it++)->type == LIVE_EYE)	black_eval += 0.5;
				}
				black[i] += (*itor)->influence*black_eval*(1 + (*itor)->freedom / 10.0 + (*itor)->liberties / 20.0);
			}
			else{
				white_eval -= 0.3  * (*itor)->weaks;
				white_eval += 0.04 * (*itor)->shared;
				white_eval += 0.08 * (*itor)->tigers;
				white_eval += 0.16 * (*itor)->fake_eyes;
				white_eval += 0.32 * (*itor)->true_eyes;
				list<chain*>::iterator iter = (*itor)->chains.begin();
				while (iter != (*itor)->chains.end()){
					if ((*iter)->stones.size() > 32)
						white_eval += (*iter)->stones.size() / 10.0;
					else if ((*iter)->stones.size() > 16)
						white_eval += (*iter)->stones.size() / 12.0;
					else if ((*iter)->stones.size() > 8)
						white_eval += (*iter)->stones.size() / 14.0;
					else if ((*iter)->stones.size() > 4)
						white_eval += (*iter)->stones.size() / 16.0;
					else if ((*iter)->stones.size() > 2)
						white_eval += (*iter)->stones.size() / 18.0;
					else
						white_eval += (*iter)->stones.size() / 20.0;
					iter++;
				}
				list<stone*>::iterator it = (*itor)->empty.begin();
				while (it != (*itor)->empty.end()){
					if ((*it++)->type == LIVE_EYE)	white_eval += 0.5;
				}
				black[i] += (*itor)->influence*white_eval*(1 + (*itor)->freedom / 10.0 + (*itor)->liberties / 20.0);
			}
		}
		delete test_board;
		test_board = new board_t(*trainboard);
		if (!test_move(test_board, i, WHITE)) {
			white[i] = INF;
			continue;
		}
		black_eval = 0;
		white_eval = 0;
		for (std::list<block*>::iterator itor = test_board->blocks.begin(); itor != test_board->blocks.end(); itor++){//所有待下点保存在try_position中
			if ((*itor)->color == BLACK) {
				black_eval -= 0.3 * (*itor)->weaks;
				black_eval += 0.04 * (*itor)->shared;
				black_eval += 0.08 * (*itor)->tigers;
				black_eval += 0.16 * (*itor)->fake_eyes;
				black_eval += 0.32 * (*itor)->true_eyes;
				list<chain*>::iterator iter = (*itor)->chains.begin();
				while (iter != (*itor)->chains.end()){
					if ((*iter)->stones.size() > 32)
						black_eval += (*iter)->stones.size() / 10.0;
					else if ((*iter)->stones.size() > 16)
						black_eval += (*iter)->stones.size() / 12.0;
					else if ((*iter)->stones.size() > 8)
						black_eval += (*iter)->stones.size() / 14.0;
					else if ((*iter)->stones.size() > 4)
						black_eval += (*iter)->stones.size() / 16.0;
					else if ((*iter)->stones.size() > 2)
						black_eval += (*iter)->stones.size() / 18.0;
					else
						black_eval += (*iter)->stones.size() / 20.0;
					iter++;
				}
				list<stone*>::iterator it = (*itor)->empty.begin();
				while (it != (*itor)->empty.end()){
					if ((*it++)->type == LIVE_EYE)	black_eval += 0.5;
				}
				white[i] += (*itor)->influence*black_eval*(1 + (*itor)->freedom / 10.0 + (*itor)->liberties / 20.0);

			}
			else{
				white_eval -= 0.3  * (*itor)->weaks;
				white_eval = 0.04 * (*itor)->shared;
				white_eval += 0.08 * (*itor)->tigers;
				white_eval += 0.16 * (*itor)->fake_eyes;
				white_eval += 0.32 * (*itor)->true_eyes;
				list<chain*>::iterator iter = (*itor)->chains.begin();
				while (iter != (*itor)->chains.end()){
					if ((*iter)->stones.size() > 32)
						white_eval += (*iter)->stones.size() / 10.0;
					else if ((*iter)->stones.size() > 16)
						white_eval += (*iter)->stones.size() / 12.0;
					else if ((*iter)->stones.size() > 8)
						white_eval += (*iter)->stones.size() / 14.0;
					else if ((*iter)->stones.size() > 4)
						white_eval += (*iter)->stones.size() / 16.0;
					else if ((*iter)->stones.size() > 2)
						white_eval += (*iter)->stones.size() / 18.0;
					else
						white_eval += (*iter)->stones.size() / 20.0;
					iter++;
				}
				list<stone*>::iterator it = (*itor)->empty.begin();
				while (it != (*itor)->empty.end()){
					if ((*it++)->type == LIVE_EYE)	white_eval += 0.5;
				}
				white[i] += (*itor)->influence*white_eval*(1 + (*itor)->freedom / 10.0 + (*itor)->liberties / 20.0);

			}
		}
		delete test_board;
		influence[i] = black[i] - white[i];
		//memory allocations take place here
		CrtMemCheckpoint(&s2);

		if (_CrtMemDifference(&s3, &s1, &s2)){
			_CrtMemDumpStatistics(&s3);//内存泄露
			printf("memory leak!\n");
			exit(-1);
		}
		else
			;//无泄露
	}
	printf("\n-------------------------------------\n");
	for (int i = 0; i != BOARD_SIZE; i++){
		printf("%2d|%2d", i, BOARD_SIZE - i);
		for (int j = 0; j != BOARD_SIZE; j++)
		{
			if (board.main_board[POS(i, j)]->color == BLACK)	printf("%5C", 'X');
			else if (board.main_board[POS(i, j)]->color == WHITE)	printf("%5C", 'O');
			else printf("%5d", influence[POS(i, j)]);
		}
		printf("\n");
	}
	//if (influence[i] < 80) influence[i] = -INF;

	select(influence);
}*/
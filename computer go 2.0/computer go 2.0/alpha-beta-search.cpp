//�����������ʱ���ȵ���evaluation�����ҷ��������鵱ǰ����ֵa��Ȼ�����alpha-beta��֦��������Ԥ����������������ֵb
//��alpha-beta��������b-a����ֵ��b-a������һ�ֵı�����Ч��Խ��Խ��
//������ǿ���ʱ���ȵ���evaluation����з��������鵱ǰ����ֵa��Ȼ�����alpha-beta��֦��������Ԥ����������������ֵb
//��alpha-beta��������a-b����ֵ��a-b������һ�ֵı�����Ч��Խ��Խ��a-b����ɱ�幦Ч��Խ��Խ��
#include"alpha-beta-search.h"
#include "new_board.h"
#define _CRTDBG_MAP_ALLOC
#define _CrtSetDbgFlag
#define _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#define _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG)
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#include <stdlib.h>
#include <crtdbg.h>

int good_points[EVAL_POINTS_NUM];
int points_grade[EVAL_POINTS_NUM];
int max(int a, int b){
	if (a>b) return a;
	return b;
}

int min(int a, int b){
	if (a<b) return a;
	return b;
}
int evaluate(board_t* board, int evaluating_function, int evaluate_color){//ѵ����
	int eval = 0;
	for (std::list<block*>::iterator itor = board->blocks.begin(); itor != board->blocks.end(); itor++){//���д��µ㱣����try_position��
		if ((*itor)->color == BLACK) {
			switch (evaluating_function)
			{
			case TRUE_EYE:
				eval += (*itor)->true_eyes;
				break;
			case FAKE_EYE:
				eval += (*itor)->fake_eyes;
				break;
			case TIGER:
				eval += (*itor)->tigers;
				break;
			case WEAK:
				eval += (*itor)->weaks;
				break;
			case FREEDOM:
				eval += (*itor)->freedom;
				break;
			case SHARED:
				eval += (*itor)->shared;

				break;
			case 10:
				eval += (*itor)->weaks;
				eval += 2 * (*itor)->liberties;
				eval += 2 * (*itor)->freedom;
				eval += 4 * (*itor)->shared;
				eval += 8 * (*itor)->tigers;
				eval += 16 * (*itor)->fake_eyes;
				eval += 32 * (*itor)->true_eyes;
				for (list<chain*>::iterator iter = (*itor)->chains.begin(); iter != (*itor)->chains.end(); iter++)
					eval += 64 * (*iter)->live;
				break;
			default:
				break;
			}
		}
		else{
			switch (evaluating_function)
			{
			case TRUE_EYE:
				eval -= (*itor)->true_eyes;
				break;
			case FAKE_EYE:
				eval -= (*itor)->fake_eyes;
				break;
			case TIGER:
				eval -= (*itor)->tigers;
				break;
			case WEAK:
				eval -= (*itor)->weaks;
				break;
			case FREEDOM:
				eval -= (*itor)->freedom;
				break;
			case SHARED:
				eval -= (*itor)->shared;
				break;
			case LIBERTY_P:
				eval -= (*itor)->liberties;
				break;
			case LIVE_EYE:
				eval -= (*itor)->liberties;
				break;
			case 10:
				eval -= (*itor)->weaks;
				eval -= 2 * (*itor)->liberties;
				eval -= 2 * (*itor)->freedom;
				eval -= 4 * (*itor)->shared;
				eval -= 8 * (*itor)->tigers;
				eval -= 16 * (*itor)->fake_eyes;
				eval -= 32 * (*itor)->true_eyes;
				for (list<chain*>::iterator iter = (*itor)->chains.begin(); iter != (*itor)->chains.end(); iter++)
					eval -= 64 * (*iter)->live;
				break;
			default:
				break;
			}
		}
	}
	eval = eval + (board->total_black_influence + board->total_white_influence) / 100;
	if (evaluate_color == WHITE) return -eval;
	return eval;
}
/*int evaluate(board_t* board, int color){//������
	int eval = 0;
	for (std::list<block*>::iterator itor = board->blocks.begin(); itor != board->blocks.end(); itor++){//���д��µ㱣����try_position��
		if ((*itor)->color == BLACK) {
			eval += (*itor)->grade;
		}
		else{
			eval -= (*itor)->grade;
		}
	}
	return eval;
}*/
/*int evaluation(board_t* board, int color, int evaluating_function,  int old_evaluation_value){
	return evaluate(board, evaluating_function, color) - old_evaluation_value;
}*/

bool test_move(board_t* test_board, int empty_slot, int color){
	int i = I(empty_slot);
	int j = J(empty_slot);
	if (test_board->legal_move(i, j, color) && !test_board->suicide(i, j, color)){
		if (test_board->main_board[empty_slot]->type == LIVE_EYE){
			return false;
		}
		test_board->play_move(i, j, color);
		return true;
	}
	return false;
}

void select(int grade[], int position[], int size){
	int temp_grade, temp_position, temp_i, eval_points_num;
	if (size < EVAL_POINTS_NUM) eval_points_num = size;
	else eval_points_num = EVAL_POINTS_NUM;
	for (int j = 0; j < eval_points_num; j++){
		temp_grade = -INF;
		temp_position = 0;
		temp_i = 0;
		for (int i = j; i < size; i++){
			if (grade[i] > temp_grade) {
				temp_grade = grade[i];
				temp_position = position[i];
				temp_i = i;
			}
		}
		good_points[j] = temp_position;
		points_grade[j] = temp_grade;
		grade[temp_i] = grade[j];
		position[temp_i] = position[j];
		grade[j] = temp_grade;
		position[j] = temp_position;
	}

}
/*bool captured_sth(int i, int j, board_t* board){
	for (int k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (board->on_board(ai, aj)
			&& board->get_board(ai, aj)->color == OTHER_COLOR(board->get_board(i, j)->color)
			// (i,j) provide liberty for (ai,aj)
			&& !board->has_additional_liberty(ai, aj, i, j))
		{
			return true;
		}
	}
	return false;
}*/
/*void recover(board_t * test_board, int i, int j){
	int recover_pos = POS(i, j);
	stone* recover_stone = test_board->main_board[recover_pos];
	chain* recover_chain = recover_stone->stone_chain;
	block * recover_block = recover_stone->stone_block;
	//Ӱ�������¼���
	bool empty_arr[BOARD_SIZE*BOARD_SIZE];
	recover_stone->evaluating_influence(1, empty_arr);
	//ɾ����
	recover_stone->stone_chain = NULL;
	recover_stone->stone_block = NULL;
	recover_chain->stones.remove(recover_stone);
	recover_stone->color = EMPTY;
	recover_stone->type = EMPTY_P;
	if (recover_chain->stones.size()==0){//�������chain��Ψһ�ģ���Ҫ��chainһ��ɾ��
		delete recover_chain;
	}
	if (recover_block->chains.size() == 0){//���chain��block��Ψһ�ģ���Ҫ��blockһ��ɾ��
		delete recover_block;
	}
	else recover_block->evaluation(); //���ɾ����block��ô��ִ�����´���
	//�յ����·ֲ�
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++)
	{
		if (empty_arr[i] == true)
			test_board->main_board[i]->empty_redistribution();
	}
}*/
void alpha_beta_search(board_t* trainboard, int color, int x, int y, int evaluating_function, int call_times){
	int old_evaluation_value = 0;

	int* position = new int[try_position.size()];
	memset(position, 0, try_position.size()*sizeof(int));//���µ�λ��
	int* grade = new int[try_position.size()];//initialize a array whose size is try position
	for (int i = 0; i < try_position.size(); i++)	grade[i] = -INF;
	//memset(grade, 0, try_position.size()*sizeof(int));//���µ�����
	int i = 0;
	list<stone*>::iterator itor = try_position.begin();
	while (itor != try_position.end()){//���д��µ㱣����try_position��
		_CrtMemState s1, s2, s3;
		_CrtMemCheckpoint(&s1);
		board_t *test_board = new board_t(*trainboard); //deep_copy�ɲ������̵��µĲ���������(���������µ��������ݽṹ)
		if(!test_move(test_board, POS((*itor)->row, (*itor)->col), color)) continue; // �ڲ�������������һ��,����λ����itorָ��Ŀյ�
		grade[i] = min_value(test_board, OTHER_COLOR(color), -INF, INF, evaluating_function, call_times - 1, old_evaluation_value, color);//����minֵ
		position[i] = POS((*itor)->row, (*itor)->col);
		i++; itor++;
		delete test_board;
		// memory allocations take place here
		_CrtMemCheckpoint(&s2);

		if (_CrtMemDifference(&s3, &s1, &s2))
			_CrtMemDumpStatistics(&s3);//�ڴ�й¶
		else
			;//��й¶
		
	}
	/*for (int i = 0; i < try_position.size(); i++)
	{
		printf("(%d, %d) %d ", I(position[i]), J(position[i]), grade[i]);
	}
	printf("\n");*/
	select(grade, position, try_position.size());//ѡ��EVAL_NUM�����ŵĵ㣬����λ�ü�¼��good_points�������ּ�¼��points_grade
	delete position;
	delete grade;
}

int max_value(board_t* old_test_board,  int color, int alpha, int beta, int evaluating_function, int call_times,  int old_evaluation_value, int evaluate_color){
	if (call_times == 0)	
		return evaluate(old_test_board, evaluating_function, evaluate_color);//ѵ����
	//evaluation(old_test_board, color, evaluating_function, old_evaluation_value);
	int v = -INF;
	for (std::list<stone*>::iterator itor = try_position.begin(); itor != try_position.end(); itor++){//blk->empty�б���blk�еĿյ�
		_CrtMemState s1, s2, s3;
		_CrtMemCheckpoint(&s1);
		board_t *test_board = new board_t(*old_test_board); //deep_copy�ɲ������̵��µĲ���������(���������µ��������ݽṹ)
		/*�Ż��������̲���*/
		if(!test_move(test_board, POS((*itor)->row, (*itor)->col), color)) continue;// �ڲ�������������һ��,����λ����itorָ��Ŀյ㣬�����������˵���õ�������ɱ
		v = max(v, min_value(test_board, OTHER_COLOR(color), alpha, beta, evaluating_function, call_times - 1, old_evaluation_value, evaluate_color));
		if (v >= beta) 
			return v;
		alpha = max(alpha, v);
		delete test_board;
		// memory allocations take place here
		_CrtMemCheckpoint(&s2);

		if (_CrtMemDifference(&s3, &s1, &s2))
			_CrtMemDumpStatistics(&s3);//�ڴ�й¶
		else
			;//��й¶
	}
	return v;
}

int min_value(board_t* old_test_board, int color, int alpha, int beta, int evaluating_function, int call_times, int old_evaluation_value, int evaluate_color){
	if (call_times == 0) 
		return evaluate(old_test_board, evaluating_function, evaluate_color);//ѵ����
	//evaluation(old_test_board, color, evaluating_function, old_evaluation_value);
	int v = INF;
	for (std::list<stone*>::iterator itor = try_position.begin(); itor != try_position.end(); itor++){
		_CrtMemState s1, s2, s3;
		_CrtMemCheckpoint(&s1);
		board_t *test_board = new board_t(*old_test_board); //deep_copy�ɲ������̵��µĲ���������(���������µ��������ݽṹ)
		if(!test_move(test_board, POS((*itor)->row, (*itor)->col), color)) {
			continue; // �ڲ�������������һ��,����λ����itorָ��Ŀյ�
		}
		v = min(v, max_value(test_board, OTHER_COLOR(color), alpha, beta, evaluating_function, call_times - 1, old_evaluation_value, evaluate_color));
		if (v <= alpha) 
			return v;
		beta = min(beta, v);
		delete test_board;
		// memory allocations take place here
		_CrtMemCheckpoint(&s2);

		if (_CrtMemDifference(&s3, &s1, &s2))
			_CrtMemDumpStatistics(&s3);//�ڴ�й¶
		else
			;//��й¶
	}
	return v;
}
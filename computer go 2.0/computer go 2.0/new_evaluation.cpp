#include "new_board.h"
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <queue>
#define INF_CENTRE 160
void block::evaluation(int black_live_eye_arr[], int white_live_eye_arr[])
{
	this->true_eyes = 0;
	this->fake_eyes = 0;
	this->tigers = 0;
	this->shared = 0;
	this->weaks = 0;
	this->liberties = 0;
	int arr[4] = { 0, 0, 0, 0 };	//[my, enemy, empty, outside]
	//judge each stone in this block is what kind of type

	//This is for separating the shared and weaks

	int num = 0;
	//give a temporary chain number to the chains in this block
	for (std::list<chain*>::iterator itor = chains.begin(); itor != chains.end(); ++itor)
	{
		(*itor)->num = num;
		num++;
	}
	const int size = chains.size();
	int **matrix;
	matrix = new int *[size];
	for (int i = 0; i < size; ++i)
		matrix[i] = new int[size];
	//Initialization
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			matrix[i][j] = 0;
		}
	}
	//PART 1
	for (std::list<stone*>::iterator itor = this->empty.begin(); itor != this->empty.end(); ++itor)
	{
		arr[0] = 0, arr[1] = 0, arr[2] = 0, arr[3] = 0;
		(*itor)->evaluating_empty(arr);
		(*itor)->evaluating_type(arr, matrix, size);
	}
	this->shared_or_weaks(matrix, size);
	//delete matrix
	for (int i = 0; i<size; ++i)
		delete[] matrix[i];
	delete[] matrix;

	//PART 2
	for (std::list<chain*>::iterator itor = this->chains.begin(); itor != this->chains.end(); ++itor)
	{
		(*itor)->evaluating_chain(black_live_eye_arr, white_live_eye_arr);
	}
}
//PART 1
void stone::evaluating_empty(int *arr)
{	//arr[4] -> [my, enemy, empty, outside]
	int countforshared = 0;
	for (int k = 0; k < 4; k++)
	{
		int index = POS(row + deltai[k], col + deltaj[k]);
		if (!stone_board->on_board(row + deltai[k], col + deltaj[k]))
		{
			arr[3]++;
		}
		else if (stone_board->main_board[index]->color == stone_block->color)
		{
			arr[0]++;
			this->shared[countforshared] = stone_board->main_board[index]->stone_chain;
			countforshared++;
			//record the chain if we find the same color
		}
		else if (stone_board->main_board[index]->color == OTHER_COLOR(stone_block->color))
		{
			arr[1]++;
		}
		else if (stone_board->main_board[index]->color == EMPTY)
		{
			arr[2]++;
		}
	}
}
void stone::evaluating_type(int arr[4], int **matrix, int size)
{
	//arr[4] -> [my, enemy, empty, outside]
	//judge the type of this empty point. 
	if ((arr[0] == 4)										//INSIDE
		|| ((arr[0] == 3) && (arr[3] == 1))					//EDGE
		|| ((arr[0] == 2) && (arr[3] == 2))					//CORNER
		)
	{
		type = TRUE_EYE_P;
		true_fake_eyes();
	}
	else if (((arr[0] == 3) && (arr[2] == 1))				//INSIDE
		|| ((arr[0] == 2) && (arr[2] == 1) && (arr[3] == 1))//EDGE
		|| ((arr[0] == 1) && (arr[2] == 1) && (arr[3] == 2))//CORNER
		)
	{
		type = TIGER_P;
		stone_block->tigers += 1;
	}
	//SHARED OR WEAKS
	else if (save_shared_info(arr, matrix, size))
	{ }
	//FREEDOM OR LIBERTY
	else
	{
		freedom_or_liberty();
	}
}
void stone::true_fake_eyes()
{
	int outside = 0, corner_sum = 0;
	int corneri[4] = { -1, -1, 1, 1 };
	int cornerj[4] = { -1, 1, -1, 1 };
	for (int n = 0; n < 4; n++)
	{
		int index2 = POS(this->row + corneri[n], this->col + cornerj[n]);
		if (stone_board->on_board(this->row + corneri[n], this->col + cornerj[n]))
		{
			if (stone_board->main_board[index2]->color == stone_block->color)
			{ 
				corner_sum += 1;
			}
		}
			else//not on the board
			{
				outside++;
			}
	}
	if (
		(outside == 3 && corner_sum == 1)
		|| (outside == 2 && corner_sum == 2)
		|| (outside == 0 && corner_sum == 3)
		|| (outside == 0 && corner_sum == 4)
		) 
	{
		this->type = TRUE_EYE_P; 
		stone_block->true_eyes += 1;
	}
	else 
	{ 
		this->type = FAKE_EYE_P; 
		stone_block->fake_eyes += 1; 
	}
}
bool stone::save_shared_info(int *arr, int**matrix, int size)
{
		if ((arr[0] == 3) && (arr[1] == 1))				//INSIDE:three black and one white
		{
			if (shared[0] == shared[1] && shared[1] == shared[2])//It is not shared
			{
				freedom_or_liberty();
				return 1;
			}
			else
			{
				type = SHARED_P;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						if ((i == shared[0]->num) && (j == shared[1]->num))
							matrix[i][j] += 1;
						else if ((i == shared[1]->num) && (j == shared[0]->num))
							matrix[i][j] += 1;
						if ((i == shared[0]->num) && (j == shared[2]->num))
							matrix[i][j] += 1;
						else if ((i == shared[2]->num) && (j == shared[0]->num))
							matrix[i][j] += 1;
						if ((i == shared[1]->num) && (j == shared[2]->num))
							matrix[i][j] += 1;
						else if ((i == shared[2]->num) && (j == shared[1]->num))
							matrix[i][j] += 1;
					}
				}
				return 1;
			}
		}
		else if (
			((arr[0] == 2) && (arr[1] == 2))					//INSIDE:two black, two white
			|| ((arr[0] == 2) && (arr[1] == 1) && (arr[2] == 1))//INSIDE:two black, one white, one empty
			|| ((arr[0] == 2) && (arr[2] == 2))					//INSIDE:two black, two empty
			|| ((arr[0] == 2) && (arr[1] == 1) && (arr[3] == 1))//EDGE	//CORNER is impossible
			)
		{
			if (shared[0] == shared[1])							//It is not shared
			{
				freedom_or_liberty();
				return 1;
			}
			else
			{
				type = SHARED_P;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						if ((i == shared[0]->num) && (j == shared[1]->num))
							matrix[i][j] += 1;
						else if ((i == shared[1]->num) && (j == shared[0]->num))
							matrix[i][j] += 1;
					}
				}
				return 1;
			}
		}
		//ELSE: it's not shared, return and continue freedom and liberty
		return 0;
}
void stone::freedom_or_liberty()
{
	if (color != 0) return;
	if ((stone_block->color == BLACK && s_influence > 0 && (s_influence > 80))
		|| (stone_block->color == WHITE && s_influence < 0 && (s_influence < -80)))
	{
			type = FREEDOM_P;
			stone_block->freedom += 1;
	}
	else
	{
			type = LIBERTY_P;
			stone_block->liberties += 1;
	}
}
void block::shared_or_weaks(int **matrix, int size)
{
	for (std::list<chain*>::iterator itor = chains.begin(); itor != chains.end(); ++itor)
	{
		for (int i = 0; i < size; i++)
		{
			if (i != (*itor)->num)
				if (matrix[(*itor)->num][i] > 1)
				{
					this->shared += matrix[(*itor)->num][i];
				}
				else if (matrix[(*itor)->num][i] == 1)
				{
					this->weaks++;
				}
		}
	}
	this->shared = this->shared / 4;
	this->weaks = this->weaks / 2;
}
//PART 2
void live_eye_bfs(chain* chain, int color, queue<int> &q, int black_live_eye_arr[], int white_live_eye_arr[]){
	int T[BOARD_SIZE*BOARD_SIZE];
	//memset(T, 0, BOARD_SIZE*BOARD_SIZE*sizeof(T));
	T[q.front()] = 1; //把node添加进入搜索集中
	while (!q.empty()){
		int node = q.front();
		q.pop();
		stone* st = chain->chain_block->block_board->get_board(I(node), J(node));
		if (st->stone_chain == NULL){
			if (color == BLACK){
				if (white_live_eye_arr[node] != 1) white_live_eye_arr[node] = 2;	//可能是白棋的真眼
			}
			else{
				if (black_live_eye_arr[node] != 1) black_live_eye_arr[node] = 2;	//可能是黑棋的真眼
			}
		}
		for (int k = 0; k < 4; k++) {
			// de的所有子节点
			int a = I(node) + deltai[k];
			int b = J(node) + deltaj[k];
			if ((chain->chain_block->block_board->on_board(a, b) && T[POS(a, b)] != 1)
				&& (chain->chain_block->block_board->get_board(a, b)->color == color)){//如果node的子节点在棋盘上且不在搜索集中,扩展它，加入队列
				q.push(POS(a, b));
				T[POS(a, b)] = 1; //把node添加进入搜索集中
			}
		}
	}
}
void not_live_eye_bfs(chain* chain, int color, queue<int> &q, int black_live_eye_arr[], int white_live_eye_arr[]){
	int T[BOARD_SIZE*BOARD_SIZE];
	//memset(T, 0, BOARD_SIZE*BOARD_SIZE*sizeof(T));
	T[q.front()] = 1; //把node添加进入搜索集中
	while (!q.empty()){
		int node = q.front();
		q.pop();
		stone* st = chain->chain_block->block_board->get_board(I(node), J(node));
		if (st->stone_chain == NULL){
			if (color == BLACK){
				white_live_eye_arr[node] = 1;	//try_position不是白棋的真眼
			}
			else{
				black_live_eye_arr[node] = 1;	//try_position不是黑棋的真眼
			}
		}
		for (int k = 0; k < 4; k++) {
			// de的所有子节点
			int a = I(node) + deltai[k];
			int b = J(node) + deltaj[k];
			if ((chain->chain_block->block_board->on_board(a, b) && T[POS(a, b)] != 1)
				&& (chain->chain_block->block_board->get_board(a, b)->color == color)){//如果node的子节点在棋盘上且不在搜索集中,扩展它，加入队列
				q.push(POS(a, b));
				T[POS(a, b)] = 1; //把node添加进入搜索集中
			}
		}
	}
}
bool chain::evaluating_chain(int black_live_eye_arr[], int white_live_eye_arr[])
{
	this->live = 0;
	list<stone*> try_position;
	int forbid = 0;
	int forbiden[5] = {-1,-1,-1,-1,-1};
	int S[BOARD_SIZE*BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) S[i] = 0;
	for (std::list<stone*>::iterator itor = this->stones.begin(); itor != this->stones.end(); ++itor)
	{
		for (int i = 0; i < 4; i++)
		{
			int row = (*itor)->row + deltai[i];
			int col = (*itor)->col + deltaj[i];
			int index = POS(row,col);
			stone* curr = chain_block->block_board->main_board[index];
			if (chain_block->block_board->on_board(row, col))
			{
				if (curr->color == EMPTY)
					{
						if (S[index] != 0) continue;
						S[index] = 1;
						curr->color = OTHER_COLOR(chain_block->color);
						if (curr->eat_this_chain(this))
						{
							try_position.push_back(curr);
							std::list<stone*>::iterator itor2 = try_position.begin();
							while (itor2 != try_position.end())
							{
								
								if (chain_block->color == BLACK){
									black_live_eye_arr[POS((*itor2)->row, (*itor2)->col)] = 1;	//try_position不是白棋的真眼
								}
								else{
									white_live_eye_arr[POS((*itor2)->row, (*itor2)->col)] = 1;	//try_position不是黑棋的真眼
								}
								(*itor2)->color = EMPTY;
								try_position.erase(itor2++);
							}
							this->live = 0;
							return 1;
						}
						else if (curr->can_live(this))
						{
							try_position.push_back(curr);
						}
						else //Neither to eat this chain nor to live
						{
							curr->color = EMPTY;
							S[index] = 2;
						}
				}
			}
		}
	}
	int count = 0;
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
		if (S[i] == 2) count++;
	}
	if (count < 2)
	{
		//S[i]周围不能是live eye
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
			if (S[i] == 2) {
				if (chain_block->color == BLACK){
					black_live_eye_arr[i] = 1;	//try_position不是白棋的真眼
				}
				else{
					white_live_eye_arr[i] = 1;	//try_position不是黑棋的真眼
				}
				queue<int> q;
				q.push(i);
				not_live_eye_bfs(this, OTHER_COLOR(this->chain_block->color), q, black_live_eye_arr, white_live_eye_arr);
			}
		}
		std::list<stone*>::iterator itor2 = try_position.begin();
		while (itor2 != try_position.end())
		{

			if (chain_block->color == BLACK){
				black_live_eye_arr[POS((*itor2)->row, (*itor2)->col)] = 1;	//try_position不是白棋的真眼
			}
			else{
				white_live_eye_arr[POS((*itor2)->row, (*itor2)->col)] = 1;	//try_position不是黑棋的真眼
			}
			(*itor2)->color = EMPTY;
			try_position.erase(itor2++);
		}
		this->live = 0;
	}
	else 
	{
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
			if (S[i] == 2) {
				if (chain_block->color == BLACK){
					if (black_live_eye_arr[i] != 1) black_live_eye_arr[i] = 2;	//可能是白棋的真眼
				}
				else{
					if (white_live_eye_arr[i] != 1) white_live_eye_arr[i] = 2;	//可能是黑棋的真眼
				}
				queue<int> q;
				q.push(i);
				live_eye_bfs(this, OTHER_COLOR(this->chain_block->color), q, black_live_eye_arr, white_live_eye_arr);
			}
		}
		this->live = 1;
	}
	std::list<stone*>::iterator itor = try_position.begin();
	while (itor != try_position.end())
	{
		(*itor)->color = EMPTY;
		try_position.erase(itor++);
	}
	return 1;
}
bool stone::eat_this_chain(chain* itor)
{
	for (int k = 0; k < 4; k++) 
	{
		int ai = row + deltai[k];
		int aj = col + deltaj[k];
		if (stone_board->on_board(ai, aj) 
			&& stone_board->get_board(ai, aj)->color == OTHER_COLOR(color)
			&& !stone_board->has_additional_liberty(ai, aj, row, col))
		{
			if (stone_board->main_board[POS(ai,aj)]->stone_chain == itor )
			return 1;
		}
	}
	return 0;
}
bool stone::can_live(chain* itor)
{
	int visited[BOARD_SIZE*BOARD_SIZE];
	memset(visited, 0, sizeof(visited));
	int a = 0;
	list <int> q1;
	q1.push_back(POS(this->row, this->col));
	while (q1.size() != EMPTY)
	{
		int top = q1.front();
		q1.pop_front();
		if (visited[top] == 1){ continue; }
		visited[top] = 1;
		for (int k = 0; k < 4; k++)
		{
			if (stone_board->on_board(I(top) + deltai[k], J(top) + deltaj[k]))
			{
				int index = POS(I(top) + deltai[k], J(top) + deltaj[k]);
				if (stone_board->main_board[index]->color == EMPTY)
				{
					return true;
				}
				else if (stone_board->main_board[index]->color == (itor)->chain_block->color)
				{
					continue;
				}
				else if (stone_board->main_board[index]->color == OTHER_COLOR((itor)->chain_block->color))
				{
					q1.push_back(index);
				}
			}
		}
	}
	return false;
}
//INFLUENCE & REDISTRIBUTION
void stone::stone_influence()
{
	int center = 0;
	int position = 0;
	if (stone_block->color == BLACK)
	{
		center = INF_CENTRE*(stone_board->range[row*BOARD_SIZE + col] * 0.05 + 1);
	}
	else { center = -INF_CENTRE*(stone_board->range[row*BOARD_SIZE + col] * 0.05 + 1); }
	int r_min = 0, r_max = 0;
		r_min = row - influence_distance;
		r_max = row + influence_distance;
		for (int i = r_min; i <= r_max; i++)
		{
			if (i < 0 || i > BOARD_SIZE - 1){ continue; }
			int c_min = col - (influence_distance - abs(row - i)), c_max = col + (influence_distance - abs(row - i));
			for (int j = c_min; j <= c_max; j++)
			{
				if (j < 0 || j > BOARD_SIZE - 1){ continue; }
				int dist = abs(j - col) + abs(i - row);
				if (stone_board->main_board[POS(i, j)]->color != EMPTY) { 
					continue; 
				}
				stone_board->main_board[POS(i, j)]->s_influence += center / (int)(pow(2, dist));
			}
		}
}
bool stone::empty_redistribution()
{
	int except = -1;
	int target;
	if (s_influence > 0){ target = BLACK; }
	else if (s_influence < 0){ target = WHITE; }
	else {
		if (stone_block == NULL) return 1;
		else
		{
			stone_block->empty.remove(this);
			stone_block = NULL;
			return 1;
		}
	}
	//BFS
	int visited[BOARD_SIZE*BOARD_SIZE];
	memset(visited, 0, sizeof(visited));
	queue <int> q;
	q.push(POS(row, col));
	visited[POS(row, col)] = 1;
	while (!q.empty())
	{
		int top;
		int mark = 0;
		top = q.front();
		q.pop();
		for (int k = 0; k < 4; k++)
		{
			if (stone_board->on_board(I(top) + deltai[k], J(top) + deltaj[k]))
			{
				int index = POS(I(top) + deltai[k], J(top) + deltaj[k]);
				if (stone_board->main_board[index]->color == EMPTY)
				{
					if (visited[index] == 0)	{
						visited[index] = 1;
						q.push(index);
					}
				}
				else if (stone_board->main_board[index]->color == OTHER_COLOR(target) && color == EMPTY && mark == 0)
				{
					except = index;
					mark = 1;
					continue;
				}
				else if (stone_board->main_board[index]->color == OTHER_COLOR(target) && color == EMPTY && mark == 1)
				{
					continue;
				}
				else if (stone_board->main_board[index]->color == target && color == EMPTY)
				{
					//IF: check if it is already in
					for (std::list<stone*>::iterator itor2 = stone_board->main_board[index]->stone_block->empty.begin(); itor2 != stone_board->main_board[index]->stone_block->empty.end(); ++itor2)
					{
						if (POS((*itor2)->row, (*itor2)->col) == POS(row, col)){
							return 1;
						}
					}
					//ELSE: delete the orignial term, add to the new list
					if (this->stone_block != NULL)
					{
						for (std::list<stone*>::iterator itor = this->stone_block->empty.begin(); itor != this->stone_block->empty.end(); itor++)
						{
							if (POS((*itor)->row, (*itor)->col) == POS(row, col))
							{
								this->stone_block->empty.erase(itor);
								break;
							}
						}
					}
					stone_board->main_board[index]->stone_block->empty.push_back(this);
					//mark the stone block
					this->stone_block = stone_board->main_board[index]->stone_block;
					return 1;
				}
			}
		}
	}

	if (except == -1)
	{
		return 0;
	}	//Impossible

	//except: the influence is negative, but it's surrounded by blacks
	//It's already in the "except" black block
	for (std::list<stone*>::iterator itor = stone_board->main_board[except]->stone_block->empty.begin(); itor != stone_board->main_board[except]->stone_block->empty.end(); ++itor)
	{
		if (POS((*itor)->row, (*itor)->col) == POS(row, col)){ return 1; }
	}
	//ELSE: delete the orignial term, add to the new list
	this->stone_block->empty.remove(this);

	stone_board->main_board[except]->stone_block->empty.push_back(this);
	//mark the stone block
	this->stone_block = stone_board->main_board[except]->stone_block;
	return 1;
}
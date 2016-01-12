#include "new_board.h"
void DECOMPOSE_BLOCKS_BFS(board_t * board, block* A, block* B, int color, queue<int> &q){//block的自解函数中调用的BFS
	int S[BOARD_SIZE*BOARD_SIZE];
	memset(S, 0, sizeof(S));
	S[q.front()] = 1; //把node添加进入搜索集中
	while (!q.empty()){
		int node = q.front();
		q.pop();
		stone* st = board->get_board(I(node), J(node));
		if (st->color == EMPTY && ((color == BLACK) && (st->s_influence > INFLUENCE_CONNECT_BLOCK) || (color == WHITE) && (st->s_influence < -INFLUENCE_CONNECT_BLOCK))
			|| (st->color != EMPTY) && (st->stone_block == B)){//当node时影响力大于10的空点或者属于棋块B的点认为是可以到达的路径,我们都对node的子节点进行扩展
			for (int k = 0; k < 4; k++) {
				// de的所有子节点
				int a = I(node) + deltai[k];
				int b = J(node) + deltaj[k];
				if (board->on_board(a, b) && S[POS(a, b)] != 1){//如果node的子节点在棋盘上且不在搜索集中,扩展它，加入队列
					q.push(POS(a, b));
					S[POS(a, b)] = 1; //把node添加进入搜索集中
				}
			}
		}
		else if ((st->color != EMPTY) && (st->stone_block == A)){//这里也可以写成st->stone_block != B如果别的数据结构都是对的，那么两种写法没有区别
			B->absorb_a_chain(*(st->stone_chain));
			q.push(POS(st->row, st->col));
		}
	}
}
void block_decompose(board_t * board, block* A){//block的自解函数，把自己划分成几个独立的块
	queue<int> q;
	chain* c = *(A->chains.begin());
	A->remove_chain_without_deleting(*c);//把c从A中删除
	block* B = new block(*c, *(A->block_board));	//产生一个新的block
	list<stone*>::iterator itor = A->empty.begin();
	q.push(POS((*(c->stones).begin())->row, (*(c->stones).begin())->col));
	DECOMPOSE_BLOCKS_BFS(A->block_board, A, B, A->color,q);
	if (A->chains.size() != EMPTY)
		block_decompose(board, A);
	else{
		delete A;
	}
}
void decompose_opponent_blocks(board_t *board, stone * the_stone){//一步棋划分对方棋块
	int x = the_stone->row;
	int y = the_stone->col;
	list<block*> divide_blocks;
	for (int i = x - 4; i <= x + 4; i++){
		for (int j = y - 4; j <= y + 4; j++){
			if (board->on_board(i, j) && (abs(i - x) + abs(j - y) <= 4) && board->
				main_board[POS(i, j)]->color == OTHER_COLOR(board->main_board[POS(x, y)]->color)){
				list<block* >::iterator findblock = find(divide_blocks.begin(), divide_blocks.end(), board->main_board[POS(i, j)]->stone_block);
				if (findblock == divide_blocks.end())
					divide_blocks.push_back(board->main_board[POS(i, j)]->stone_block);
			}
		}
	}
	while (divide_blocks.size() != EMPTY){
		block* temp_block = divide_blocks.front();
		divide_blocks.pop_front();
		block_decompose(board, temp_block);
	}
}
void COMBINE_BLOCKS_BFS(board_t * board, block* blk, int color, queue<int> &q){
	int S[BOARD_SIZE*BOARD_SIZE];
	memset(S, 0, sizeof(S));
	S[q.front()] = 1; //把node添加进入搜索集中
	while (!q.empty()){
		int node = q.front();
		q.pop();
		stone* st = board->get_board(I(node), J(node));
		if (st->color == EMPTY && ((color == BLACK) && (st->s_influence > INFLUENCE_CONNECT_BLOCK) || (color == WHITE) && (st->s_influence < -INFLUENCE_CONNECT_BLOCK))
			|| (st->color != EMPTY) && (st->stone_block == blk)){
			for (int k = 0; k < 4; k++) {
				// de的所有子节点
				int a = I(node) + deltai[k];
				int b = J(node) + deltaj[k];
				if (board->on_board(a, b) && S[POS(a, b)] != 1){//如果node的子节点在棋盘上且不在搜索集中,扩展它，加入队列
					q.push(POS(a, b));
					S[POS(a,b)] = 1; //把node添加进入搜索集中
				}
			}
		}
		else if ((st->color == color) && (st->stone_block != blk)){
			blk->combine_block(*(st->stone_block));
			q.push(node);
		}
	}
}
void explore_surrounding_block(stone* st){//一个新落下的棋子属于的块与其它块合并
	queue<int> q;
	q.push(POS(st->row, st->col));
	COMBINE_BLOCKS_BFS(st->stone_board, st->stone_block, st->color,q);
}

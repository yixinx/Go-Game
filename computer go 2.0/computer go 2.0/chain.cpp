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

chain::chain(chain & old_chain, block & new_block)
{
	length = old_chain.length;
	chain_block = &new_block;
	list<stone *>::iterator iter = old_chain.stones.begin();
	for (iter; iter != old_chain.stones.end(); iter++)
	{
		int tmp_row = (*iter)->row;
		int tmp_col = (*iter)->col;
		new_block.block_board->main_board[POS(tmp_row, tmp_col)]->color = old_chain.chain_block->color;
		new_block.block_board->main_board[POS(tmp_row, tmp_col)]->type = EMPTY_P;
		new_block.block_board->main_board[POS(tmp_row, tmp_col)]->stone_block = &new_block;
		new_block.block_board->main_board[POS(tmp_row, tmp_col)]->stone_chain = this;
		this->stones.push_back(new_block.block_board->main_board[POS(tmp_row, tmp_col)]);
	}
}

chain::chain(stone & new_stone, board_t &board)
{
	stones.push_back(&new_stone);
	new_stone.stone_chain = this;
	chain_block = new_stone.stone_block;
	chain_block->chains.push_back(this);
	length = 1;
	live = 0;
	num = 0;
}

void chain::combine_chain(chain * chain2)
{
	if (chain2 == NULL) return;
	list<stone *>::iterator iter = chain2->stones.begin();

	/* change the stone_block of the stones in chain2 */
	while (iter != chain2->stones.end())
	{
		(*iter)->stone_chain = this;
		(*iter)->stone_block = this->chain_block;
		stones.push_back(*iter);
		chain2->stones.erase(iter++);
	}
	length += chain2->length;
	
	/* merge the block if chain2 is in a different block */
	if (chain2->chain_block != chain_block)
	{
		chain_block->combine_block(*(chain2->chain_block));
	}
	else {
		delete chain2;
	}
}

void chain::absorb_a_stone(stone * the_stone)
{
	stones.push_back(the_stone);
	length += 1;
	the_stone->stone_chain = this;
	the_stone->stone_block = chain_block;
}


chain::~chain()
{
	list<stone *>::iterator iter = stones.begin();
	while (iter != stones.end())
	{
		(*iter)->color = EMPTY;
		(*iter)->stone_chain = NULL;
		(*iter)->stone_block = NULL;
		stones.erase(iter++);
	}
	if (chain_block != NULL)
		chain_block->chains.remove(this);
}
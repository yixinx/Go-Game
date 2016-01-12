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



void
block::init_basic(int the_color)
{
	freedom = 0;
	color = the_color;
	influence = 0;
	//safety = 0;
}



block::block(block &old_block, board_t & new_board)
{
	init_basic(EMPTY);
	block_board = &new_board;
	//board.blocks.push_back(this);
	color = old_block.color;
	int it = 0;
	while (it != NULL)
	{
		weight[it] = old_block.weight[it];
	}
	freedom = old_block.freedom;
	influence = old_block.influence;
	true_eyes = old_block.true_eyes;
	fake_eyes = old_block.fake_eyes;
	tigers = old_block.tigers;
	weaks = old_block.weaks;

	list <stone *>::iterator empty_it = old_block.empty.begin();
	for ( empty_it ; empty_it != old_block.empty.end(); empty_it++)
	{
		int temp_row = (*empty_it)->row;
		int temp_col = (*empty_it)->col;
		empty.push_back(new_board.main_board[POS(temp_row, temp_col)]);
		new_board.main_board[POS(temp_row, temp_col)]->stone_block = this;
		//new_board.main_board[POS(temp_row, temp_col)]->type = EMPTY_P;
	}

	list<chain *>::iterator chain_it = old_block.chains.begin();
	for (chain_it; chain_it != old_block.chains.end(); chain_it++)
	{
		chain * new_chain = new chain(**chain_it, *this);
		chains.push_back(new_chain);
	}

}

block::block(chain & new_chain, board_t & board)
{
	init_basic( (*(new_chain.stones.begin()))->color );
	block_board = &board;
	board.blocks.push_back(this);
	chains.push_back(&new_chain);
	new_chain.chain_block = this;
	list<stone *>::iterator iter = new_chain.stones.begin();
	for (iter; iter != new_chain.stones.end(); iter++)
	{
		(*iter)->stone_block = this;
	}
}

block::block(stone & new_stone, board_t & board)
{
	init_basic(new_stone.color);
	new_stone.stone_block = this;
	chain * new_chain = new chain(new_stone,board);
	block_board = &board;
	board.blocks.push_back(this);
}

void block::absorb_a_chain(chain & chain)
{
	if (chain.stones.size() != 0){
		list<stone *>::iterator iter = chain.stones.begin();
		while (iter != chain.stones.end())
		{
			(*iter++)->stone_block = this;
		}
	}
	chain.chain_block->chains.remove(&chain);
	chains.push_back(&chain);
	chain.chain_block = this;
}
void block::remove_chain_without_deleting(chain &chain)
{
	chains.remove(&chain);
	list<stone *>::iterator iter = chain.stones.begin();
	for (iter; iter != chain.stones.end(); iter++)
	{
		(*iter)->stone_block = NULL;
		//stones.remove(*iter);
	}
	chain.chain_block = NULL;
}
void block::absorb_an_empty(int i, int j)
{
	block_board->main_board[POS(i, j)]->stone_block = this;
	empty.push_back(block_board->main_board[POS(i, j)]);
}

void block::remove_an_empty(int i, int j)
{
	block_board->main_board[POS(i, j)]->stone_block = NULL;
	empty.remove(block_board->main_board[POS(i, j)]);
}

void block::combine_block(block & block2)
{
	/*list<stone* >::iterator iter = block2.empty.begin();
	while (iter != block2.empty.end()){
		(*iter)->stone_block = this;
		empty.push_back(*iter);
		block2.empty.erase(iter++);
	}*/
	if (block2.chains.size() != 0){
		list<chain *>::iterator itor = block2.chains.begin();
		while (itor != block2.chains.end())
		{
			if (!(*itor)->stones.empty())
				absorb_a_chain(**(itor++));
			else
				itor++;
		}
	}
	delete &block2;
}

block::~block()
{
	list<chain *>::iterator iter = chains.begin();
	while (iter != chains.end())
	{
		chain* temp = *iter;
		chains.erase(iter++);
		delete temp;
	}
	list<stone *>::iterator iter2 = empty.begin();
	while (iter2 != empty.end())
	{
		(*iter2)->stone_block = NULL;
		empty.erase(iter2++);
	}
	block_board->blocks.remove(this);
}
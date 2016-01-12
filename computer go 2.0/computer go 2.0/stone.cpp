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


stone::stone(int r, int c, board_t * new_board)
{
	row = r;
	col = c;
	color = EMPTY;
	type = EMPTY_P;
	s_influence = 0;
	stone_board = new_board;
}

stone::stone(stone * old_stone, board_t * new_board)
{
	row = old_stone->row;
	col = old_stone->col;
	stone_block = NULL;
	stone_chain = NULL;
	stone_board = new_board;
	color = old_stone->color;
	s_influence = old_stone->s_influence;
	type = old_stone->type;
}

stone::~stone()
{
	color = EMPTY;
	stone_block = NULL;
	stone_chain = NULL;
}

// TODO: add to block
void stone::add_to_chain(board_t &curr_board)
{
	int ai = 0, aj = 0;
	int flag = 0;
	chain * last_chain = NULL;

	for (int k = 0; k < 4; k++)
	{
		
		ai = row + deltai[k];
		aj = col + deltaj[k];

		if (!stone_board->on_board(ai, aj)) continue;

		chain * temp_chain = stone_board->main_board[POS(ai, aj)]->stone_chain;

		if (temp_chain != NULL && (*(temp_chain->stones.begin()))->color == color)
			if (flag == 0)
			{
				temp_chain->absorb_a_stone(this);
				flag = 1;
				last_chain = this->stone_chain;
			}
			else 
			{
				if (temp_chain != this->stone_chain)
				{
					temp_chain->combine_chain(last_chain);
					last_chain = this->stone_chain;
				}
			}			
	}

	if (flag == 0)
	{
		block * new_block = new block(*this, curr_board);
	}
}

/* remove itself means change the stone_block, stone_chain,color, type, but will not destroy the object. It is called in the destructor for the chain */
void
stone::remove_itself()
{
	/*
	// update the chain, 
	stone_chain->stones.remove(this);
	// TODO: if the chain becomes empty, and then maybe the block becomes empty, we should delete the chain or even the block
	if (stone_chain->stones.empty())
	{
		stone_block->chains.remove(stone_chain);
		delete stone_chain;
		if (stone_block->chains.empty())
		{
			stone_block->stones.remove(this);
			stone_block->block_board->blocks.remove(stone_block);
			delete stone_block;
		}
		else stone_block->stones.remove(this);

	}
	else
	{
		// update the block that the chain belongs to
		stone_block->stones.remove(this);
		stone_block->block_board->blocks.remove(stone_block);
		delete stone_block;
	}
	*/

	stone_block = NULL;
	stone_chain = NULL;
	color = EMPTY;
	type = EMPTY_P;

}

stone_type stone::change_type(stone_type the_type)
{
	return type = the_type;
}
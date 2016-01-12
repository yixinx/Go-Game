#define _CRT_SECURE_NO_WARNINGS

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
* This is Brown, a simple go program.                           *
*                                                               *
* Copyright 2003 and 2004 by Gunnar Farnebäck.                  *
*                                                               *
* Permission is hereby granted, free of charge, to any person   *
* obtaining a copy of this file gtp.c, to deal in the Software  *
* without restriction, including without limitation the rights  *
* to use, copy, modify, merge, publish, distribute, and/or      *
* sell copies of the Software, and to permit persons to whom    *
* the Software is furnished to do so, provided that the above   *
* copyright notice(s) and this permission notice appear in all  *
* copies of the Software and that both the above copyright      *
* notice(s) and this permission notice appear in supporting     *
* documentation.                                                *
*                                                               *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY     *
* KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE    *
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR       *
* PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO      *
* EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS  *
* NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR    *
* CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING    *
* FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF    *
* CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT    *
* OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS       *
* SOFTWARE.                                                     *
*                                                               *
* Except as contained in this notice, the name of a copyright   *
* holder shall not be used in advertising or otherwise to       *
* promote the sale, use or other dealings in this Software      *
* without prior written authorization of the copyright holder.  *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>  /* for rand() and srand() */
#include <time.h>
#include <process.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <shellapi.h>
#pragma comment(lib, "user32.lib")
#include "new_board.h"
#include "gtp.h"
#include "model.h"

using namespace std;

/* Forward declarations. */
static int gtp_protocol_version(char *s);
static int gtp_name(char *s);
static int gtp_version(char *s);
static int gtp_known_command(char *s);
static int gtp_list_commands(char *s);
static int gtp_quit(char *s);
static int gtp_boardsize(char *s);
static int gtp_clear_board(char *s);
static int gtp_komi(char *s);
static int gtp_fixed_handicap(char *s);
static int gtp_place_free_handicap(char *s);
static int gtp_set_free_handicap(char *s);
static int gtp_play(char *s);
static int gtp_genmove(char *s);
static int gtp_final_score(char *s);
static int gtp_final_status_list(char *s);
static int gtp_showboard(char *s);
//static int gtp_mc(char *s);
static int gtp_polling(char *s);
/* List of known commands. */
static struct gtp_command commands[] = {
	{ "protocol_version", gtp_protocol_version },
	{ "name", gtp_name },
	{ "version", gtp_version },
	{ "known_command", gtp_known_command },
	{ "list_commands", gtp_list_commands },
	{ "quit", gtp_quit },
	{ "boardsize", gtp_boardsize },
	{ "clear_board", gtp_clear_board },
	{ "komi", gtp_komi },
	{ "fixed_handicap", gtp_fixed_handicap },
	{ "place_free_handicap", gtp_place_free_handicap },
	{ "set_free_handicap", gtp_set_free_handicap },
	{ "play", gtp_play },
	{ "genmove", gtp_genmove },
	{ "final_score", gtp_final_score },
	{ "final_status_list", gtp_final_status_list },
	{ "showboard", gtp_showboard },
	//{ "MC", gtp_mc },
	{ "spin", gtp_polling },
	{ NULL, NULL }
};

board_t board;
int x;
int y;
int other_color;
int stone_count;
class A{
public:
	int a;
	A(int i);
};
A::A(int i){
	a = i;
}
void test(){
	for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
		board_t* test_board;
		test_board = new board_t(board);
		test_board->play_move(I(i), J(i), BLACK);
		delete test_board;
	}
}
int main(int argc, char **argv)
{
	/*test();
	board.play_move(0, 0, 1);
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);
	test();
	_CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&s3, &s1, &s2)){
		_CrtMemDumpStatistics(&s3);//ÄÚ´æÐ¹Â¶
		printf("memory leak!\n");
		exit(-1);
	}
	else
		;//ÎÞÐ¹Â¶

	*/
	unsigned int random_seed = (unsigned int) time(NULL);
	stone_count = 0;
	/* Optionally a random seed can be passed as an argument to the program. */
	if (argc > 1)
		sscanf(argv[1], "%u", &random_seed);
	srand(random_seed);

	/* Make sure that stdout is not block buffered. */
	setbuf(stdout, NULL);

	/* Initialize the board. */
	//board.init_brown(board.board_size);


	/* Inform the GTP utility functions about the initial board size. */
	gtp_internal_set_boardsize(board.board_size);
	board.board_size = BOARD_SIZE;
	//simboard.board_size = BOARD_SIZE;
	board.set_init_board();
	//simboard.init_brown(BOARD_SIZE);
	//gtp_main_loop(commands, stdin, NULL);
	FILE *in_stream, *out_stream;
	if ((in_stream = freopen("new_board4.txt", "r", stdin)) == NULL)
		exit(-1);
	//if ((out_stream = freopen("new_board_out.txt", "w", stdout)) == NULL)
		//exit(-1);
	 //Process GTP commands. 
	gtp_main_loop(commands, in_stream, NULL);
	fclose(in_stream);
	//fclose(out_stream);
	return 0;
}

void WINAPI GetSystemInfo(
	__out  LPSYSTEM_INFO lpSystemInfo
	);

SYSTEM_INFO siSysInfo;

/* We are talking version 2 of the protocol. */
static int
gtp_protocol_version(char *s)
{
	return gtp_success("2");
}
/*
static int
gtp_mc(char *s)
{
	clock_t startTime = clock();
	srand((unsigned long)time(NULL));
	GetSystemInfo(&siSysInfo);
	unsigned int procNum = 4*siSysInfo.dwNumberOfProcessors;
	msgpasser_t *msg = new msgpasser_t[procNum];
	HANDLE *flag = new HANDLE[procNum];
	simple_board_t simboard(board);
	for (unsigned int t = 0; t < procNum; t++)
	{
		msg[t].i = t % BOARD_SIZE;
		msg[t].j = (BOARD_SIZE - t ) % BOARD_SIZE;
		msg[t].color = 1;
		msg[t].wins = 0;
		msg[t].seed = rand() % 77 + 31;
		msg[t].board_here = &simboard;
	}
	for (unsigned int i = 0; i < procNum; ++i)
	{
		flag[i] = (HANDLE)_beginthread(MtC, 0, &(msg[i]));
	}
	WaitForMultipleObjects(procNum, flag, true, INFINITE);
	for (unsigned int i = 0; i < procNum; i++)
	{
		printf("%d\n", msg[i].wins);
	}
	delete[] msg;
	delete[] flag;
	cout << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;
	return 0;
}
*/
static int
gtp_name(char *s)
{
	return gtp_success("Mont Based Brown");
}

static int
gtp_version(char *s)
{
	return gtp_success(VERSION_STRING);
}

static int
gtp_known_command(char *s)
{
	int i;
	char command_name[GTP_BUFSIZE];

	/* If no command name supplied, return false (this command never
	* fails according to specification).
	*/
	if (sscanf(s, "%s", command_name) < 1)
		return gtp_success("false");

	for (i = 0; commands[i].name; i++)
		if (!strcmp(command_name, commands[i].name))
			return gtp_success("true");

	return gtp_success("false");
}

static int
gtp_list_commands(char *s)
{
	int i;

	gtp_start_response(GTP_SUCCESS);

	for (i = 0; commands[i].name; i++)
		gtp_printf("%s\n", commands[i].name);

	gtp_printf("\n");
	return GTP_OK;
}

static int
gtp_quit(char *s)
{
	gtp_success("");
	return GTP_QUIT;
}

static int
gtp_boardsize(char *s)
{
	int boardsize;

	if (sscanf(s, "%d", &boardsize) < 1)
		return gtp_failure("boardsize not an integer");

	if (boardsize < MIN_BOARD || boardsize > BOARD_SIZE)
		return gtp_failure("unacceptable size");

	board.board_size = boardsize;
	gtp_internal_set_boardsize(boardsize);
	//board.init_brown(boardsize);
	board.set_init_board();
	return gtp_success("");
}

static int
gtp_clear_board(char *s)
{
	board.clear_board();
	return gtp_success("");
}

static int
gtp_komi(char *s)
{
	if (sscanf(s, "%f", &(board.komi)) < 1)
		return gtp_failure("komi not a float");

	return gtp_success("");
}

/* Common code for fixed_handicap and place_free_handicap. */
static int
place_handicap(char *s, int fixed)
{
	int handicap;
	int m, n;
	int first_stone = 1;

	if (!board.board_empty())
		return gtp_failure("board not empty");

	if (sscanf(s, "%d", &handicap) < 1)
		return gtp_failure("handicap not an integer");

	if (handicap < 2)
		return gtp_failure("invalid handicap");

	if (fixed && !board.valid_fixed_handicap(handicap))
		return gtp_failure("invalid handicap");

	if (fixed)
		board.place_fixed_handicap(handicap);
	else
		board.place_free_handicap(handicap);

	gtp_start_response(GTP_SUCCESS);
	for (m = 0; m < board.board_size; m++)
		for (n = 0; n < board.board_size; n++)
			if (board.get_board(m, n)->color != EMPTY) {
				if (first_stone)
					first_stone = 0;
				else
					gtp_printf(" ");
				gtp_mprintf("%m", m, n);
			}
	return gtp_finish_response();
}

static int
gtp_fixed_handicap(char *s)
{
	return place_handicap(s, 1);
}

static int
gtp_place_free_handicap(char *s)
{
	return place_handicap(s, 0);
}

static int
gtp_set_free_handicap(char *s)
{
	int i, j;
	int n;
	int handicap = 0;

	if (!board.board_empty())
		return gtp_failure("board not empty");

	while ((n = gtp_decode_coord(s, &i, &j)) > 0) {
		s += n;

		if (board.get_board(i, j)->color != EMPTY) {
			board.clear_board();
			return gtp_failure("repeated vertex");
		}

		board.play_move(i, j, BLACK);
		handicap++;
	}

	if (sscanf(s, "%*s") != EOF) {
		board.clear_board();
		return gtp_failure("invalid coordinate");
	}

	if (handicap < 2 || handicap >= board.board_size * board.board_size) {
		board.clear_board();
		return gtp_failure("invalid handicap");
	}

	return gtp_success("");
}

static int
gtp_play(char *s)
{
	int i, j;
	int color = EMPTY;

	if (!gtp_decode_move(s, &color, &i, &j))
		return gtp_failure("invalid color or coordinate");

	if (!board.legal_move(i, j, color))
		return gtp_failure("illegal move");
	x = i;
	y = j;
	int a = POS(i, j);
	other_color = color;
	//printf("%d, %d, %d\n", i, j, color);
	board.play_move(i, j, color);
	stone_count++;
	//gtp_showboard(NULL);
	//board.print_all_block_or_chain(3);
	return gtp_success("");
}

static int
gtp_genmove(char *s)
{
	int i, j;
	int color = EMPTY;

	if (!gtp_decode_color(s, &color))
		return gtp_failure("invalid color");

	train_model(&i, &j, color);

	x = i;
	y = j;
	other_color = color;
	//printf("%d, %d, %d\n", i, j, color);
	board.play_move(i, j, color);
	stone_count++;
	/*if (stone_count >= 15) {
		for (int i = 0; i < board.board_size*board.board_size; i++){
			board.range[i] = 1;
		}
	}*/
	gtp_start_response(GTP_SUCCESS);
	gtp_mprintf("%m", i, j);
	//gtp_showboard(NULL);
	//board.print_all_block_or_chain(2);
	return gtp_finish_response();
}

/* Compute final score. We use area scoring since that is the only
* option that makes sense for this move generation algorithm.
*/
static int
gtp_final_score(char *s)
{
	double score = board.komi;
	int i, j, final_status[BOARD_SIZE*BOARD_SIZE];

	board.compute_final_status(final_status);
	for (i = 0; i < board.board_size; i++)
		for (j = 0; j < board.board_size; j++) {
			int status = board.get_final_status(i, j,final_status);
			if (status == BLACK_TERRITORY)
				score--;
			else if (status == WHITE_TERRITORY)
				score++;
			else if ((status == ALIVE) ^ (board.get_board(i, j)->color == WHITE))
				score--;
			else
				score++;
		}

	if (score > 0.0)
		return gtp_success("W+%3.1f", score);
	if (score < 0.0)
		return gtp_success("B+%3.1f", -score);
	return gtp_success("0");
}

static int
gtp_final_status_list(char *s)
{
	int n;
	int i, j;
	int status = UNKNOWN;
	int final_status[BOARD_SIZE*BOARD_SIZE];
	char status_string[GTP_BUFSIZE];
	int first_string;

	if (sscanf(s, "%s %n", status_string, &n) != 1)
		return gtp_failure("missing status");

	if (!strcmp(status_string, "alive"))
		status = ALIVE;
	else if (!strcmp(status_string, "dead"))
		status = DEAD;
	else if (!strcmp(status_string, "seki"))
		status = SEKI;
	else
		return gtp_failure("invalid status");

	board.compute_final_status(final_status);

	gtp_start_response(GTP_SUCCESS);

	first_string = 1;
	for (i = 0; i < board.board_size; i++)
		for (j = 0; j < board.board_size; j++)
			if (board.get_final_status(i, j,final_status) == status) {
				int stonei[BOARD_SIZE * BOARD_SIZE];
				int stonej[BOARD_SIZE * BOARD_SIZE];
				int num_stones = 0;
				chain * curr_chain = board.get_chain(i, j);
				list<stone *>::iterator iter = curr_chain->stones.begin();
				/* Clear the status so we don't find the string again. */
				for (iter; iter != curr_chain->stones.end(); iter++) {
					board.set_final_status((*iter)->row, (*iter)->col, UNKNOWN, final_status);
					num_stones++;
					stonei[num_stones] = (*iter)->row;
					stonej[num_stones] = (*iter)->col;
				}
				if (first_string)
					first_string = 0;
				else
					gtp_printf("\n");

				gtp_print_vertices(num_stones, stonei, stonej);
			}

	return gtp_finish_response();
}

/* Write a row of letters, skipping 'I'. */
static void
letters(void)
{
	int i;

	printf("  ");
	for (i = 0; i < board.board_size; i++)
		printf(" %c", 'A' + i + (i >= 8));
}

static int
gtp_showboard(char *s)
{
	int i, j;
	int symbols[3] = { '.', 'O', 'X' };

	gtp_start_response(GTP_SUCCESS);
	gtp_printf("\n");

	letters();

	for (i = 0; i < board.board_size; i++) {
		printf("\n%2d", board.board_size - i);

		for (j = 0; j < board.board_size; j++)

			printf(" %c", symbols[board.get_board(i, j)->color]);

		printf(" %d", board.board_size - i);
	}

	printf("\n");
	letters();
	return gtp_finish_response();
}

static int gtp_polling(char *s)
{
	HANDLE hand1, fhand;
	hand1 = CreateFile((LPCTSTR)"local", (GENERIC_READ | GENERIC_WRITE), FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	fhand = CreateFileMapping(hand1, NULL, PAGE_READWRITE, 0, 256, (LPCTSTR)"objspe");
	if (fhand == NULL)
	{
		//printf("creat mapping obj failed\n");
		return 0;
	}
	//printf("%d\n", fhand);
	LPVOID fileptr;
	fileptr = MapViewOfFile(fhand, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == fileptr)
	{
		//printf("viewing failed\n");
		return 0;
	}
	//printf("%d\n", fileptr);
	ShellExecuteA(NULL, "open", ".\\extra\\GoGui.exe", NULL, NULL, 0);
	DWORD temp = 4;
	//std::cin >> temp;
	Sleep(5000);
	*((LPDWORD)fileptr) = temp;
	return 0;
}


/*
* Local Variables:
* tab-width: 8
* c-basic-offset: 2
* End:
*/

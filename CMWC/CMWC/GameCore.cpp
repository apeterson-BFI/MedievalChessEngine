#include "GameCore.h"
#include <cstdio>__msvc_all_public_headers.hpp
#include <string>activecf.h

char columnLetters[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

// 1 row = 2 hex characters
// 16 hex = board
u64 whiteStart		= 0x000000000000FFFFULL;
u64 blackStart		= 0xFFFF000000000000ULL;
u64 pawnsStart		= 0x00F000000000FF00ULL;	// 2nd row, 7th row 
u64 knightsStart	= 0x4200000000000042ULL;	// 1 index, 6 index on 1st row, 8th row: 2^1 = 2, 2^6 = 64 / 16 = 4.
u64 bishopsStart	= 0x2400000000000024ULL;	// 2nd index, 5th index = 2^2 = 4, 2^(5-4)=2
u64 rooksStart		= 0x1800000000000018ULL;	// 0st index, 7th index = 2^0 = 1, 2^(7-4)=8
u64 queenStart		= 0x8000000000000080ULL;	// 3rd index = 2^3 = 8
u64 kingStart		= 0x0100000000000001ULL;	// 4th index = 2^(4-4) = 1

void runAIOnlyGame(int depth)
{
	Board board;
	board.white = whiteStart;
	board.black = blackStart;
	board.knights = knightsStart;
	board.bishops = bishopsStart;
	board.rooks = rooksStart;
	board.queens = queenStart;
	board.kings = kingStart;
	board.turn = White;

	Sequence doSequence;

	while (getResult(&board) == 0)
	{
		chooseMove(&board, depth, &doSequence);
		make(&board, &doSequence);

		describeMove(&doSequence);
	}
}

void describeMove(Sequence* move)
{
	if (move->action1.cost != 0)
	{
		describeAction(&(move->action1));
	}

	if (move->action2.cost != 0)
	{
		describeAction(&(move->action2));
	}

	if (move->action3.cost != 0)
	{
		describeAction(&(move->action3));
	}
}

void describeAction(Action* action)
{
	if (action->actionType == Move)
	{
		printf("%c%d-%c%d (%d)", columnLetters[action->startc], action->startr + 1, columnLetters[action->endc], action->endr + 1, action->cost);
	}
	else if (action->actionType == Melee)
	{
		printf("melee %c%d (%d)", columnLetters[action->endc], action->endr + 1, action->cost);
	}
	else if (action->actionType == Cannon)
	{
		printf("%c%d cannon %c%d (%d)", columnLetters[action->startc], action->startr + 1, columnLetters[action->endc], action->endr + 1, action->cost);
	}
}
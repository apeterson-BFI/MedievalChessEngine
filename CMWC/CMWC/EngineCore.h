#pragma once

#include "Bitboard.h"

struct EngineEval
{
	int testvalue;
	int value;
};

void chooseMove(Board* pos, int depth, Sequence* bestSequence);
int alphabeta(Board* pos, int depth, int alpha, int beta, Sequence* bestSequence);
int heuristic(Board* pos);
bool chooseAction(Board* pos, EngineEval* eval, int depth, int* alpha, int* beta, Board* working, Sequence* sequence, int seqIndex, int remaining, int bMove, Sequence* bestSequence);
bool tryMove(Board* pos, EngineEval* eval, int depth, int* alpha, int* beta, Sequence* sequence, Sequence* bestSequence);
bool tryMove(Board* pos, EngineEval* eval, int depth, int* alpha, int* beta, Sequence* sequence, Sequence* bestSequence, int ev);
#include "EngineCore.h"
#include <cstddef>

int pieceValue[] = { 10, 30, 25, 45, 45, 40 };

void chooseMove(struct Board* pos, int depth, Sequence* bestSequence)
{
    int er = alphabeta(pos, depth, -2000, 2000, bestSequence);
}

/*
function alphabeta(node, depth, α, β, maximizingPlayer) is
    if depth = 0 or node is a terminal node then
        return the heuristic value of node
    if maximizingPlayer then
        value := −∞
        for each child of node do
            value := max(value, alphabeta(child, depth − 1, α, β, FALSE))
            α := max(α, value)
            if α ≥ β then
                break (* β cut-off *)
        return value
    else
        value := +∞
        for each child of node do
            value := min(value, alphabeta(child, depth − 1, α, β, TRUE))
            β := min(β, value)
            if α ≥ β then
                break (* α cut-off *)
        return value
*/
/* Precodition: Not expecting 0 depth, or game result over. */

int alphabeta(Board* pos, int depth, int alpha, int beta, Sequence* bestSequence)
{
    Sequence sequence;
    sequence.action1.cost = 0;
    sequence.action2.cost = 0;
    sequence.action3.cost = 0;

    bool abex = true;
    
    EngineEval eval;
    eval.testvalue = 0;

    Board working;

    copyBoard(pos, &working);

    eval.value = (pos->turn == White ? -2000 : 2000);

    abex = chooseAction(pos, &eval, depth, &alpha, &beta, &working, &sequence, 1, 3, -1, bestSequence);

    return eval.value;
}

bool chooseAction(Board* pos, EngineEval* eval, int depth, int* alpha, int* beta, Board* working, Sequence* sequence, int seqIndex, int remaining, int bMove, Sequence* bestSequence)
{
    bool abex;
    int result = getResult(pos);

    if (result == 1)
    {
        return tryMove(pos, eval, depth, alpha, beta, sequence, bestSequence, 1000);
    }
    else if (result == -1)
    {
        return tryMove(pos, eval, depth, alpha, beta, sequence, bestSequence, -1000);
    }

    u64 testVal;
    u64 sourceMask;
    u64 destMask;
    u64 nonoccmask;
    Action* currAction = nullptr;

    if (seqIndex == 1)
        currAction = &(sequence->action1);
    else if (seqIndex == 2)
        currAction = &(sequence->action2);
    else if (seqIndex == 3)
        currAction = &(sequence->action3);

    u64 friendlyMask = (working->turn == White ? working->white : working->black);
    u64 enemyMask = (working->turn == White ? working->black : working->white);
    u64 friendlyCannon = friendlyMask & working->rooks;

    for (int moveCost = remaining; moveCost >= 1; moveCost--)
    {
        sourceMask = friendlyMask;

        for (int moveIndex = 0; moveIndex < 64; moveIndex++)
        {
            if (sourceMask == 0ULL)
                break;
            else if (moveIndex == bMove)
                continue;

            testVal = sourceMask & 1ULL;

            if (testVal == 1ULL)
            {
                nonoccmask = ~(working->white | working->black) | (1ULL << moveIndex);
                destMask = calcExtendedMoveMask(moveIndex, moveCost, nonoccmask);

                for (int targetIndex = 0; targetIndex < 64; targetIndex++)
                {
                    if (destMask == 0ULL)
                        break;

                    testVal = destMask & 1ULL;

                    if (testVal == 1ULL)
                    {
                        setMoveAction(currAction, moveIndex, targetIndex, moveCost);

                        if (remaining == moveCost)
                        {
                            abex = tryMove(pos, eval, depth, alpha, beta, sequence, bestSequence);

                            if (!abex)
                                return false;
                        }
                        else
                        {
                            makeAction(working, currAction);
                            abex = chooseAction(pos, eval, depth, alpha, beta, working, sequence, seqIndex + 1, remaining - moveCost, targetIndex, bestSequence);       // ban this end point from being moves start point (use a 2 (3) cost action)
                            unmakeAction(working, currAction);

                            if (!abex)
                                return false;
                        }
                    }

                    destMask = destMask >> 1;
                }
            }

            sourceMask = sourceMask >> 1;
        }
    }

    sourceMask = friendlyCannon;

    for (int cannonIndex = 0; cannonIndex < 64; cannonIndex++)
    {
        if (sourceMask == 0ULL)
            break;

        testVal = sourceMask & 1ULL;

        if (testVal == 1ULL)
        {
            destMask = enemyMask;

            for (int targetIndex = 0; targetIndex < 64; targetIndex++)
            {
                if (destMask == 0ULL)
                    break;

                testVal = destMask & 1ULL;

                if (testVal == 1ULL)
                {
                    setCannonAction(currAction, cannonIndex, targetIndex);

                    if (isCannonActionLegal(working, currAction))
                    {
                        if (remaining == 1)
                        {
                            abex = tryMove(pos, eval, depth, alpha, beta, sequence, bestSequence);

                            if (!abex)
                                return false;
                        }
                        else
                        {
                            makeAction(working, currAction);
                            abex = chooseAction(pos, eval, depth, alpha, beta, working, sequence, seqIndex + 1, remaining - 1, -1, bestSequence);
                            unmakeAction(working, currAction);

                            if (!abex)
                                return false;
                        }
                    }
                }

                destMask = destMask >> 1;
            }
        }

        sourceMask = sourceMask >> 1;
    }

    destMask = enemyMask;

    for (int targetIndex = 0; targetIndex < 64; targetIndex++)
    {
        if (destMask == 0ULL)
            break;

        testVal = destMask & 1ULL;

        if (testVal == 1ULL)
        {
            setMeleeAction(currAction, targetIndex);

            if (isMeleeActionLegal(working, currAction))
            {
                if (remaining == 1)
                {
                    abex = tryMove(pos, eval, depth, alpha, beta, sequence, bestSequence);

                    if (!abex)
                        return false;
                }
                else
                {
                    makeAction(working, currAction);
                    abex = chooseAction(pos, eval, depth, alpha, beta, working, sequence, seqIndex + 1, remaining - 1, -1, bestSequence);
                    unmakeAction(working, currAction);

                    if (!abex)
                        return false;
                }
            }
        }

        destMask = destMask >> 1;
    }

    return true;
}

bool tryMove(Board* pos, EngineEval* eval, int depth, int* alpha, int* beta, Sequence* sequence, Sequence* bestSequence)
{
    bool retVal = true;

    make(pos, sequence);

    if(depth == 1)
        eval->testvalue = heuristic(pos);
    else 
        eval->testvalue = alphabeta(pos, depth - 1, *alpha, *beta, NULL);

    unmake(pos, sequence);

    if (pos->turn == White)
    {
        if (eval->testvalue > eval->value)
        {
            eval->value = eval->testvalue;
            
            if(bestSequence != NULL)
                copySequence(sequence, bestSequence);

            if (eval->value > *alpha)
            {
                *alpha = eval->value;
            }
        }
    }
    else
    {
        if (eval->testvalue < eval->value)
        {
            eval->value = eval->testvalue;

            if (bestSequence != NULL)
                copySequence(sequence, bestSequence);

            if (eval->value < *beta)
            {
                *beta = eval->value;
            }
        }
    }

    retVal = *alpha < *beta; // return true for normal situation, false for when the beta exclusion happens.
    return retVal;
}

bool tryMove(Board* pos, EngineEval* eval, int depth, int* alpha, int* beta, Sequence* sequence, Sequence* bestSequence, int ev)
{
    bool retVal = true;

    int testvalue = ev;

    if (pos->turn == White)
    {
        if (eval->testvalue > eval->value)
        {
            eval->value = eval->testvalue;

            if (bestSequence != NULL)
                copySequence(sequence, bestSequence);

            if (eval->value > *alpha)
            {
                *alpha = eval->value;
            }
        }
    }
    else
    {
        if (eval->testvalue < eval->value)
        {
            eval->value = eval->testvalue;

            if (bestSequence != NULL)
                copySequence(sequence, bestSequence);

            if (eval->value < *beta)
            {
                *beta = eval->value;
            }
        }
    }

    retVal = *alpha < *beta; // return true for normal situation, false for when the beta exclusion happens.
    return retVal;
}

int heuristic(struct Board* pos)
{
	u64 wPawn = pos->white & pos->pawns;
	u64 wKnight = pos->white & pos->knights;
	u64 wBishop = pos->white & pos->bishops;
    u64 wRook = pos->white & pos->rooks;
	u64 wQueen = pos->white & pos->queens;
    u64 wKing = pos->white & pos->kings;
    u64 bPawn = pos->black & pos->pawns;
    u64 bKnight = pos->black & pos->knights;
    u64 bBishop = pos->black & pos->bishops;
    u64 bRook = pos->black & pos->rooks;
    u64 bQueen = pos->black & pos->queens;
    u64 bKing = pos->black & pos->kings;

	int moveAdv = (pos->turn == White ? 1 : -1);
    int pawnAdv = pieceValue[Pawn] * (bitCount(wPawn) - bitCount(bPawn));
    int knightAdv = pieceValue[Knight] * (bitCount(wKnight) - bitCount(bKnight));
    int bishopAdv = pieceValue[Bishop] * (bitCount(wBishop) - bitCount(bBishop));
    int rookAdv = pieceValue[Rook] * (bitCount(wRook) - bitCount(bRook));
    int queenAdv = pieceValue[Queen] * (bitCount(wQueen) - bitCount(bQueen));
    int kingAdv = pieceValue[King] * (bitCount(wKing) - bitCount(bKing));

    int rcW = rowCount(pos->white, White);
    int rcB = rowCount(pos->black, Black);
    int forwardAdv = rcW - rcB;

    int result = getResult(pos) * 1000;

    if (result != 0)
        return result;
    else
        return moveAdv + pawnAdv + knightAdv + bishopAdv + rookAdv + queenAdv + kingAdv + forwardAdv;
}
#include "Bitboard.h"

int meleeAttack[] = { 2, 3, 4, 0, 4, 5 };

int meleeDefense[] = { 3, 5, 4, 2, 6, 4 };

bool cannonAbility[] = { false, false, false, true, false, false };

//				 0,01,10,11,100,101,110,111,1000,1001,1010, 1011,1100,1101,1110,1111
int bitHex[] = { 0, 1, 1, 2, 1, 2, 2,   3,  1,   2,   2,   3,   2,   3,   3,   4 };

u64 movePattern[] = { 0x0000000000000302ULL,
						0x0000000000000705ULL,
						0x0000000000000E0AULL,
						0x0000000000001C14ULL,
						0x0000000000003828ULL,
						0x0000000000007050ULL,
						0x000000000000E0A0ULL,
						0x000000000000C040ULL,
						0x0000000000030203ULL,
						0x0000000000070507ULL,
						0x00000000000E0A0EULL,
						0x00000000001C141CULL,
						0x0000000000382838ULL,
						0x0000000000705070ULL,
						0x0000000000E0A0E0ULL,
						0x0000000000C040C0ULL,
						0x0000000003020300ULL,
						0x0000000007050700ULL,
						0x000000000E0A0E00ULL,
						0x000000001C141C00ULL,
						0x0000000038283800ULL,
						0x0000000070507000ULL,
						0x00000000E0A0E000ULL,
						0x00000000C040C000ULL,
						0x0000000302030000ULL,
						0x0000000705070000ULL,
						0x0000000E0A0E0000ULL,
						0x0000001C141C0000ULL,
						0x0000003828380000ULL,
						0x0000007050700000ULL,
						0x000000E0A0E00000ULL,
						0x000000C040C00000ULL,
						0x0000030203000000ULL,
						0x0000070507000000ULL,
						0x00000E0A0E000000ULL,
						0x00001C141C000000ULL,
						0x0000382838000000ULL,
						0x0000705070000000ULL,
						0x0000E0A0E0000000ULL,
						0x0000C040C0000000ULL,
						0x0003020300000000ULL,
						0x0007050700000000ULL,
						0x000E0A0E00000000ULL,
						0x001C141C00000000ULL,
						0x0038283800000000ULL,
						0x0070507000000000ULL,
						0x00E0A0E000000000ULL,
						0x00C040C000000000ULL,
						0x0302030000000000ULL,
						0x0705070000000000ULL,
						0x0E0A0E0000000000ULL,
						0x1C141C0000000000ULL,
						0x3828380000000000ULL,
						0x7050700000000000ULL,
						0xE0A0E00000000000ULL,
						0xC040C00000000000ULL,
						0x0203000000000000ULL,
						0x0507000000000000ULL,
						0x0A0E000000000000ULL,
						0x141C000000000000ULL,
						0x2838000000000000ULL,
						0x5070000000000000ULL,
						0xA0E0000000000000ULL,
						0x40C0000000000000ULL };

void make(struct Board* pos, struct Sequence* orders)
{
	if (orders->action1.cost != 0)
	{
		makeAction(pos, &(orders->action1));
	}

	if (orders->action2.cost != 0)
	{
		makeAction(pos, &(orders->action2));
	}

	if (orders->action3.cost != 0)
	{
		makeAction(pos, &(orders->action3));
	}

	pos->turn = (pos->turn == White ? Black : White);
}

void unmake(struct Board* pos, struct Sequence* orders)
{
	if (orders->action1.cost != 0)
	{
		unmakeAction(pos, &(orders->action1));
	}

	if (orders->action2.cost != 0)
	{
		unmakeAction(pos, &(orders->action2));
	}

	if (orders->action3.cost != 0)
	{
		unmakeAction(pos, &(orders->action3));
	}

	pos->turn = (pos->turn == White ? Black : White);
}

void makeAction(struct Board* pos, struct Action* action)
{
	Piece p;
	int endCell = findCellFromRC(action->endr, action->endc);

	Turn enemy = (pos->turn == White ? Black : White);

	if (action->actionType == Cannon || action->actionType == Melee)
	{
		p = findPieceAtCell(pos, endCell);

		action->target = p;
		removePiece(pos, p, endCell, enemy);
	}
	else if (action->actionType == Move)
	{
		int startCell = findCellFromRC(action->startr, action->startc);

		p = findPieceAtCell(pos, startCell);
		action->target = p;

		// remove before add or else Start = End fails.
		removePiece(pos, p, startCell, pos->turn);
		addPiece(pos, p, endCell, pos->turn);
	}
}

void unmakeAction(struct Board* pos, struct Action* action)
{
	Piece p = action->target;
	int endCell = findCellFromRC(action->endr, action->endc);

	Turn enemy = (pos->turn == White ? Black : White);

	if (action->actionType == Cannon || action->actionType == Melee)
	{
		addPiece(pos, p, endCell, pos->turn);	// us now was enemy when the action happened
	}
	else if (action->actionType == Move)
	{
		int startCell = findCellFromRC(action->startr, action->startc);

		// remove before add or else Start = End fails.
		removePiece(pos, p, endCell, enemy);	// enemy now was Us when the action happened
		addPiece(pos, p, startCell, enemy);
	}
}

bool isLegal(struct Board *pos, struct Sequence *orders)
{
	if (orders->action1.cost + orders->action2.cost + orders->action3.cost != 3)
		return false;

	bool actionLegal;

	if (orders->action3.cost != 0)
	{
		actionLegal = isActionLegal(pos, &(orders->action3));

		if (!actionLegal)
			return false;
	}

	if (orders->action2.cost != 0)
	{
		actionLegal = isActionLegal(pos, &(orders->action2));

		if (!actionLegal)
		{
			return false;
		}
	}

	if (orders->action1.cost != 0)
	{
		actionLegal = isActionLegal(pos, &(orders->action1));

		if (!actionLegal)
		{
			return false;
		}
	}

	return true;
}

bool isActionLegal(struct Board* pos, struct Action* action)
{
	if (action->actionType == Move)
	{
		return isMoveActionLegal(pos, action);
	}
	else if (action->actionType == Melee)
	{
		return isMeleeActionLegal(pos, action);
	}
	else if (action->actionType == Cannon)
	{
		return isCannonActionLegal(pos, action);
	}
	else
	{
		return false;
	}
}

bool isCannonActionLegal(struct Board* pos, struct Action* action)
{
	if (pos->turn != White && pos->turn != Black)
		return false;

	if (!validRC(action->startr, action->startc) || !validRC(action->endr, action->endc))
		return false;

	if (action->cost != 1)
		return false;

	u64 startMask = (pos->turn == White ? pos->white : pos->black);
	u64 endMask = (pos->turn == White ? pos->black : pos->white);

	int startCell = findCellFromRC(action->startr, action->startc);
	int endCell = findCellFromRC(action->endr, action->endc);

	Piece p = findPieceAtCell(pos, startCell);

	if (!cannonAbility[p])
		return false;

	u64 start = getBitAtIndex(startCell, startMask);
	u64 end = getBitAtIndex(endCell, endMask);

	int deltaR = action->endr - action->startr;
	int deltaC = action->endc - action->startc;

	if (deltaR > 0)
		deltaR = 1;
	else if (deltaR < 0)
		deltaR = -1;
	else 
		deltaR = 0;

	if (deltaC > 0)
		deltaC = 1;
	else if (deltaC < 0)
		deltaC = -1;
	else
		deltaC = 0;

	int r = action->startr;
	int c = action->startc;
	r += deltaR;
	c += deltaC;

	int dcell = deltaR * 8 + deltaC;

	u64 nonoccmask = ~(pos->white | pos->black);
	int pathCell = findCellFromRC(r, c);
	u64 val = 0ULL;

	while (c >= 0 && c <= 7 && r >= 0 && r <= 7)
	{
		if (pathCell == endCell)
			break;

		val = getBitAtIndex(pathCell, nonoccmask);

		if (val != 1ULL)
		{
			return false;
		}

		pathCell += dcell;
		r += deltaR;
		c += deltaC;
	}

	return true;
}

bool isMeleeActionLegal(struct Board* pos, struct Action* action)
{
	if (pos->turn != White && pos->turn != Black)
		return false;

	if (!validRC(action->endr, action->endc))
		return false;

	if (action->cost != 1)
		return false;

	u64 allyMask = (pos->turn == White ? pos->white : pos->black);
	u64 endmask = (pos->turn == White ? pos->black : pos->white);	// white's turn, target must be black, vice versus
	int endCell = findCellFromRC(action->endr, action->endc);

	u64 end = getBitAtIndex(endCell, endmask);

	if (end == 0ULL)
		return false;

	Piece p = findPieceAtCell(pos, endCell);
	int defense = meleeDefense[p];

	u64 attackermask = movePattern[endCell] & allyMask;
	int attack = getTotalAttackerStrength(pos, attackermask);

	return (attack >= defense);
}

bool isMoveActionLegal(struct Board* pos, struct Action* action)
{
	if (pos->turn != White && pos->turn != Black)
		return false;

	if (!validRC(action->startr, action->startc) || !validRC(action->endr, action->endc))
		return false;
 
	u64 startmask = (pos->turn == White ? pos->white : pos->black); // occupied by color whose turn it is
	u64 nonoccmask = ~(pos->white | pos->black);						// not occupied by white or black

	int startCell = findCellFromRC(action->startr, action->startc);
	int endCell = findCellFromRC(action->endr, action->endc);

	u64 movemask = calcExtendedMoveMask(startCell, action->cost, nonoccmask);

	u64 start = getBitAtIndex(startCell, startmask);				// is there a piece of the right color at index
	u64 end = getBitAtIndex(endCell, movemask);						// is the end square a proper king move (or extended king move for cost=2,3)

	return (start & end) != 0ULL;
}

u64 getBitAtIndex(int index, u64 mask)
{
	u64 rval = (mask >> index) & 0x1ULL;

	return rval;
}

u64 getBitAtIndexRC(int row, int column, u64 mask)
{
	return getBitAtIndex(findCellFromRC(row, column), mask);
}

u64 getBitRep(int index)
{
	return (0x1ULL << index);
}

int findCellFromRC(int row, int column)
{
	return 8 * row + column;
}

bool validRC(int row, int column)
{
	return row >= 0 && row <= 7 && column >= 0 && column <= 7;
}

u64 calcExtendedMoveMask(int index, int cost, u64 nonoccmask)
{
	if (cost == 1)
	{
		return movePattern[index] & nonoccmask;
	}

	nonoccmask = nonoccmask | getBitRep(index);			// allow moving back to start position in a 2 or 3 cost move.

	u64 moveMask = movePattern[index] & nonoccmask;
	cost--;

	while (cost > 0)
	{
		moveMask = extendMovePattern(moveMask, nonoccmask);
		cost--;
	}

	return moveMask;
}

u64 extendMovePattern(u64 poss, u64 nonoccmask)
{
	int offset = 0;
	u64 val;
	u64 accum = 0ULL;

	while (poss != 0ULL && offset < 64)
	{
		val = poss & 1ULL;

		if (val == 1ULL)
		{
			val = movePattern[offset] & nonoccmask;		// pattern of movement from this square, modified by what squares are not occupied
			accum = accum | val;						// accumulate possibilities by or.
		}

		poss = poss >> 1;
		offset++;
	}

	return accum;
}

int getTotalAttackerStrength(struct Board* pos, u64 attackerMask)
{
	int offset = 0;
	u64 val;
	int strength = 0;
	Piece p;

	while (attackerMask != 0ULL && offset < 64)
	{
		val = attackerMask & 1ULL;

		if (val == 1ULL)
		{
			p = findPieceAtCell(pos, offset);
			strength += meleeAttack[p];
		}

		attackerMask = attackerMask >> 1;
		offset++;
	}

	return strength;
}

Piece findPieceAtCell(struct Board* pos, int index)
{
	u64 checker = getBitAtIndex(index, pos->pawns);

	if (checker != 0ULL)
		return Pawn;

	checker = getBitAtIndex(index, pos->knights);

	if (checker != 0ULL)
		return Knight;
	
	checker = getBitAtIndex(index, pos->bishops);

	if (checker != 0ULL)
		return Bishop;

	checker = getBitAtIndex(index, pos->rooks);

	if (checker != 0ULL)
		return Rook;

	checker = getBitAtIndex(index, pos->queens);

	if (checker != 0ULL)
		return Queen;

	checker = getBitAtIndex(index, pos->kings);

	if (checker != 0ULL)
		return King;

	return (Piece)-1;
}

void removePiece(struct Board* pos, Piece piece, int index, Turn t)
{
	u64 rmask = ~getBitRep(index);	// mask to remove at index bit

	if (piece == Pawn)
		pos->pawns = pos->pawns & rmask;
	else if (piece == Knight)
		pos->knights = pos->knights & rmask;
	else if (piece == Bishop)
		pos->bishops = pos->bishops & rmask;
	else if (piece == Rook)
		pos->rooks = pos->rooks & rmask;
	else if (piece == Queen)
		pos->queens = pos->queens & rmask;
	else if (piece == King)
		pos->kings = pos->kings & rmask;

	if (t == White)
		pos->white = pos->white & rmask;
	else if (t == Black)
		pos->black = pos->black & rmask;
}

void addPiece(struct Board* pos, Piece piece, int index, Turn t)
{
	u64 amask = getBitRep(index);	// mask to add at index bit

	if (piece == Pawn)
		pos->pawns = pos->pawns | amask;
	else if (piece == Knight)
		pos->knights = pos->knights | amask;
	else if (piece == Bishop)
		pos->bishops = pos->bishops | amask;
	else if (piece == Rook)
		pos->rooks = pos->rooks | amask;
	else if (piece == Queen)
		pos->queens = pos->queens | amask;
	else if (piece == King)
		pos->kings = pos->kings | amask;

	if (t == White)
		pos->white = pos->white | amask;
	else if (t == Black)
		pos->black = pos->black | amask;
}

int bitCount(u64 val)
{
	int sval;
	int accum = 0;

	while (val != 0)
	{
		sval = (int)(val & 0xFULL);
		accum += bitHex[sval];

		val = val >> 4;
	}

	return accum;
}

int rowCount(u64 val, Turn t)
{
	int sval;
	int accum = 0;
	int row = 0;
	int part = 0;
	
	while (val != 0)
	{
		sval = (int)(val & 0xFULL);
		
		if (t == White)
		{
			accum += bitHex[sval] * row;
		}
		else if(t == Black)
		{
			accum += bitHex[sval] * (7 - row);
		}

		val = val >> 4;

		if (part == 0)
			part = 1;
		else if (part == 1)
		{
			part = 0;
			row++;
		}
	}

	return accum;
}

void setMoveAction(struct Action* action, int fromIndex, int toIndex, int cost)
{
	action->actionType = Move;
	action->cost = cost;

	action->startr = fromIndex >> 3;
	action->startc = fromIndex & 7;

	action->endr = toIndex >> 3;
	action->endc = toIndex & 7;
}

void setCannonAction(struct Action* action, int cannonIndex, int targetIndex)
{
	action->actionType = Cannon;
	action->cost = 1;

	action->startr = cannonIndex >> 3;
	action->startc = cannonIndex & 7;

	action->endr = targetIndex >> 3;
	action->endc = targetIndex & 7;
}

void setMeleeAction(struct Action* action, int targetIndex)
{
	action->actionType = Melee;
	action->cost = 1;

	action->endr = targetIndex >> 3;
	action->endc = targetIndex & 7;
}

void copyBoard(struct Board* baseBoard, struct Board* targetBoard)
{
	targetBoard->white = baseBoard->white;
	targetBoard->black = baseBoard->black;
	targetBoard->pawns = baseBoard->pawns;
	targetBoard->knights = baseBoard->knights;
	targetBoard->bishops = baseBoard->bishops;
	targetBoard->rooks = baseBoard->rooks;
	targetBoard->queens = baseBoard->queens;
	targetBoard->kings = baseBoard->kings;
	targetBoard->turn = baseBoard->turn;
}

void copyAction(struct Action* baseAction, struct Action* targetAction)
{
	targetAction->actionType = baseAction->actionType;
	targetAction->cost = baseAction->cost;
	targetAction->startr = baseAction->startr;
	targetAction->startc = baseAction->startc;
	targetAction->endr = baseAction->endr;
	targetAction->endc = baseAction->endc;
	targetAction->target = baseAction->target;
}

void copySequence(struct Sequence* baseSequence, struct Sequence* targetSequence)
{
	copyAction(&(baseSequence->action1), &(targetSequence->action1));
	copyAction(&(baseSequence->action2), &(targetSequence->action2));
	copyAction(&(baseSequence->action3), &(targetSequence->action3));
}

int getResult(struct Board* baseBoard)
{
	int wEx = (baseBoard->white == 0ULL ? -1 : 0);
	int bEx = (baseBoard->black == 0ULL ? 1 : 0);

	return wEx + bEx;
}
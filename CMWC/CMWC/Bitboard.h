#pragma once

using u64 = unsigned long long;

enum Turn : short
{
	White,
	Black
};

enum Piece : short
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

struct Board
{
	u64 white;
	u64 black;
	u64 pawns;
	u64 knights;
	u64 bishops;
	u64 rooks;
	u64 queens;
	u64 kings;
	enum Turn turn;
};

enum ActionType : short 
{
	Move,
	Melee,
	Cannon
};

struct Action 
{
	int startc;
	int startr;
	int endc;
	int endr;
	int cost;
	Piece target;
	enum ActionType actionType;
};

struct Sequence
{
	struct Action action1;
	struct Action action2;
	struct Action action3;
};

void make(struct Board* pos, struct Sequence* orders);
void unmake(struct Board* pos, struct Sequence* orders);
bool isLegal(struct Board *pos, struct Sequence *orders);
bool isActionLegal(struct Board* pos, struct Action* action);
bool isCannonActionLegal(struct Board *pos, struct Action *action);
bool isMeleeActionLegal(struct Board* pos, struct Action* action);
bool isMoveActionLegal(struct Board* pos, struct Action* action);
u64 getBitAtIndex(int index, u64 mask);
u64 getBitAtIndexRC(int row, int column, u64 mask);
int findCellFromRC(int row, int column);
u64 getBitRep(int index);
bool validRC(int row, int column);
u64 calcExtendedMoveMask(int index, int cost, u64 nonoccmask);
u64 extendMovePattern(u64 poss, u64 nonoccmask);
Piece findPieceAtCell(struct Board* pos, int index);
int getTotalAttackerStrength(struct Board* pos, u64 attackerMask);
void makeAction(struct Board* pos, struct Action* action);
void removePiece(struct Board* pos, Piece piece, int index);
void addPiece(struct Board* pos, Piece piece, int index);
void unmakeAction(struct Board* pos, struct Action* action);
int bitCount(u64 val);
int rowCount(u64 val, Turn t);
void setMoveAction(struct Action* action, int fromIndex, int toIndex, int cost);
void setCannonAction(struct Action* action, int cannonIndex, int targetIndex);
void setMeleeAction(struct Action* action, int targetIndex);
void copyBoard(struct Board* baseBoard, struct Board* targetBoard);
void copyAction(struct Action* baseAction, struct Action* targetAction);
void copySequence(struct Sequence* baseSequence, struct Sequence* targetSequence);
int getResult(struct Board* baseBoard);
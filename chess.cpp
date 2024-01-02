#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <time.h>

#define  BlankCate 0
#define  BlackCate 1
#define  WhiteCate 2
#define  maxValue 10000
using namespace std;
struct ChessGrid;
struct ChessBoard;
struct Point;
struct Move;

long long startTime;
long long endTime;
long long timeLimit = 1800;
bool timeOut = false;

bool debug = true;
int turn = 0;
int myCate;
int otherCate;
int verticalMove;
int kingLine;
int startLine;

int SearchChessBoard(ChessBoard chess, vector<Move>& normalMove, vector<Move>& mustMove, bool myTime);
void Respond(Move it);
void RefreshChessBoard(ChessBoard& chess, vector<Move>& otherMove);
void Refresh(ChessBoard& chess, Move itMove, bool myside);
Point AddPoint(int x, int y);
void Expand(ChessBoard chess, vector<Move>& normalMove, vector<Move>& mustMove, int x, int y,
	bool first, int pos, Move lastMove, int thisCate, int thatCate, int moveDir, int becomeKing, bool isKing);

bool TimeControl()
{
	/*endTime = clock();
	if ((long long)endTime - (long long)startTime >= (long long)timeLimit)
	{
		timeOut = true;
		return true;
	}*/
	return false;
}

struct ChessGrid
{
	//int ID;
	bool king;
	int cate;
	bool bejudge;
	ChessGrid() :king(false), cate(BlankCate), bejudge(false) {}
}blankChess;

struct ChessBoard
{
	ChessGrid chessBoard[8][8];
	int myCount, otherCount;
	void Display()
	{
		cout << endl << "   0 1 2 3 4 5 6 7" << endl;
		for (int i = 0; i < 8; i++)
		{
			cout << " " << i << " ";
			for (int j = 0; j < 8; j++)
			{
				if (chessBoard[i][j].cate == WhiteCate)
					cout << "O ";
				else if (chessBoard[i][j].cate == BlackCate)
					cout << "X ";
				else
					cout << "  ";
			}
			cout << endl;
		}
	}
	ChessBoard() :myCount(12), otherCount(12) {}
};

struct Point
{
	int x;
	int y;
	char dot;
	Point() :x(0), y(0), dot(',') {}
	Point(int x, int y, char dot) :x(x), y(y), dot(dot) {}
	friend ostream& operator<<(ostream& os, const Point& T);
	friend istream& operator>>(istream& is, Point& T);
};

ostream& operator<<(ostream& os, const Point& T)
{
	os << T.x << ',';
	os << T.y;
	return os;
}

istream& operator>>(istream& is, Point& T)
{
	is >> T.x >> T.dot >> T.y;
	return is;
}

struct Move
{
	int count;
	int advance;
	bool myside;
	bool onlyMove;
	vector<Point> movePoint;
	Move() :count(0), advance(0), myside(false), onlyMove(false) {}
}myChoice;

bool InBorder(int x, int y)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8)
		return true;
	return false;
}

Point AddPoint(int x, int y)
{
	Point start;
	start.x = x;
	start.y = y;
	return start;
}

void Initial(ChessBoard& chess)
{
	int j;
	for (int i = 0; i < 8; i++)
	{
		if (i % 2 == 0)
			j = 1;
		else
			j = 0;
		for (; j < 8; j += 2)
		{
			if (i == 0)
			{
				if (j == 5)
				{
					chess.chessBoard[i][j].cate = BlackCate;
					chess.chessBoard[i][j].king = true;
				}
			}
			if (i == 1)
			{
				if (j == 0 )
					chess.chessBoard[i][j].cate = BlackCate;
			}
			
			if (i == 4)
			{
				if (j == 3)
					chess.chessBoard[i][j].cate = BlackCate;
				if (j == 5)
					chess.chessBoard[i][j].cate = WhiteCate;
			}
			if (i == 6)
			{
				if (j == 1)
					chess.chessBoard[i][j].cate = WhiteCate;
			}
			if (i ==7)
			{
				if (j == 2)
					chess.chessBoard[i][j].cate = WhiteCate;
				chess.chessBoard[i][j].king = true;
			}
		}
	}
	if (debug)
		chess.Display();
}

void RefreshChessBoard(ChessBoard& chess, vector<Move>& otherMove)
{
	Move otherSide;
	cin >> otherSide.count;
	for (int i = 0; i < otherSide.count; i++)
	{
		Point temp;
		cin >> temp;
		otherSide.movePoint.push_back(temp);
	}
	Refresh(chess, otherSide, false);
}

void Refresh(ChessBoard& chess, Move itMove, bool myside)
{
	vector<Point>::iterator it;
	int tempCount = 0;
	bool king = false;
	for (it = itMove.movePoint.begin(); it != itMove.movePoint.end(); it++, tempCount++)
	{
		if (myside && it->x == kingLine)
			king = true;
		if (!myside && it->x == startLine)
			king = true;
		if (tempCount < itMove.count - 1)
		{
			if (abs(it->x - (it + 1)->x) != 1 && abs(it->y - (it + 1)->y) != 1)
			{
				chess.chessBoard[(it->x + (it + 1)->x) / 2][(it->y + (it + 1)->y) / 2] = blankChess;
				if (myside)
					--chess.otherCount;
				else
					--chess.myCount;
			}
		}
		if (tempCount == itMove.count - 1)
		{
			chess.chessBoard[it->x][it->y] = chess.chessBoard[itMove.movePoint.begin()->x][itMove.movePoint.begin()->y];
			if (king)
				chess.chessBoard[it->x][it->y].king = king;
			chess.chessBoard[itMove.movePoint.begin()->x][itMove.movePoint.begin()->y] = blankChess;
			break;
		}
	}
	if (debug)
		chess.Display();

}

void Respond(Move it)
{
	myChoice = it;
	cout << it.count << " ";
	for (int i = 0; i < it.count; i++)
	{
		cout << it.movePoint[i];
		if (i < it.count - 1)
			cout << " ";
	}
	cout << endl;
}

int AlphaBeta(int depth, bool myside, int alpha, int beta, ChessBoard& chess, vector<vector<Move> >& Normtree,
	vector<vector<Move> >& Musttree, int coin, int addcoin, bool start)
{
	if (turn > 60)
		return coin;
	if (depth == 0)
		return  coin;

	if (myside)
		addcoin = 1;
	else
		addcoin = -1;
	if (SearchChessBoard(chess, Normtree[depth], Musttree[depth], myside))
	{
		for (int i = 0; i < Musttree[depth].size(); i++)
		{

			ChessBoard tempChess = chess;
			Refresh(tempChess, Musttree[depth][i], myside);

			int score = AlphaBeta(depth - 1, !(myside), alpha, beta, tempChess,
				Normtree, Musttree, coin + Musttree[depth][i].advance * addcoin, addcoin, false);
			Normtree[depth - 1].clear();
			Musttree[depth - 1].clear();
			if (!myside)
			{
				if (score < beta)
				{
					beta = score;
					if (alpha >= beta)
						return alpha;
				}
			}
			else
			{
				if (score > alpha)
				{
					alpha = score;
					if (start)
						myChoice = Musttree[depth][i];
					if (alpha >= beta)
						return beta;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < Normtree[depth].size(); i++)
		{
			ChessBoard tempChess = chess;
			Refresh(tempChess, Normtree[depth][i], myside);
			int score = AlphaBeta(depth - 1, !(myside), alpha, beta, tempChess,
				Normtree, Musttree, coin + Normtree[depth][i].advance * addcoin, addcoin, false);
			Normtree[depth - 1].clear();
			Musttree[depth - 1].clear();
			if (!myside)
			{
				if (score < beta)
				{
					beta = score;
					if (alpha >= beta)
						return alpha;
				}
			}
			else
			{
				if (score > alpha)
				{
					alpha = score;
					if (start)
						myChoice = Normtree[depth][i];
					if (alpha >= beta)
						return beta;
				}
			}
		}
	}
	if (!myside)
		return beta;
	else
		return alpha;
	return alpha;
}

int SearchChessBoard(ChessBoard chess, vector<Move>& normalMove, vector<Move>& mustMove, bool myTime)
{
	int moveDir = 0;
	int thisCate;
	int thatCate;
	int becomeKing;
	int start;
	if (myTime)
	{
		moveDir = verticalMove;
		thisCate = myCate;
		thatCate = otherCate;
		becomeKing = kingLine;
		start = startLine;
	}
	else
	{
		moveDir = verticalMove * (-1);
		thisCate = otherCate;
		thatCate = myCate;
		becomeKing = startLine;
		start = kingLine;
	}
	int j;
	for (int i = start; i != becomeKing + moveDir; i += moveDir)
	{
		if (i % 2 == 0)
			j = 1;
		else
			j = 0;
		for (; j < 8; j += 2)
		{
			if (chess.chessBoard[i][j].cate == thisCate)
			{
				Move start;
				start.myside = myTime;
				Expand(chess, normalMove, mustMove, i, j, true, 0, start, thisCate, thatCate, moveDir, becomeKing, chess.chessBoard[i][j].king);
			}
		}
	}
	int maxMove = 0;
	vector<Move>::iterator it;
	if (!mustMove.size())
		;
	else
	{
		if (mustMove.size() == 1)
		{
			maxMove = 2;
		}
		else if (mustMove.size() > 1)
		{
			for (int i = 0; i < mustMove.size(); i++)
			{
				if (mustMove[i].count > maxMove)
					maxMove = mustMove[i].count;
			}
			it = mustMove.begin();
			vector<Move> temp;
			for (int i = 0; i < mustMove.size(); ++i)
			{
				if (mustMove[i].count == maxMove)
					temp.push_back(mustMove[i]);
			}
			mustMove = temp;
		}
		
		for (it=mustMove.begin();it!=mustMove.end(); ++it)
		{
			if (!chess.chessBoard[it->movePoint.begin()->x][it->movePoint.begin()->y].king)
			{
				int temp = (it->movePoint[it->movePoint.size() - 1].x - it->movePoint.begin()->x) / moveDir;
				if (temp > 0)
					it->advance += 10 * temp;
			}
		}
	}
	return maxMove;
}

void AddNormalMove(ChessBoard chess, vector<Move>& normalMove, int x, int y, int i, int j, int moveDir, int becomeKing, bool isKing, Move startMove)
{
	if (InBorder(x + i, y + j))
	{
		if (chess.chessBoard[x + i][y + j].cate == BlankCate)
		{
			Point start = AddPoint(x, y);
			Point tempP = AddPoint(x + i, y + j);
			Move temp = startMove;
			temp.count = 2;
			temp.movePoint.push_back(start);
			temp.movePoint.push_back(tempP);
			if ((!isKing) && (x + i) == becomeKing)
				temp.advance += 200;
			if ((!isKing) && i == moveDir)
				temp.advance += 10;
			normalMove.push_back(temp);
		}
	}
}

void Expand(ChessBoard chess, vector<Move>& normalMove, vector<Move>& mustMove, int x, int y,
	bool first, int pos, Move lastMove, int thisCate, int thatCate, int moveDir, int becomeKing, bool isKing)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8)
	{
		if (chess.chessBoard[x][y].bejudge)
		{
			return;
		}
		chess.chessBoard[x][y].bejudge = !chess.chessBoard[x][y].bejudge;
		int eatCount = 0;
		int tempSizeOne = 0;
		vector<Move>::iterator it;
		for (int i = -1; i <= 1; i += 2)
		{
			for (int j = -1; j <= 1; j += 2)
			{
				if (first)
				{
					if (chess.chessBoard[x][y].king)
						AddNormalMove(chess, normalMove, x, y, i, j, moveDir, becomeKing, isKing, lastMove);
					else
					{
						if (i == moveDir)
							AddNormalMove(chess, normalMove, x, y, i, j, moveDir, becomeKing, isKing, lastMove);
					}
				}
				if (InBorder(x + 2 * i, y + 2 * j))
				{
					if (chess.chessBoard[x + i][y + j].cate == thatCate && chess.chessBoard[x + 2 * i][y + 2 * j].cate == BlankCate
						&& (!chess.chessBoard[x + 2 * i][y + 2 * j].bejudge))
					{
						bool flag = false;
						if (!isKing)
						{
							if (x + 2 * i == becomeKing)
							{
								flag = true;
								isKing = !isKing;
							}
						}
						++eatCount;

						Point tempP = AddPoint(x + 2 * i, y + 2 * j);
						Move tempM = lastMove;
						if (flag)
							tempM.advance += 200;
						if (chess.chessBoard[x + i][y + j].king)
							tempM.advance += 300;
						else
							tempM.advance += 100;

						if (first)
						{
							Point tempP0 = AddPoint(x, y);
							tempM.count = 2;
							tempM.movePoint.push_back(tempP0);
							tempM.movePoint.push_back(tempP);
							mustMove.push_back(tempM);
							pos = mustMove.size() - 1;
						}
						else
						{
							if (eatCount <= 1)
							{
								if (flag)
									mustMove[pos].advance += 200;

								mustMove[pos].count += 1;
								mustMove[pos].movePoint.push_back(tempP);
								if (chess.chessBoard[x + i][y + j].king)
									mustMove[pos].advance += 300;
								else
									mustMove[pos].advance += 100;
							}
							else
							{
								tempM.count += 1;
								tempM.movePoint.push_back(tempP);
								mustMove.push_back(tempM);
								pos = mustMove.size() - 1;
							}
						}

						Expand(chess, normalMove, mustMove, x + 2 * i, y + 2 * j, false, pos, mustMove[pos], thisCate, thatCate, moveDir, becomeKing, isKing);
					}
				}
			}
		}
	}
}

int main()
{
	srand(time(NULL));
	ChessBoard chess;
	Initial(chess);

	string start;
	string order;

	blankChess.cate = BlankCate;
	blankChess.king = false;
	blankChess.bejudge = false;

	vector<vector< Move > > normMoveTree;
	vector<vector< Move > > mustMoveTree;

	vector<Move> otherMove;

	vector<Move> normal;
	vector<Move> must;

	for (int i = 0; i < 10; i++)
	{
		normMoveTree.push_back(normal);
		mustMoveTree.push_back(must);
	}

	cin >> start >> myCate;

	if (myCate == BlackCate)
	{
		otherCate = WhiteCate;
		verticalMove = -1;
		kingLine = 0;
		startLine = 7;
	}
	else
	{
		otherCate = BlackCate;
		verticalMove = 1;
		kingLine = 7;
		startLine = 0;
	}
	cout << "OK" << endl;

	while (true)
	{
		cin >> order;
		if (order[0] == 'P')
		{
			RefreshChessBoard(chess, otherMove);
			otherMove.clear();
			if (debug)
			{
				chess.Display();
			}
		}
		else if (order[0] == 'T')
		{
			timeOut = false;
			startTime = clock();
			for (int depth = 1; (turn + depth <= 60) && !timeOut && depth < 9; depth++)
			{
				AlphaBeta(depth, true, -1 * maxValue, maxValue, chess, normMoveTree,
					mustMoveTree, 0, 0, true);
				
				normMoveTree[depth].clear();
				mustMoveTree[depth].clear();
				if (TimeControl())
					break;
			}

			Respond(myChoice);
			Refresh(chess, myChoice, true);
			turn += 1;
			if (debug)
			{
				chess.Display();
			}
		}
		else if (order[0] == 'E')
			break;
	}
	return 0;
}
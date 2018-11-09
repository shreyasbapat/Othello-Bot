/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <ctime>
#include <list>
using namespace std;
using namespace Desdemona;

/*
*
*Values taken from the Research paper of University of Washington.
*
*/

#define INF 1e18

Turn my;

clock_t start,finish;
OthelloBoard globalBoard;


bool canMove(char self, char opp, char *str)  {
	if (str[0] != opp) return false;
	for (int ctr = 1; ctr < 8; ctr++) {
		if (str[ctr] == 'e') return false;
		if (str[ctr] == self) return true;
	}
	return false;
}

bool isLegalMove(char self, char opp, char grid[8][8], int startx, int starty)   {
	if (grid[startx][starty] != 'e') return false;
	char str[10];
	int x, y, dx, dy, ctr;
	for (dy = -1; dy <= 1; dy++)
		for (dx = -1; dx <= 1; dx++)    {
	        // keep going if both velocities are zero
			if (!dy && !dx) continue;
			str[0] = '\0';
			for (ctr = 1; ctr < 8; ctr++)   {
				x = startx + ctr*dx;
				y = starty + ctr*dy;
				if (x >= 0 && y >= 0 && x<8 && y<8) str[ctr-1] = grid[x][y];
				else str[ctr-1] = 0;
			}
			if (canMove(self, opp, str)) return true;
		}
	return false;
}

int numValidMoves(char self, char opp, char grid[8][8])   {
	int count = 0, i, j;
	for(i = 0; i < 8; i++) for(j = 0; j < 8; j++) if(isLegalMove(self, opp, grid, i, j)) count++;
	return count;
}

double othelloBoardEvaluator(char grid[8][8])  {
	char my_color = 'm',opp_color = 'y';
    int myTiles = 0, oppTiles = 0, i, j, k, myFrontTiles = 0, oppFrontTiles = 0, x, y;
    double p = 0.0, c = 0.0, l = 0.0, m = 0.0, f = 0.0, d = 0.0;

    int X1[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int Y1[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int V[8][8] = 	{ { 20, -3, 11, 8, 8, 11, -3, 20 },
    				{ -3, -7, -4, 1, 1, -4, -7, -3 },
    				{ 11, -4, 2, 2, 2, 2, -4, 11 },
    				{ 8, 1, 2, -3, -3, 2, 1, 8 },
    				{ 8, 1, 2, -3, -3, 2, 1, 8 },
    				{ 11, -4, 2, 2, 2, 2, -4, 11 },
    				{ -3, -7, -4, 1, 1, -4, -7, -3 },
    				{ 20, -3, 11, 8, 8, 11, -3, 20 } };
    // int V[8][8]={ 10,-3,2,2,2,2,-3,10
    //             ,-3,-4,-1,-1,-1,-1,-4,-3
    //             ,2,-1,1,0,0,1,-1,-2
    //             ,2,-1,0,1,1,0,-1,2
    //             ,2,-1,0,1,1,0,-1,2
    //             ,2,-1,1,0,0,1,1,-2
    //             ,-3,-4,-1,-1,-1,1,-4,-3
    //             ,10,-3,2,2,2,2,-3,10 };

	// Piece difference, frontier disks and disk squares
    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)  {
            if(grid[i][j] == my_color)  {
                d += V[i][j];
                myTiles++;
            } 
            else if(grid[i][j] == opp_color)  {
                d -= V[i][j];
                oppTiles++;
            }
            if(grid[i][j] != 'e')   {
                for(k = 0; k < 8; k++)  {
                    x = i + X1[k]; y = j + Y1[k];
                    if(x >= 0 && x < 8 && y >= 0 && y < 8 && grid[x][y] == 'e') {
                        if(grid[i][j] == my_color)  myFrontTiles++;
                        else oppFrontTiles++;
                        break;
                    }
                }
            }
        }
        // int V[8][8]={ 10,-3,2,2,2,2,-3,10
    //             ,-3,-4,-1,-1,-1,-1,-4,-3
    //             ,2,-1,1,0,0,1,-1,-2
    //             ,2,-1,0,1,1,0,-1,2
    //             ,2,-1,0,1,1,0,-1,2
    //             ,2,-1,1,0,0,1,1,-2
    //             ,-3,-4,-1,-1,-1,1,-4,-3
    //             ,10,-3,2,2,2,2,-3,10 };

    if(myTiles > oppTiles) p = (100.0 * myTiles)/(myTiles + oppTiles);
    else if(myTiles < oppTiles) p = -(100.0 * oppTiles)/(myTiles + oppTiles);

    if(myFrontTiles > oppFrontTiles) f = -(100.0 * myFrontTiles)/(myFrontTiles + oppFrontTiles);
    else if(myFrontTiles < oppFrontTiles) f = (100.0 * oppFrontTiles)/(myFrontTiles + oppFrontTiles);

    // Corner occupancy
    myTiles = oppTiles = 0;
    if(grid[0][0] == my_color) myTiles++;
    else if(grid[0][0] == opp_color) oppTiles++;
    if(grid[0][7] == my_color) myTiles++;
    else if(grid[0][7] == opp_color) oppTiles++;
    if(grid[7][0] == my_color) myTiles++;
    else if(grid[7][0] == opp_color) oppTiles++;
    if(grid[7][7] == my_color) myTiles++;
    else if(grid[7][7] == opp_color) oppTiles++;
    c = 25 * (myTiles - oppTiles);

    // Corner closeness
    myTiles = oppTiles = 0;
    if(grid[0][0] == 'e')   {
        if(grid[0][1] == my_color) myTiles++;
        else if(grid[0][1] == opp_color) oppTiles++;
        if(grid[1][1] == my_color) myTiles++;
        else if(grid[1][1] == opp_color) oppTiles++;
        if(grid[1][0] == my_color) myTiles++;
        else if(grid[1][0] == opp_color) oppTiles++;
    }
    if(grid[0][7] == 'e')   {
        if(grid[0][6] == my_color) myTiles++;
        else if(grid[0][6] == opp_color) oppTiles++;
        if(grid[1][6] == my_color) myTiles++;
        else if(grid[1][6] == opp_color) oppTiles++;
        if(grid[1][7] == my_color) myTiles++;
        else if(grid[1][7] == opp_color) oppTiles++;
    }
    if(grid[7][0] == 'e')   {
        if(grid[7][1] == my_color) myTiles++;
        else if(grid[7][1] == opp_color) oppTiles++;
        if(grid[6][1] == my_color) myTiles++;
        else if(grid[6][1] == opp_color) oppTiles++;
        if(grid[6][0] == my_color) myTiles++;
        else if(grid[6][0] == opp_color) oppTiles++;
    }
    if(grid[7][7] == 'e')   {
        if(grid[6][7] == my_color) myTiles++;
        else if(grid[6][7] == opp_color) oppTiles++;
        if(grid[6][6] == my_color) myTiles++;
        else if(grid[6][6] == opp_color) oppTiles++;
        if(grid[7][6] == my_color) myTiles++;
        else if(grid[7][6] == opp_color) oppTiles++;
    }
    l = -10 * (myTiles - oppTiles);

    // Mobility
    myTiles = numValidMoves(my_color, opp_color, grid);
    oppTiles = numValidMoves(opp_color, my_color, grid);
    if(myTiles > oppTiles) m = (100.0 * myTiles)/(myTiles + oppTiles);
    else if(myTiles < oppTiles) m = -(100.0 * oppTiles)/(myTiles + oppTiles);

    // final weighted score
    double score = (11 * p) + (850.724 * c) + (382.026 * l) + (86.922 * m) + (78.396 * f) + (10 * d);
    return score;
}

double testMyMove(OthelloBoard board, Move move, Turn turn, short level, double alpha, double beta) {
    finish = clock();
    if(((double)(finish-start)/CLOCKS_PER_SEC)>1.95) {
        if(level&1) return -INF;
        return INF;
    }
	if(level == 6) {
		char grid[8][8];
		for(int i=0;i<8;i++) {
			for(int j=0;j<8;j++) {
				Coin findTurn = board.get(i,j);
				if(findTurn == turn) grid[i][j] = 'y';
				else if(findTurn == other(turn)) grid[i][j] = 'm';
				else grid[i][j] = 'e';
			}
		}
		return othelloBoardEvaluator(grid);
	}
	board.makeMove(turn,move);
	turn = other(turn);
	list<Move> newMoves = board.getValidMoves(turn);
	list<Move>::iterator iter = newMoves.begin();
	double ret = -INF;
	if(level&1) ret *= -1;
	if(!(newMoves.size())) return ret;
	for(;iter!=newMoves.end();iter++) {
		double curr = testMyMove(board,*iter,turn,level+1,alpha,beta);
		if(level&1) {
			ret = min(ret,curr);
			beta = min(beta,ret);
		}
		else {
			ret = max(ret,curr);
			alpha = max(alpha,ret);		
		}
		if(beta<=alpha) break;
	}
	return ret; 
}

double tester(OthelloBoard board,Turn turn) {
    char grid[8][8];
    for(int i=0;i<8;i++) {
        for(int j=0;j<8;j++) {
        Coin findTurn = board.get(i,j);
        if(findTurn == turn) grid[i][j] = 'm';
        else if(findTurn == other(turn)) grid[i][j] = 'y';
        else grid[i][j] = 'e';
        }
    }
    return othelloBoardEvaluator(grid);
}

bool compare(Move a, Move b) {
    OthelloBoard One = globalBoard,Two = globalBoard;
    One.makeMove(my,a);
    Two.makeMove(my,b);
    return tester(One,my)>tester(Two,my);
}

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
}

Move MyBot::play( const OthelloBoard& board )
{
    start = clock();
    list<Move> moves = board.getValidMoves( turn );
    my = turn;
    globalBoard = board;
    moves.sort(compare);
    list<Move>::iterator it = moves.begin();
    Move bestMove((*it).x,(*it).y);
    double retVal = -INF;
    double MAX = INF, MIN = -INF;
    OthelloBoard copyBoard = board;
    short level = 1;
    for(;it!=moves.end();it++) {
    	double currValue = testMyMove(copyBoard,*it,turn,level,MIN,MAX);
    	if(currValue > retVal) {
    		retVal = currValue;
    		bestMove = *it;
    	}
    	copyBoard = board;
    }
    return bestMove;
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}
////////////////////////////////
//
// University of California, Riverside
// CS170 Spring 2015 Assignment 3
// grid.cpp
// 
// You are encouraged to modify and add to this file
//////////////////////////////

#include "grid.h"
#include <iostream>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

using namespace std;
///////////////
/// Utility ///
///////////////

///////////
// Function centerStr
///////////
// In -> string s      - value of string we are centering.
//       int reserve   - # of characters (width).
//       char autofill - char to print for the left and right remaining space.
// Out -> string
//
// Returns a string with length of the reserved space with the string, s, centered.
string centerStr(const string & s, int reserve, char autofill)
{
    int blanks = reserve - s.length();
    int lbuf = blanks / 2; // Add one to favor remainder to right
    int rbuf = (blanks+1) / 2;
    stringstream ss;
    ss << setfill(autofill) << setw(lbuf) << "" << s << setw(rbuf) << "";
    return ss.str();
}

///////////
// Function toString
///////////
// In -> double
// Out -> string
//
// Returns a double converted to a string with fixed precision.
string toString(double value, int prec)
{
    stringstream ss;
    if (value > 0.0)
        ss << '+';
    ss << setprecision(prec) << fixed << value;
    return ss.str();
}

///////////
// Function toString
///////////
// In -> Direction
// Out -> string
//
// Returns an ascii version of Direction as a string.
string toString(Direction dir)
{
    switch (dir)
    {
        case NORTH:
            return "^";
        case EAST:
            return ">";
        case SOUTH:
            return "v";
        case WEST:
            return "<";
        default:
            return "?";
    }
}

///////////////////////
/// GridCell Object ///
///////////////////////

// Constructors
GridCell::GridCell()
    : type(BLANK), reward(0.0), start(false), policy(NONE, 0.0), lRate(0)
{
}

GridCell::GridCell(GridCellType type, double reward, bool start, int lRate)
    : type(type), reward(reward), start(start), policy(NONE, 0.0), lRate(0)
{
}
    
///////////
// Function print
///////////
// In -> string pstr - agent symbol, can be the empty string if not need print.
//       int reserve - the amount of characters (width) we can print for each line.
// Out -> vector<string> - index by row, the output contents of this cell
//
// Returns the output contents of this cell in four different rows.
// Refer to Grid class for more information of the ascii-grid layout
vector<string> GridCell::print(const string & pstr, int reserve) const
{
    vector<string> ret(4);
        
    string prefix1st = "+";
    string prefixedge = "|";
    if (type == OBSTACLE)
    {
        ret[0] = prefix1st + centerStr("", reserve - prefix1st.length(), '-');
        ret[1] = prefixedge + centerStr("xxxx", reserve - prefixedge.length(), ' ');
        ret[2] = prefixedge + centerStr("xxxx", reserve - prefixedge.length(), ' ');
        ret[3] = prefixedge + centerStr("xxxx", reserve - prefixedge.length(), ' ');
    }
    else
    {
        ret[0] = prefix1st + centerStr("", reserve - prefix1st.length(), '-');
        ret[1] = prefixedge + centerStr(toString(reward + policy.second), reserve - prefixedge.length());
        ret[2] = prefixedge + centerStr(start ? pstr + "S" : pstr, reserve - prefixedge.length());
        if (type == TERMINAL)
            ret[3] = prefixedge + centerStr("*", reserve - prefixedge.length());
        else
            ret[3] = prefixedge + centerStr(toString(policy.first), reserve - prefixedge.length());
    }
        
    return ret;
}
    
///////////
// Function getPolicy
///////////
// In ->
// Out -> pair<Direction, double>
//
// Returns the current policy in the form of the pair,
// The first being the action and the the second being the value.
pair<Direction, double> GridCell::getPolicy() const
{
    return policy;
}

///////////////////
/// Grid Object ///
///////////////////
    
///////////////
// Constructors
///////////////
// In -> int n, parameter described by assignment prompt
//       int m, parameter described by assignment prompt
// Out ->
Grid::Grid(int n, int startX, int startY)
{
    bounds = pair<int,int>(n, n);
    grid = vector<vector<GridCell> >(bounds.first, vector<GridCell>(bounds.second));
    startLocation = pair<int,int>(startX, startY);
    
    /* Currently ignores the m parameter in terms of rewards and penalty placement.
        Also ignores n obstacle placement and assigns startLocation to invalid
        location on the grid.
     */
}
    
// Accessors to the 2D grid
GridCell& Grid::operator[](const pair<int,int> & pos)
{
    return grid[pos.first][pos.second];
}
const GridCell& Grid::operator[](const pair<int,int> & pos) const
{
    return grid[pos.first][pos.second];
}
pair<int,int> Grid::getBounds() const
{
    return bounds;
}
int Grid::getRows() const
{
    return bounds.first;
}
int Grid::getCols() const
{
    return bounds.second;
}
pair<int,int> Grid::getStartLocation() const
{
    return startLocation;
}
    
///////////
// Function print
///////////
// In -> pair<int,int> agentPos -
//          position of agent provided. If indicies are negative or out
//          of bounds. This parameter is ignored.
// Out ->
//
// Prints each cell content with special information centered on its row in the following format:
//
// +------
// |value
// |start
// |policy
//
// value - displays the value of the current policy in the format [+/-]##.##
// start - displays 'S' for whether this cell is a start location. A 'P' is also appended
//          if the agentPos is located at that cell.
// policy - displays '^' '>' 'v' '<' if the best policy is NORTH, EAST, SOUTH, or WEST respectively.
//              If policy is set to NONE, it will display '?'
//
// Special cases:
// - If cell is an obstacle, the contents of the cell for each line contain "xxxx" instead
// - if cell is a terminal, the policy displayed for that cell will be '*' instead
//
// Once each cell is printed, it will append a final right and bottom edge
// to complete the ascii table.
void Grid::print(const pair<int,int> & agentPos) const
{
    const int RESERVE = 7;
    for (int i = 0; i < bounds.first; ++i)
    {
        vector<string> outputs(4);
            
        for (int j = 0; j < bounds.second; ++j)
        {
            vector<string> ret;
            if (i == agentPos.first && j == agentPos.second)
                ret = grid[i][j].print("P", RESERVE);
            else
                ret = grid[i][j].print(" ", RESERVE);
            outputs[0] += ret[0];
            outputs[1] += ret[1];
            outputs[2] += ret[2];
            outputs[3] += ret[3];
        }
        cout << outputs[0] << "+" << endl
        << outputs[1] << "|" << endl
        << outputs[2] << "|" << endl
        << outputs[3] << "|" << endl;
    }
    cout << left << setfill('-');
    for (int j = 0; j < bounds.second; ++j)
        cout << setw(RESERVE) << "+";
    cout << "+";
    cout << endl;
}

int main(int argc, char ** argv)
{
	srand(time(NULL)); //change to srand(time(NULL)) later

	if(argc == 3)
	{
		unsigned int n = atoi(argv[1]);
		unsigned int m = atoi(argv[2]);
		vector <int> randRewardX;
		vector <int> randRewardY;
		vector <int> randPenaltyX;
		vector <int> randPenaltyY;
		vector <int> randObstaclesX;
		vector <int> randObstaclesY;
		FirstCase:
		for(unsigned int num = 0; num < m; num++) // This entire for loop randomizes the rewards and penalties on the gridworld.
		{
			unsigned int rewardCount = 0;
			unsigned int penaltyCount = 0;
			int randRX = rand() % n;
			int randRY = rand() % n;
			int PenaltyX = rand() % n;
			int PenaltyY = rand() % n;
			if(num == 0 && (randRX != PenaltyX || randRY != PenaltyY))
			{
				randRewardX.push_back(randRX);
				randRewardY.push_back(randRY);
				randPenaltyX.push_back(PenaltyX);
				randPenaltyY.push_back(PenaltyY);
			}
			else if(num == 0 && randRX == PenaltyX && randRY == PenaltyY)
			{
				goto FirstCase;
			}
			else if(num != 0)
			{
				ResetReward:
				for(unsigned int a = 0; a < randRewardX.size(); a++)
				{
					if((randRewardX.at(a) != randRX || randRewardY.at(a) != randRY) && (randPenaltyX.at(a) != randRX || randPenaltyY.at(a) != randRY)) 
					{
						rewardCount++;
					}
					if(rewardCount == randRewardX.size())
					{
						randRewardX.push_back(randRX);
						randRewardY.push_back(randRY);
						break;
					}
					if(a == randRewardX.size()-1)
					{
						randRX = rand() % n;
						randRY = rand() % n;
						rewardCount = 0;
						goto ResetReward;
					}
				}
				rewardCount = 0;
				ResetPenalty:
				for(unsigned int b = 0; b < randPenaltyY.size(); b++)
				{
					if((randPenaltyX.at(b) != PenaltyX || randPenaltyY.at(b) != PenaltyY) && (randRewardX.at(b) != PenaltyX || randRewardY.at(b) != PenaltyY) 
					&& (PenaltyX != randRX || PenaltyY != randRY))
					{
						penaltyCount++;
					}
					else
					{
						PenaltyX = rand() % n;
						PenaltyY = rand() % n;
						penaltyCount = 0;
						goto ResetPenalty;
					}
					if(penaltyCount == randPenaltyY.size())
					{
						randPenaltyX.push_back(PenaltyX);
						randPenaltyY.push_back(PenaltyY);
						break;
					}
					else if(b == randPenaltyY.size()-1)
					{
						PenaltyX = rand() % n;
						PenaltyY = rand() % n;
						penaltyCount = 0;
						goto ResetPenalty;
					}
				}
				penaltyCount = 0;
			}
		}
		
		for(unsigned int c = 0; c < n; c++) // Setting up randomized points of obstacles in the gridworld that do not overlap with rewards/penalties
		{
			Obstacles:
			int randObsX = rand() % n;
			int randObsY = rand() % n;
			bool checkTrue = false;
			unsigned int obsCount = 0;
			unsigned int repeat = 0;
			for(unsigned int j = 0; j < m; j++)
			{
				if(c == 0 && (((randObsX != randRewardX.at(j)) || (randObsY != randRewardY.at(j))) && ((randObsX != randPenaltyX.at(j)) || (randObsY != randPenaltyY.at(j)))))
				{
					repeat++;
				}
				else if(c == 0 && ((randObsX == randRewardX.at(j) && randObsY == randRewardY.at(j)) || ((randObsX == randPenaltyX.at(j)) && (randObsY == randPenaltyY.at(j))))) 
				{
					goto Obstacles;
				}
				if(c == 0 && repeat == m)
				{
					randObstaclesX.push_back(randObsX);
					randObstaclesY.push_back(randObsY);
					break;
				}
				else if(c != 0)
				{
					for(unsigned int k = 0; k < randObstaclesX.size(); k++)
					{
						if(((randObsX != randRewardX.at(j)) || (randObsY != randRewardY.at(j))) && ((randObsX != randPenaltyX.at(j)) || (randObsY != randPenaltyY.at(j))) 
						&& ((randObsX != randObstaclesX.at(k)) || (randObsY != randObstaclesY.at(k))))
						{
							obsCount++;
						}
						else if(((randObsX == randRewardX.at(j)) && (randObsY == randRewardY.at(j))) || ((randObsX == randPenaltyX.at(j)) && (randObsY == randPenaltyY.at(j))) 
						|| ((randObsX == randObstaclesX.at(k)) && (randObsY == randObstaclesY.at(k)))) 
						{
							goto Obstacles;
						}
						if(obsCount == (randObstaclesX.size()*m))
						{
							randObstaclesX.push_back(randObsX);
							randObstaclesY.push_back(randObsY);
							checkTrue = true;
							break;	
						}
					}
				}
				if(checkTrue)
				{
					checkTrue = false;
					break;
				}
			}
		}
		StartReset: // Selecting a random starting spot for the agent
		int startX = rand() % n;
		int startY = rand() % n;
		for(unsigned int o = 0; o < n; o++)
		{
			if((startX == randObstaclesX.at(o) && startY == randObstaclesY.at(o)))
			{
				goto StartReset;
			}
		}
		for(unsigned int j = 0; j < m; j++)
		{
			if((startX == randRewardX.at(j) && startY == randRewardY.at(j)) || (startX == randPenaltyX.at(j) && startY == randPenaltyY.at(j)))
			{
				goto StartReset;
			}
		}
		
		Grid a = Grid(n, startX, startY);

		for(int i = 0; i < a.getBounds().first; i++) // Initializing the Gridworld with specific attributes
		{
			for(int j = 0; j < a.getBounds().second; j++)
			{
				unsigned int rCount = 0;
				unsigned int pCount = 0;
				unsigned int oCount = 0;
				GridCell g = GridCell();
				for(unsigned int k = 0; k < m; k++)
				{
					if(i != randRewardX.at(k) || j != randRewardY.at(k))
					{
						rCount++;
					}
					if(i != randPenaltyX.at(k) || j != randPenaltyY.at(k))
					{
						pCount++;
					}
				}
				for(unsigned int x = 0; x < n; x++)
				{
					if(i != randObstaclesX.at(x) || j != randObstaclesY.at(x))
					{
						oCount++;
					}
				}
				if(startX == i && startY == j)
				{
					g.type = GridCell::BLANK;
					g.reward = 0.0; 
					g.start = true;
					g.lRate = 0;
					a[make_pair(i,j)] = g;
					continue;
				}
				if(rCount != m) 
				{
					g.type = GridCell::TERMINAL;
					g.reward = 10;
					g.start = false;
					g.lRate = 0;
					a[make_pair(i,j)] = g;
					continue;
				}
				if(pCount != m) 
				{
					g.type = GridCell::TERMINAL;
					g.reward = -10;
					g.start = false;
					g.lRate = 0;
					a[make_pair(i,j)] = g;
					continue;	
				}
				if(oCount != n)
				{
					g.type = GridCell::OBSTACLE;
					g.reward = 0.0;
					g.start = false;
					g.lRate = 0;
					a[make_pair(i,j)] = g;
					continue;
				}
				g.type = GridCell::BLANK;
				g.reward = 0.0;
				g.start = false;
				g.lRate = 0;
				a[make_pair(i,j)] = g;
			}
		}
		a.print(make_pair(startX, startY));
		for(unsigned int i = 0; i < 100000; i++)
		{
			GridCell g = a[a.getStartLocation()];
			int xCoord = a.getStartLocation().first;
			int yCoord = a.getStartLocation().second;
			GridCell f = a[pair<int, int>(xCoord, yCoord)];
			Randomize:
			unsigned int randDirection = 1;//(rand() % 4) + 1;
			if(randDirection == 1)
			{
				if(xCoord == 0)
				{
					goto NumberTwo;
				}
				f = a[pair<int, int> (xCoord-1, yCoord)];
				g.policy = make_pair(NORTH, 0.0);
				if(f.type == GridCell::OBSTACLE)
				{
					goto NumberTwo;
				}
				else if(f.type == GridCell::TERMINAL)
				{
					double maxFirst = 0.1*a[pair<int,int>(xCoord, yCoord-1)].policy.second;
					double maxSecond = 0.1*a[pair<int,int>(xCoord, yCoord+1)].policy.second;
					if(yCoord == 0)
					{
						maxFirst = 0.0; 
					}
					if(yCoord == n)
					{
						maxSecond = 0.0; 
					}
					double maxThree = 0.8 * f.reward;
					double maxNum = maxFirst + maxSecond + maxThree; 
					f.lRate++;
					double eq = g.policy.second + (60.0/(f.lRate+59))*(g.reward + ((0.9*maxNum) - g.policy.second));
					if(maxThree > maxSecond && maxThree > maxFirst)
					{
						g.policy = make_pair(NORTH, eq); 
					}
					else if(maxSecond > maxThree && maxSecond > maxFirst)
					{
						g.policy = make_pair(EAST, eq);
					}
					else if(maxFirst > maxThree && maxFirst > maxThree)
					{
						g.policy = make_pair(WEST, eq);
					}
					a[pair<int, int>(xCoord, yCoord)] = g;
					continue;
					//cout << "MaxNum:" << maxNum << endl;
					//cout << toString(g.getPolicy().first) << endl;
					//a.print(make_pair(xCoord, yCoord));
				}
				else if(f.type == GridCell::BLANK)
				{
					double maxFirst = 0.1*a[pair<int,int>(xCoord, yCoord-1)].policy.second;
					double maxSecond = 0.1*a[pair<int,int>(xCoord, yCoord+1)].policy.second;
					if(yCoord == 0)
					{
						maxFirst = 0.0; 
					}
					if(yCoord == n)
					{
						maxSecond = 0.0; 
					}
					double maxThree = 0.8 * f.reward;
					double maxNum = maxFirst + maxSecond + maxThree; 
					f.lRate++;
					double eq = g.policy.second + (60.0/(f.lRate+59))*(g.reward + ((0.9*maxNum) - g.policy.second));
					if(maxThree > maxSecond && maxThree > maxFirst)
					{
						g.policy = make_pair(NORTH, eq); 
					}
					else if(maxSecond > maxThree && maxSecond > maxFirst)
					{
						g.policy = make_pair(EAST, eq);
					}
					else if(maxFirst > maxThree && maxFirst > maxThree)
					{
						g.policy = make_pair(WEST, eq);
					}
					else
					{
						g.policy = make_pair(NORTH, eq);
					}
					a[pair<int, int>(xCoord, yCoord)] = g;
					g = a[pair<int,int>(xCoord-1, yCoord)];
					goto Randomize;
				}
			}
			else if(randDirection == 2)
			{
				NumberTwo:
				;	
			}
			else if(randDirection == 3)
			{
				GridCell g = a[a.getStartLocation()];

			}
			else if(randDirection == 4)
			{
				GridCell g = a[a.getStartLocation()];
			}
		}
		//a.print(make_pair(startX, startY));
		//cout << a.getBounds().first << " " << a.getBounds().second << endl;
		//cout << a.getRows() << " " << a.getCols() << endl;
		//cout << a.getStartLocation().first << " " << a.getStartLocation().second << endl;
		cout << "Rewards: " << endl;  // Testing Purposes
		for(unsigned int i = 0; i < m; i++)
		{
			cout << randRewardX[i] << " " << randRewardY[i] << endl;
		}
		cout << "Penalties: " << endl;
		for(unsigned int i = 0; i < m; i++)
		{
			cout << randPenaltyX[i] << " " << randPenaltyY[i] << endl;
		}
		cout << "Obstacles: " << endl;
		for(unsigned int i = 0; i < n; i++)
		{
			cout << randObstaclesX[i] << " " << randObstaclesY[i] << endl;
		}
		cout << "Starting Point: " << endl << startX << " " << startY << endl;
	}
	else
	{
		cout << "Wrong amount of arguments" << endl;
	}
	return 0;
}

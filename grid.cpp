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
    : type(BLANK), reward(0.0), start(false), policy(NONE, 0.0)
{
}

GridCell::GridCell(GridCellType type, double reward, bool start)
    : type(type), reward(reward), start(start), policy(NONE, 0.0)
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
Grid::Grid(int n, int m)
{
    bounds = pair<int,int>(n, n);
    grid = vector<vector<GridCell> >(bounds.first, vector<GridCell>(bounds.second));
    startLocation = pair<int,int>(-1, -1);
    
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
		int n = atoi(argv[1]);
		int m = atoi(argv[2]);
		Grid a = Grid(n, m);
		vector <int> randRewardX;
		vector <int> randRewardY;
		vector <int> randPenaltyX;
		vector <int> randPenaltyY;
		vector <int> randObstaclesX;
		vector <int> randObstaclesY;
		FirstCase:
		for(unsigned int num = 0; num < m; num++) // This entire for loop randomizes the rewards and penalties on the gridworld.
		{
			int rewardCount = 0;
			int penaltyCount = 0;
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
		
		for(unsigned int c = 0; c < n; c++)
		{
			int randObsX = rand() % n;
			int randObsY = rand() % n;
			int obsCount = 0;
			int repeat = 0;
			Obstacles:
			for(unsigned int j = 0; j < m; j++)
			{
				if(c == 0 && (randObsX != randRewardX[j] || randObsY != randRewardY[j]) && (randObsX != randPenaltyX[j] || randObsY != randPenaltyY[j]))
				{
					repeat++;
				}
				if(repeat == m)
				{
					randObstaclesX.push_back(randObsX);
					randObstaclesY.push_back(randObsY);
					break;
				}
				else if(c == 0 && (randObsX == randRewardX[j] || randObsY == randRewardY[j]) && (randObsX == randPenaltyX[j] || randObsY == randPenaltyY[j]))
				{
					randObsX = rand() % n;
					randObsY = rand() % n;
					repeat = 0;
					goto Obstacles;
				}
				else if(c != 0)
				{
					ResetObs:
					for(unsigned int k = 0; k < randObstaclesX.size(); k++)
					{
						if((randObsX != randRewardX.at(j) || randObsY != randRewardY.at(j)) && (randObsX != randPenaltyX.at(j) || randObsY != randPenaltyY.at(j)) 
						&& (randObsX != randObstaclesX.at(k)) || randObsY != randObstaclesY.at(k))
						{
							obsCount++;
						}
						if(obsCount == randObstaclesX.size())
						{
							randObstaclesX.push_back(randObsX);
							randObstaclesY.push_back(randObsY);
							break;	
						}
						else if(k ==  (randObstaclesX.size()-1))
						{
							randObsX = rand() % n;
							randObsY = rand() % n;
							obsCount = 0;
							goto ResetObs;
						}
					}
					obsCount = 0;
				}
			}
		}
		/*for(unsigned int i = 0; i < a.getBounds().first; i++) // Initializing the Gridworld with specific attributes
		{
			for(unsigned int j = 0; j < a.getBounds().second; j++)
			{
				GridCell g = grid[i][j];
				if(i == randRewardX && j == randRewardY)
				{
					g.type = TERMINAL;
					g.reward = m; 
					g.start = false;
				}
				if(i == randPenaltyX && j == randPenaltyY) 
				{
					g.type = TERMINAL;
					g.reward = (-1*m);
					g.start = false;
				}
				else if(i
			}
		}
		for(unsigned int i = 0; i < m; i++)
		{
			cout << "Pairs: " << randRewardX.at(i) << "," << randRewardY.at(i) << endl;
			cout << "Penalties: " << randPenaltyX.at(i) << "," << randPenaltyY.at(i) << endl;
		}*/
		cout << "Rewards: " << endl;
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
	}
	else
	{
		cout << "Wrong amount of arguments" << endl;
	}
	return 0;
}
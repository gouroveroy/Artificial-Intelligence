#include <bits/stdc++.h>
#include "NPuzzle.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    function<int(vector<vector<int>> &, vector<vector<int>> &)> heuristicFunction = Heuristic::getLinearConflict;
    if (argc > 1)
    {
        string heuristicName = argv[1];
        heuristicFunction = Heuristic::getHeuristicFunction(heuristicName);
    }

    int k = 3;
    if (argc > 2)
    {
        k = atoi(argv[2]);
    }

    vector<vector<int>> initialBoard(k, vector<int>(k, 0));

    if (argc > 3)
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                initialBoard[i][j] = atoi(argv[3 + i * k + j]);
            }
        }
    }

    vector<vector<int>> goalBoard(k, vector<int>(k, 0));

    int g = 0;
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (g == k * k - 1)
            {
                goalBoard[i][j] = 0;
            }
            else
            {
                goalBoard[i][j] = ++g;
            }
        }
    }

    NPuzzle puzzle(k, initialBoard, goalBoard);
    puzzle.setHeuristicFunction(heuristicFunction);

    pair<Node, pair<int, int>> result = puzzle.AStarSearch();
    Node solutionNode = result.first;

    if (solutionNode.getMoves() == -1)
    {
        cout << "Unsolvable puzzle" << endl;
        return 0;
    }

    pair<int, int> nodeCounts = result.second;
    int exploredNodes = nodeCounts.first;
    int expandedNodes = nodeCounts.second;

    cout << "Minimum number of moves = " << solutionNode.getMoves() << endl << endl;

    solutionNode.printPath();

    cout << "Heuristic function: " << Heuristic::getHeuristicName(heuristicFunction) << endl;
    cout << "Explored Nodes: " << exploredNodes << endl;
    cout << "Expanded Nodes: " << expandedNodes << endl;
    return 0;
}

#include <bits/stdc++.h>
#include "NPuzzle.cpp"
#include "Heuristic.cpp"
using namespace std;

int main()
{
    int k = 3;
    vector<vector<int>> initialBoard = {
        {0, 1, 3},
        {4, 2, 5},
        {7, 8, 6}};

    vector<vector<int>> goalBoard = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}};

    NPuzzle puzzle(k, initialBoard, goalBoard);
    Heuristic heuristicFunction = Heuristic::LinearConflict;
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

    cout << "Minimum number of moves = " << solutionNode.getMoves() << endl
         << endl;

    solutionNode.printPath();

    cout << "Heuristic function: " << getHeuristicName(heuristicFunction) << endl;
    cout << "Explored Nodes: " << exploredNodes << endl;
    cout << "Expanded Nodes: " << expandedNodes << endl;
    return 0;
}

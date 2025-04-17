#include <bits/stdc++.h>
#include "NPuzzle.cpp"
using namespace std;

int main()
{
    int k = 3;
    vector<vector<int>> initialBoard = {
        {1, 2, 3},
        {4, 5, 6},
        {8, 7, 0}};

    vector<vector<int>> goalBoard = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}};

    vector<string> heuristicFunctions = {"Hamming", "Manhattan", "Euclidean", "Linear Conflict"};
    int heuristicFunctionIndex = 3;
    NPuzzle puzzle(k, initialBoard, goalBoard);
    puzzle.setHeuristicFunction(heuristicFunctions[heuristicFunctionIndex]);

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

    cout << "For the heuristic function: " << heuristicFunctions[heuristicFunctionIndex] << endl;
    cout << "Explored Nodes: " << exploredNodes << endl;
    cout << "Expanded Nodes: " << expandedNodes << endl;
    return 0;
}

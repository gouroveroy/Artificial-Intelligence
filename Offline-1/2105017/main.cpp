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
    vector<vector<int>> initialBoard = {
        {1, 5, 0},
        {7, 6, 4},
        {2, 3, 8}};

    if (argc > 2)
    {
        k = atoi(argv[2]);
        initialBoard.resize(k, vector<int>(k, 0));
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                initialBoard[i][j] = atoi(argv[3 + i * k + j]);
            }
        }
    }

    vector<vector<int>> goalBoard = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}};

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

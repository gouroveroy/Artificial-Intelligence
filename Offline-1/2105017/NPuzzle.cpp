#include <bits/stdc++.h>
#include "Node.cpp"
#include "Heuristic.cpp"
using namespace std;

class NPuzzle
{
    int k;
    shared_ptr<Node> initialNode;
    vector<vector<int>> goalBoard;
    vector<vector<int>> initialBoard;
    function<int(vector<vector<int>> &, vector<vector<int>> &)> heuristicFunction;

public:
    NPuzzle(int k, vector<vector<int>> &initialBoard, vector<vector<int>> &goalBoard)
    {
        this->k = k;
        this->goalBoard = goalBoard;
        this->initialBoard = initialBoard;
        this->initialNode = make_shared<Node>(initialBoard);
    }

    void setHeuristicFunction(function<int(vector<vector<int>> &, vector<vector<int>> &)> heuristic)
    {
        this->heuristicFunction = heuristic;
    }

    int countInversions()
    {
        int inversions = 0;
        vector<int> rowMajorOrder;
        for (int i = 0; i < initialBoard.size(); i++)
        {
            for (int j = 0; j < initialBoard[i].size(); j++)
            {
                if (initialBoard[i][j] != 0)
                {
                    rowMajorOrder.push_back(initialBoard[i][j]);
                }
            }
        }

        for (int i = 0; i < rowMajorOrder.size(); i++)
        {
            for (int j = i + 1; j < rowMajorOrder.size(); j++)
            {
                if (rowMajorOrder[i] > rowMajorOrder[j])
                {
                    inversions++;
                }
            }
        }
        return inversions;
    }

    bool isSolvable()
    {
        int inversions = countInversions();
        if (k % 2 == 0)
        {
            int emptyRow = initialNode->getEmptyCell().first + 1;
            return (inversions + emptyRow) % 2 == 0;
        }

        return inversions % 2 == 0;
    }

    pair<Node, pair<int, int>> AStarSearch()
    {
        if (!isSolvable())
        {
            vector<vector<int>> emptyBoard(k, vector<int>(k, -1));
            return {Node(emptyBoard, -1, nullptr), {0, 0}};
        }

        int exploredNodes = 0;
        int expandedNodes = 0;

        auto comparator = [&](Node &a, Node &b)
        {
            double g_a = a.getMoves();
            double g_b = b.getMoves();

            auto boardCopyA = a.getBoard();
            auto boardCopyB = b.getBoard();

            double h_a = heuristicFunction(boardCopyA, goalBoard);
            double h_b = heuristicFunction(boardCopyB, goalBoard);

            double f_a = g_a + h_a;
            double f_b = g_b + h_b;

            if (f_a == f_b)
            {
                return h_a > h_b;
            }

            return f_a > f_b;
        };

        priority_queue<Node, vector<Node>, decltype(comparator)> openList(comparator);
        unordered_map<string, int> closedList;

        openList.push(*initialNode);

        while (!openList.empty())
        {
            Node currentNode = openList.top();
            string boardString = currentNode.getBoardString();

            if (closedList.find(boardString) != closedList.end() && closedList[boardString] <= currentNode.getMoves())
            {
                openList.pop();
                continue;
            }

            closedList[boardString] = currentNode.getMoves();
            openList.pop();
            expandedNodes++;

            if (currentNode.isGoalState())
            {
                return {currentNode, {exploredNodes, expandedNodes}};
            }

            vector<Node> neighbors = currentNode.getNeighboringBoardConfigurations();
            for (Node &neighbor : neighbors)
            {
                if (closedList.find(neighbor.getBoardString()) == closedList.end())
                {
                    exploredNodes++;
                    openList.push(neighbor);
                }
            }
        }
        vector<vector<int>> emptyBoard(k, vector<int>(k, -1));
        return {Node(emptyBoard, -1, nullptr), {exploredNodes, expandedNodes}};
    }
};

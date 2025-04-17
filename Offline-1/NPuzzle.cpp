#include <bits/stdc++.h>
#include "Node.cpp"
#include "Heuristic.cpp"
using namespace std;

class NPuzzle
{
    int k;
    vector<vector<int>> initialBoard;
    vector<vector<int>> goalBoard;
    Heuristic heuristicFunction;
    shared_ptr<Node> initialNode;

public:
    NPuzzle(int k, vector<vector<int>> &initialBoard, vector<vector<int>> &goalBoard)
    {
        this->k = k;
        this->initialBoard = initialBoard;
        this->goalBoard = goalBoard;
        this->initialNode = make_shared<Node>(initialBoard);
    }

    vector<vector<int>> getInitialBoard()
    {
        return initialBoard;
    }

    void setHeuristicFunction(Heuristic heuristic)
    {
        this->heuristicFunction = heuristic;
    }

    vector<vector<int>> getGoalBoard()
    {
        return goalBoard;
    }

    int getK()
    {
        return k;
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
        else
        {
            return inversions % 2 == 0;
        }
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
            int f_a, f_b;

            switch (heuristicFunction)
            {
            case Heuristic::Hamming:
                f_a = a.getHammingDistance() + a.getMoves();
                f_b = b.getHammingDistance() + b.getMoves();
                break;

            case Heuristic::Manhattan:
                f_a = a.getManhattanDistance() + a.getMoves();
                f_b = b.getManhattanDistance() + b.getMoves();
                break;

            case Heuristic::Euclidean:
                f_a = a.getEuclideanDistance() + a.getMoves();
                f_b = b.getEuclideanDistance() + b.getMoves();
                break;

            case Heuristic::LinearConflict:
                f_a = a.getLinearConflict() + a.getMoves();
                f_b = b.getLinearConflict() + b.getMoves();
                break;

            default:
                f_a = f_b = 0;
            }

            return f_a > f_b;
        };

        priority_queue<Node, vector<Node>, decltype(comparator)> openList(comparator);
        unordered_set<string> closedList;

        openList.push(*initialNode);

        while (!openList.empty())
        {
            Node currentNode = openList.top();
            closedList.insert(currentNode.getBoardString());
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

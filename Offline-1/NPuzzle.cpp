#include <bits/stdc++.h>
#include "Node.cpp"
#include "Heuristic.cpp"
using namespace std;

/* This code snippet defines a C++ class named `NPuzzle` that represents a puzzle-solving algorithm for
the N-Puzzle problem. Here is a breakdown of the class members: */
class NPuzzle
{
    int k;
    shared_ptr<Node> initialNode;
    vector<vector<int>> goalBoard;
    vector<vector<int>> initialBoard;
    function<int(vector<vector<int>> &, vector<vector<int>> &)> heuristicFunction;

public:
    /**
     * The NPuzzle constructor initializes the puzzle with the specified dimensions and initial and
     * goal boards.
     *
     * @param k The parameter `k` in the `NPuzzle` constructor represents the size of the puzzle board.
     * It is used to specify the dimensions of the puzzle board, which is typically a square grid with
     * k rows and k columns.
     * @param initialBoard The `initialBoard` parameter is a 2D vector representing the initial state
     * of the puzzle board. Each element in the vector represents a tile on the board, and the values
     * in the elements indicate the number on the tile at that position.
     * @param goalBoard The `goalBoard` parameter represents the final configuration of the puzzle
     * board that the algorithm is trying to reach. It is a 2D vector that contains the desired
     * arrangement of numbers on the puzzle board. The algorithm will work towards transforming the
     * `initialBoard` configuration into this `goalBoard` configuration
     */
    NPuzzle(int k, vector<vector<int>> &initialBoard, vector<vector<int>> &goalBoard)
    {
        this->k = k;
        this->goalBoard = goalBoard;
        this->initialBoard = initialBoard;
        this->initialNode = make_shared<Node>(initialBoard);
    }

    /**
     * The function `setHeuristicFunction` assigns a given heuristic function to a member variable.
     *
     * @param heuristic The `setHeuristicFunction` function takes a parameter `heuristic` which is a
     * function that accepts two parameters - both of type `vector<vector<int>> &` - and returns an
     * integer value. This function is used to set the heuristic function for a specific purpose, such
     * as in a search
     */
    void setHeuristicFunction(function<int(vector<vector<int>> &, vector<vector<int>> &)> heuristic)
    {
        this->heuristicFunction = heuristic;
    }

    /**
     * The function `countInversions` calculates the number of inversions in a given board
     * configuration.
     *
     * @return The function `countInversions` returns the number of inversions in the `initialBoard`
     * vector.
     */
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

    /**
     * The function "isSolvable" determines if a puzzle is solvable based on the number of inversions
     * and the position of the empty cell.
     *
     * @return The `isSolvable` function is returning a boolean value based on whether the puzzle is
     * solvable or not. It calculates the number of inversions in the puzzle and checks if the puzzle
     * is solvable based on the number of inversions and the position of the empty cell. If the puzzle
     * size is even, it considers the row number of the empty cell in the calculation. The function
     * returns true
     */
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

    /**
     * The above function implements the A* search algorithm to find the optimal solution for a given
     * puzzle problem.
     *
     * @return The AStarSearch function returns a pair containing a Node object and a pair of integers.
     * The Node object represents the final state of the puzzle after performing the A* search
     * algorithm, and the pair of integers represents the number of nodes explored and expanded during
     * the search process.
     */
    pair<Node, pair<int, int>> AStarSearch()
    {
        if (!isSolvable())
        {
            vector<vector<int>> emptyBoard(k, vector<int>(k, -1));
            return {Node(emptyBoard, -1, nullptr), {0, 0}};
        }

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

        int exploredNodes = 1;
        int expandedNodes = 0;
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

            if (currentNode.isGoalState())
            {
                return {currentNode, {exploredNodes, expandedNodes}};
            }
            expandedNodes++;

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

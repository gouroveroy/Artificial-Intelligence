#include <bits/stdc++.h>

using namespace std;

/* This class is named Heuristic and is written in C++. */
class Heuristic
{
public:
    /* The line `static int (*heuristicFunction)(vector<vector<int>> &, vector<vector<int>> &);` in the
    `Heuristic` class is declaring a static member variable named `heuristicFunction`. This variable
    is a function pointer that points to a function taking two parameters: a reference to a vector
    of vectors of integers (representing a board state) and another reference to a vector of vectors
    of integers (representing a goal board state), and returning an integer. */
    static int (*heuristicFunction)(vector<vector<int>> &, vector<vector<int>> &);

    /**
     * The function calculates the Hamming distance between two 2D vectors representing boards.
     * 
     * @param board The `board` parameter is a 2D vector representing the current state of the puzzle
     * board. Each element in the vector represents a row of the board, and the values within each row
     * represent the tiles on the board.
     * @param goalBoard The `goalBoard` parameter represents the target board configuration that you
     * want to achieve. The function `getHammingDistance` calculates the Hamming distance between the
     * current board configuration (`board`) and the target board configuration (`goalBoard`). The
     * Hamming distance is the number of tiles that are in the
     * 
     * @return The function `getHammingDistance` returns the Hamming distance between two 2D vectors
     * `board` and `goalBoard`. The Hamming distance is calculated by comparing the elements of the two
     * vectors at corresponding positions and counting the number of positions where the elements are
     * different (excluding 0 values in `board`).
     */
    static int getHammingDistance(vector<vector<int>> &board, vector<vector<int>> &goalBoard)
    {
        int distance = 0;
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                if (board[i][j] != goalBoard[i][j] && board[i][j] != 0)
                {
                    distance++;
                }
            }
        }
        return distance;
    }

    /**
     * The function calculates the Manhattan distance between a given board configuration and a goal
     * board configuration.
     * 
     * @param board The `board` parameter is a 2D vector representing the current state of the board in
     * a puzzle game. Each element in the vector represents a row, and the elements within each row
     * represent the values in the columns of the board.
     * @param goalBoard The `goalBoard` parameter represents the target configuration of the board that
     * we want to achieve. It is a 2D vector representing the desired state of the board where each
     * element corresponds to a tile value at a specific position.
     * 
     * @return The function `getManhattanDistance` returns the Manhattan distance between the current
     * board state (`board`) and the goal board state (`goalBoard`).
     */
    static int getManhattanDistance(vector<vector<int>> &board, vector<vector<int>> &goalBoard)
    {
        int distance = 0;
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                if (board[i][j] != goalBoard[i][j] && board[i][j] != 0)
                {
                    int value = board[i][j];
                    int goalRow = (value - 1) / board.size();
                    int goalCol = (value - 1) % board.size();
                    distance += abs(i - goalRow) + abs(j - goalCol);
                }
            }
        }
        return distance;
    }

    /**
     * The function calculates the Euclidean distance between a given board state and a goal board
     * state in a sliding puzzle game.
     * 
     * @param board The `board` parameter is a 2D vector representing the current state of the board in
     * a puzzle game. Each element in the vector represents a row, and each row contains integers
     * representing the values in each column of the board.
     * @param goalBoard The `goalBoard` parameter represents the target configuration of the board that
     * you are trying to achieve. It is a 2D vector that stores the values of the board in the goal
     * state. The function `getEuclideanDistance` calculates the Euclidean distance between the current
     * board configuration (`board`)
     * 
     * @return The function `getEuclideanDistance` returns an integer value representing the Euclidean
     * distance between the current board state (`board`) and the goal board state (`goalBoard`).
     */
    static int getEuclideanDistance(vector<vector<int>> &board, vector<vector<int>> &goalBoard)
    {
        int distance = 0;
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                if (board[i][j] != goalBoard[i][j] && board[i][j] != 0)
                {
                    int value = board[i][j];
                    int goalRow = (value - 1) / board.size();
                    int goalCol = (value - 1) % board.size();
                    distance += sqrt(pow(i - goalRow, 2) + pow(j - goalCol, 2));
                }
            }
        }
        return distance;
    }

    /**
     * The function `getLinearConflict` calculates the linear conflict heuristic for the 15-puzzle game
     * in C++.
     * 
     * @param board The `board` parameter represents the current state of the puzzle board, stored as a
     * 2D vector of integers.
     * @param goalBoard The `goalBoard` parameter represents the target configuration of the board that
     * you are trying to achieve. It is a 2D vector that stores the correct arrangement of tiles on the
     * board. The `getLinearConflict` function calculates the linear conflicts between the current
     * board configuration (`board`) and the goal
     * 
     * @return The function `getLinearConflict` returns the sum of the Manhattan distance between the
     * current board configuration and the goal board configuration, and twice the total number of
     * linear conflicts in the board.
     */
    static int getLinearConflict(vector<vector<int>> &board, vector<vector<int>> &goalBoard)
    {
        int conflict = 0;
        int n = board.size();

        // Row conflicts
        /*
        for (each row) {
            for (each pair of tiles in that row) {
                if (both tiles are in their correct row,
                    and they are in the wrong order) {
                    conflict++;
                }
            }
        }
        */
        for (int row = 0; row < n; row++)
        {
            for (int i = 0; i < n; i++)
            {
                int tile1 = board[row][i];
                if (tile1 == 0)
                {
                    continue;
                }
                int goalRow1 = (tile1 - 1) / n;
                if (goalRow1 != row)
                {
                    continue;
                }

                for (int j = i + 1; j < n; j++)
                {
                    int tile2 = board[row][j];
                    if (tile2 == 0)
                    {
                        continue;
                    }
                    int goalRow2 = (tile2 - 1) / n;
                    if (goalRow2 != row)
                    {
                        continue;
                    }

                    int goalCol1 = (tile1 - 1) % n;
                    int goalCol2 = (tile2 - 1) % n;
                    if (goalCol1 > goalCol2)
                    {
                        conflict++;
                    }
                }
            }
        }

        // Column conflicts
        /*
        for (each column) {
            for (each pair of tiles in that column) {
                if (both tiles are in their correct column,
                    and they are in the wrong order) {
                    conflict++;
                }
            }
        }
        */
        for (int col = 0; col < n; col++)
        {
            for (int i = 0; i < n; i++)
            {
                int tile1 = board[i][col];
                if (tile1 == 0)
                {
                    continue;
                }
                int goalCol1 = (tile1 - 1) % n;
                if (goalCol1 != col)
                {
                    continue;
                }

                for (int j = i + 1; j < n; j++)
                {
                    int tile2 = board[j][col];
                    if (tile2 == 0)
                    {
                        continue;
                    }
                    int goalCol2 = (tile2 - 1) % n;
                    if (goalCol2 != col)
                    {
                        continue;
                    }

                    int goalRow1 = (tile1 - 1) / n;
                    int goalRow2 = (tile2 - 1) / n;
                    if (goalRow1 > goalRow2)
                    {
                        conflict++;
                    }
                }
            }
        }
        return getManhattanDistance(board, goalBoard) + 2 * conflict;
    }

    /**
     * The function `getHeuristicFunction` assigns a specific heuristic function based on the input
     * string name or defaults to a Hamming distance heuristic.
     * 
     * @param name The function `getHeuristicFunction` takes a string parameter `name` as input. This
     * function checks the value of the `name` parameter and assigns a corresponding heuristic function
     * to the `heuristicFunction` variable based on the value of `name`. The heuristic functions like
     * `getHammingDistance
     * 
     * @return The function `getHeuristicFunction` returns a function pointer to one of the heuristic
     * functions based on the input `name`.
     */
    static function<int(vector<vector<int>> &, vector<vector<int>> &)> getHeuristicFunction(string name)
    {
        if (name == "hamming")
        {
            heuristicFunction = &getHammingDistance;
        }
        else if (name == "manhattan")
        {
            heuristicFunction = &getManhattanDistance;
        }
        else if (name == "euclidean")
        {
            heuristicFunction = &getEuclideanDistance;
        }
        else if (name == "linearconflict")
        {
            heuristicFunction = &getLinearConflict;
        }
        else
        {
            heuristicFunction = &getHammingDistance;
        }

        return heuristicFunction;
    }

    /**
     * The function `getHeuristicName` determines the name of a given heuristic function based on its
     * target pointer.
     * 
     * @param heuristicFunction The `heuristicFunction` parameter is a function that takes two
     * parameters - a reference to a 2D vector of integers representing the current state and a
     * reference to a 2D vector of integers representing the target state. The function returns an
     * integer value.
     * 
     * @return The `getHeuristicName` function returns the name of the heuristic function passed to it
     * as a parameter. If the heuristic function is one of the predefined functions
     * (`getHammingDistance`, `getManhattanDistance`, `getEuclideanDistance`, `getLinearConflict`), it
     * returns the corresponding name ("Hamming", "Manhattan", "Euclidean", "Linear Conflict"). If the
     * heuristic
     */
    static string getHeuristicName(function<int(vector<vector<int>> &, vector<vector<int>> &)> heuristicFunction)
    {
        auto targetPtr = heuristicFunction.target<int (*)(vector<vector<int>> &, vector<vector<int>> &)>();
        if (targetPtr == nullptr)
        {
            return "unknown";
        }
        if (*targetPtr == &getHammingDistance)
        {
            return "Hamming";
        }
        if (*targetPtr == &getManhattanDistance)
        {
            return "Manhattan";
        }
        if (*targetPtr == &getEuclideanDistance)
        {
            return "Euclidean";
        }
        if (*targetPtr == &getLinearConflict)
        {
            return "Linear Conflict";
        }

        return "unknown";
    }
};

/* The line `int (*Heuristic::heuristicFunction)(vector<vector<int>> &, vector<vector<int>> &) =
Heuristic::getHammingDistance;` in the `Heuristic` class is initializing the static member variable
`heuristicFunction` with the address of the `getHammingDistance` function. This means that by
default, the `heuristicFunction` static member variable will point to the `getHammingDistance`
function when the `Heuristic` class is used unless explicitly changed using the
`getHeuristicFunction` method with a different heuristic name. */
int (*Heuristic::heuristicFunction)(vector<vector<int>> &, vector<vector<int>> &) = Heuristic::getHammingDistance;

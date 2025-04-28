#include <bits/stdc++.h>

using namespace std;

class Heuristic
{
public:
    static int (*heuristicFunction)(vector<vector<int>> &, vector<vector<int>> &);

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

int (*Heuristic::heuristicFunction)(vector<vector<int>> &, vector<vector<int>> &) = Heuristic::getHammingDistance;

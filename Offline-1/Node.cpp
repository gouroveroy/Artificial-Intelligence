#include <bits/stdc++.h>
using namespace std;

class Node
{
    vector<vector<int>> board;
    int moves;
    shared_ptr<Node> parent;
    pair<int, int> emptyCell;

public:
    Node(vector<vector<int>> &board, int moves = 0, shared_ptr<Node> parent = nullptr)
    {
        this->board.resize(board.size(), vector<int>(board[0].size()));
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                if (board[i][j] == 0)
                {
                    emptyCell = {i, j};
                }
                this->board[i][j] = board[i][j];
            }
        }

        this->moves = moves;
        this->parent = parent;
    }

    Node(const Node &node)
    {
        this->board = node.board;
        this->emptyCell = node.emptyCell;
        this->parent = node.parent;
        this->moves = node.moves;
    }

    vector<vector<int>> getBoard()
    {
        return board;
    }

    shared_ptr<Node> getParent()
    {
        return parent;
    }

    int getMoves()
    {
        return moves;
    }

    pair<int, int> getEmptyCell()
    {
        return emptyCell;
    }

    Node getCurrentNode()
    {
        return *this;
    }

    bool isValidCell(int i, int j)
    {
        return (i >= 0 && i < board.size() && j >= 0 && j < board.size());
    }

    bool isEqual(const Node &node)
    {
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board.size(); j++)
            {
                if (board[i][j] != node.board[i][j])
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool isGoalState()
    {
        int n = board.size();
        int k = 1;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == n - 1 && j == n - 1)
                {
                    return board[i][j] == 0;
                }
                if (board[i][j] != k++)
                {
                    return false;
                }
            }
        }
        return true;
    }

    int getHammingDistance()
    {
        int distance = 0;
        int n = board.size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (board[i][j] != 0 && board[i][j] != i * n + j + 1)
                {
                    distance++;
                }
            }
        }
        return distance;
    }

    int getManhattanDistance()
    {
        int distance = 0;
        int n = board.size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (board[i][j] != 0)
                {
                    int targetRow = (board[i][j] - 1) / n;
                    int targetCol = (board[i][j] - 1) % n;
                    distance += abs(i - targetRow) + abs(j - targetCol);
                }
            }
        }
        return distance;
    }

    int getEuclideanDistance()
    {
        int distance = 0;
        int n = board.size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (board[i][j] != 0)
                {
                    int targetRow = (board[i][j] - 1) / n;
                    int targetCol = (board[i][j] - 1) % n;
                    distance += sqrt(pow(i - targetRow, 2) + pow(j - targetCol, 2));
                }
            }
        }
        return distance;
    }

    int getLinearConflict()
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
        return getManhattanDistance() + 2 * conflict;
    }

    vector<Node> getNeighboringBoardConfigurations()
    {
        // direction matrix : left, down, up, right
        vector<int> dx = {-1, 0, 0, 1};
        vector<int> dy = {0, -1, 1, 0};

        vector<Node> children;

        for (int i = 0; i < 4; i++)
        {
            int di = emptyCell.first + dx[i];
            int dj = emptyCell.second + dy[i];

            if (isValidCell(di, dj))
            {
                vector<vector<int>> newBoard = board;
                swap(newBoard[emptyCell.first][emptyCell.second], newBoard[di][dj]);
                Node child(newBoard, moves + 1, make_shared<Node>(*this));
                children.push_back(child);
            }
        }
        return children;
    }

    string getBoardString()
    {
        string boardString = "";
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                boardString += to_string(board[i][j]);
            }
        }
        return boardString;
    }

    void printBoard()
    {
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board.size(); j++)
            {
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    void printPath()
    {
        if (parent != nullptr)
        {
            parent->printPath();
        }
        printBoard();
    }
};

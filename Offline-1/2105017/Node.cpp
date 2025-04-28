#include <bits/stdc++.h>
using namespace std;

class Node
{
    int moves;
    shared_ptr<Node> parent;
    pair<int, int> emptyCell;
    vector<vector<int>> board;

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

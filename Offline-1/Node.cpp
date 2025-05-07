#include <bits/stdc++.h>
using namespace std;

/* This code snippet defines a C++ class named `Node` that represents a node in a puzzle-solving
algorithm. Here is a breakdown of the member variables of the `Node` class: */
class Node
{
    int moves;
    shared_ptr<Node> parent;
    pair<int, int> emptyCell;
    vector<vector<int>> board;

public:
    /**
     * The Node constructor initializes a Node object with a given board configuration, number of
     * moves, and parent Node.
     * 
     * @param board The `board` parameter is a 2D vector representing the game board. Each element of
     * the vector is a row of the board, and each element within a row represents a cell on the board.
     * The value of each cell is an integer, where 0 typically represents an empty cell.
     * @param moves The `moves` parameter in the `Node` constructor represents the number of moves
     * taken to reach the current state of the board. It is an integer value that indicates how many
     * moves have been made from the initial state to the current state represented by this node in a
     * game or puzzle-solving scenario.
     * @param parent The `parent` parameter in the `Node` constructor is a `shared_ptr` that points to
     * the parent node of the current node being created. It is used to maintain the parent-child
     * relationship in a tree structure. By keeping track of the parent node, you can easily navigate
     * up the tree from
     */
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

    /**
     * The above function is a copy constructor for a Node class in C++ that initializes a new Node
     * object with the same values as an existing Node object.
     * 
     * @param node The `Node` class has a copy constructor that takes a reference to another `Node`
     * object as a parameter. The constructor initializes the new `Node` object with the same values
     * for `board`, `emptyCell`, `parent`, and `moves` as the `Node` object passed in as
     */
    Node(const Node &node)
    {
        this->board = node.board;
        this->emptyCell = node.emptyCell;
        this->parent = node.parent;
        this->moves = node.moves;
    }

    /**
     * The function `getBoard` returns a 2D vector representing a board.
     * 
     * @return A 2D vector of integers representing the game board is being returned.
     */
    vector<vector<int>> getBoard()
    {
        return board;
    }

    /**
     * This function returns a shared pointer to the parent node.
     * 
     * @return A `shared_ptr` to a `Node` object is being returned.
     */
    shared_ptr<Node> getParent()
    {
        return parent;
    }

    /**
     * The function `getMoves` returns the number of moves.
     * 
     * @return The function `getMoves()` returns the value of the variable `moves`.
     */
    int getMoves()
    {
        return moves;
    }

    /**
     * The function `getEmptyCell` returns a pair of integers representing the coordinates of an empty
     * cell.
     * 
     * @return A pair of integers representing the coordinates of an empty cell.
     */
    pair<int, int> getEmptyCell()
    {
        return emptyCell;
    }

    /**
     * The getCurrentNode function returns a copy of the current Node object.
     * 
     * @return The `getCurrentNode` function is attempting to return a `Node` object by dereferencing
     * `this`. However, `this` is a pointer to the current object, so dereferencing it with `*this`
     * will return the object itself, not a pointer to it. This code will likely result in a
     * compilation error.
     */
    Node getCurrentNode()
    {
        return *this;
    }

    /**
     * The isValidCell function checks if the given cell coordinates are within the bounds of the
     * board.
     * 
     * @param i The parameter `i` represents the row index of a cell in a 2D board.
     * @param j The parameter `j` represents the column index of a cell in a 2D board.
     * 
     * @return The function `isValidCell` returns a boolean value indicating whether the cell at
     * coordinates (i, j) is within the bounds of the board.
     */
    bool isValidCell(int i, int j)
    {
        return (i >= 0 && i < board.size() && j >= 0 && j < board.size());
    }

    /**
     * The function `isEqual` compares the board of a given `Node` object with the board of the current
     * object and returns true if they are equal, false otherwise.
     * 
     * @param node The `isEqual` function you provided is a method that compares the `board` of the
     * current `Node` object with the `board` of another `Node` object passed as a parameter to
     * determine if they are equal.
     * 
     * @return The `isEqual` function is returning a boolean value. It returns `true` if the current
     * `Node` object's `board` is equal to the `board` of the `Node` object passed as a parameter, and
     * `false` otherwise.
     */
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

    /**
     * The function `isGoalState` checks if the current state of a board is the goal state by comparing
     * the values in the board to a sequence of numbers.
     * 
     * @return The function `isGoalState()` returns a boolean value. It returns `true` if the current
     * state of the board represents the goal state, and `false` otherwise.
     */
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

    /**
     * The function `getNeighboringBoardConfigurations` generates neighboring board configurations by
     * moving the empty cell in four directions.
     * 
     * @return The function `getNeighboringBoardConfigurations` returns a vector of `Node` objects
     * representing neighboring board configurations.
     */
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

    /**
     * The function `getBoardString` generates a string representation of a 2D board by concatenating
     * the values of each cell.
     * 
     * @return The `getBoardString` function returns a string representation of the `board` data
     * structure. It concatenates the elements of the `board` into a single string and returns it.
     */
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

    /**
     * The function `printBoard` prints the contents of a 2D board to the console.
     */
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

    /**
     * The function printPath recursively prints the path from the current node to the root node by
     * calling printBoard() at each step.
     */
    void printPath()
    {
        if (parent != nullptr)
        {
            parent->printPath();
        }
        printBoard();
    }
};

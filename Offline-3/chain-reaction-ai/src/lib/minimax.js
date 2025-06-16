import { applyMoveWithFrames, checkWinner } from './gameLogic';
import { ALL_HEURISTICS } from './heuristics';

/**
 * getRandomMove(board, color)
 * - board: NxM array of { count, color }.
 * - color: 'R' or 'B' (AI’s color).
 * Returns: { row, col } or null if no valid moves.
 * This function returns a random valid move for the given color.
 * If no valid moves are available, it returns null.
 */

export function getRandomMove(board, color) {
    const moves = [];
    const numRows = board.length;
    const numCols = board[0].length;
    for (let r = 0; r < numRows; r++) {
        for (let c = 0; c < numCols; c++) {
            const cell = board[r][c];
            if (cell.count === 0 || cell.color === color) {
                moves.push({ row: r, col: c });
            }
        }
    }
    if (moves.length === 0) return null;
    return moves[Math.floor(Math.random() * moves.length)];
}


/**
 * findBestMove(board, myColor, depthLimit, heuristicIndex = 0)
 *  - board: NxM array of { count, color }.
 *  - myColor: 'R' or 'B' (AI’s color).
 *  - depthLimit: maximum depth for the search.
 *  - heuristicIndex: which heuristic from ALL_HEURISTICS to use.
 *
 * Returns: { move: { row, col }, value: numeric }
 */

export function findBestMove(board, myColor, depthLimit, heuristicIndex) {
    const opponent = myColor === 'R' ? 'B' : 'R';
    const heuristic = ALL_HEURISTICS[heuristicIndex];

    // Generate legal moves
    function generateMoves(stateBoard, color, maxMoves = Infinity) {
        const moves = [];
        const numRows = stateBoard.length;
        const numCols = stateBoard[0].length;
        for (let r = 0; r < numRows; r++) {
            for (let c = 0; c < numCols; c++) {
                const cell = stateBoard[r][c];
                if (cell.count === 0 || cell.color === color) {
                    moves.push({ row: r, col: c });
                }
            }
        }
        moves.sort((a, b) => {
            const ca = stateBoard[a.row][a.col].count;
            const cb = stateBoard[b.row][b.col].count;
            return cb - ca;
        });
        return moves.slice(0, maxMoves);
    }

    // Minimax with alpha-beta pruning
    function alphaBeta(stateBoard, currentColor, depth, alpha, beta) {
        const winner = checkWinner(stateBoard);
        if (winner === myColor) return { value: +Infinity };
        if (winner === opponent) return { value: -Infinity };

        if (depth === 0) {
            return { value: heuristic(stateBoard, myColor) };
        }

        const moves = generateMoves(stateBoard, currentColor, 10);
        if (moves.length === 0) {
            return { value: heuristic(stateBoard, myColor) };
        }

        let bestValue, bestMove = null;

        if (currentColor === myColor) {
            bestValue = -Infinity;
            for (const mv of moves) {
                const frames = applyMoveWithFrames(stateBoard, { row: mv.row, col: mv.col, color: currentColor });
                const childBoard = frames.at(-1);
                const { value: v } = alphaBeta(childBoard, opponent, depth - 1, alpha, beta);
                if (v > bestValue) {
                    bestValue = v;
                    bestMove = mv;
                }
                alpha = Math.max(alpha, bestValue);
                if (beta <= alpha) break;
            }
            return { value: bestValue, move: bestMove };
        } else {
            bestValue = +Infinity;
            for (const mv of moves) {
                const frames = applyMoveWithFrames(stateBoard, { row: mv.row, col: mv.col, color: currentColor });
                const childBoard = frames.at(-1);
                const { value: v } = alphaBeta(childBoard, myColor, depth - 1, alpha, beta);
                if (v < bestValue) {
                    bestValue = v;
                    bestMove = mv;
                }
                beta = Math.min(beta, bestValue);
                if (beta <= alpha) break;
            }
            return { value: bestValue, move: bestMove };
        }
    }

    // Start recursion
    const rootResult = alphaBeta(board, myColor, depthLimit, -Infinity, +Infinity);
    if (!rootResult.move) {
        // If minimax found no move, pick a random legal move
        const randomMove = getRandomMove(board, myColor);
        return { move: randomMove, value: 0 };
    }
    return { move: rootResult.move, value: rootResult.value };
}

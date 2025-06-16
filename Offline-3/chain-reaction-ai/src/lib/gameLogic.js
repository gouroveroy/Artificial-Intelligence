import fs from 'fs';
import path from 'path';
import lockfile from 'proper-lockfile';

const GAME_STATE_PATH = path.join(process.cwd(), 'gamestate.txt');

/**
 * Reads gamestate.txt and returns an object:
 *   { header: 'Human Move:' | 'AI Move:', board: Array[rows][cols], rows, cols }
 * Now the first line after header is: <rows> <cols>
 */
export async function readGameState() {
    let release;
    try {
        release = await lockfile.lock(GAME_STATE_PATH, { retries: 5, realpath: false });
        const raw = fs.readFileSync(GAME_STATE_PATH, 'utf-8').trimEnd();
        const lines = raw.split('\n');
        const header = lines[0].trim();
        const boardLines = lines.slice(1);
        const rows = boardLines.length;
        const cols = boardLines[0]?.trim().split(/\s+/).length || 0;
        const board = boardLines.map((row) => {
            const cells = row.trim().split(/\s+/);
            return cells.map((cell) => {
                if (cell === '0') {
                    return { count: 0, color: null };
                } else {
                    const count = parseInt(cell.slice(0, -1), 10);
                    const color = cell.slice(-1);
                    return { count, color };
                }
            });
        });
        return { header, board, rows, cols };
    } finally {
        if (release) await release();
    }
}

/**
 * Writes to gamestate.txt with the exact format:
 *  <header>\n
 *  <rows> <cols>\n
 *  <row1>\n
 *  <row2>\n
 *  … up to rowN\n
 *
 * @param {'Human Move:'|'AI Move:'} header
 * @param {Array} board  (any size)
 */
export async function writeGameState(header, board) {
    let release;
    try {
        release = await lockfile.lock(GAME_STATE_PATH, { retries: 5, realpath: false });
        const rows = board.length;
        const cols = board[0].length;
        const lines = [];
        lines.push(header);
        for (let r = 0; r < rows; r++) {
            const rowCells = [];
            for (let c = 0; c < cols; c++) {
                const cell = board[r][c];
                if (cell.count === 0) {
                    rowCells.push('0');
                } else {
                    rowCells.push(`${cell.count}${cell.color}`);
                }
            }
            lines.push(rowCells.join(' '));
        }
        const out = lines.join('\n') + '\n';
        fs.writeFileSync(GAME_STATE_PATH, out, 'utf-8');
    } finally {
        if (release) await release();
    }
}

/**
 * Deep‐clone a board array so we can modify without mutating original.
 */
export function cloneBoard(board) {
    return board.map((row) =>
        row.map((cell) => ({ count: cell.count, color: cell.color }))
    );
}

/**
 * Given a board and a move ({ row, col, color }), apply the human (or AI) move:
 *  1. Place one orb of `color` at (row,col). It must be either empty or already same color.
 *  2. Repeatedly trigger explosions (chain reactions) until stable.
 *
 * Returns the new board array after stabilization.
 */
export function applyMoveWithFrames(board, move) {
    const { row, col, color } = move;
    const frames = [];
    const newBoard = cloneBoard(board);
    const numRows = board.length;
    const numCols = board[0].length;
    const inBounds = (r, c) => r >= 0 && r < numRows && c >= 0 && c < numCols;

    function criticalMass(r, c) {
        let count = 0;
        if (r > 0) count++;
        if (r < numRows - 1) count++;
        if (c > 0) count++;
        if (c < numCols - 1) count++;
        return count;
    }

    // Place the initial orb
    newBoard[row][col].count += 1;
    newBoard[row][col].color = color;
    frames.push(cloneBoard(newBoard));

    let stepCount = 0;
    let maxIterations = numRows * numCols * 4;
    let unstable = true;
    while (unstable && stepCount < maxIterations) {
        stepCount++;
        unstable = false;
        // Find all cells that need to explode
        const toExplode = [];
        for (let r = 0; r < numRows; r++) {
            for (let c = 0; c < numCols; c++) {
                const cell = newBoard[r][c];
                if (cell.count >= criticalMass(r, c) && cell.count > 0) {
                    toExplode.push({ r, c, color: cell.color });
                }
            }
        }
        if (toExplode.length === 0) break;
        unstable = true;
        for (const { r, c, color } of toExplode) {
            const cm = criticalMass(r, c);
            newBoard[r][c].count -= cm;
            if (newBoard[r][c].count === 0) {
                newBoard[r][c].color = null;
            }
            const dirs = [
                [1, 0], [-1, 0], [0, 1], [0, -1],
            ];
            for (const [dr, dc] of dirs) {
                const nr = r + dr;
                const nc = c + dc;
                if (!inBounds(nr, nc)) continue;
                const neighbor = newBoard[nr][nc];
                if (neighbor.color === null) {
                    neighbor.color = color;
                    neighbor.count = 1;
                } else if (neighbor.color === color) {
                    neighbor.count += 1;
                } else {
                    neighbor.color = color;
                    neighbor.count += 1;
                }
            }
        }
        frames.push(cloneBoard(newBoard));
    }
    return frames;
}

/**
 * Check if a player (color = 'R' or 'B') has won: i.e., the opponent has 0 orbs anywhere.
 * Returns: 'R' if Red won, 'B' if Blue won, or null if no winner yet.
 */
export function checkWinner(board) {
    const numRows = board.length;
    const numCols = board[0].length;
    let emptyCount = 0;
    let colorCounts = { R: 0, B: 0 };

    for (const row of board) {
        for (const cell of row) {
            if (cell.count === 0) {
                emptyCount++;
            } else if (cell.color === 'R') {
                colorCounts.R++;
            } else if (cell.color === 'B') {
                colorCounts.B++;
            }
        }
    }

    if (emptyCount === numRows * numCols || emptyCount === numRows * numCols - 1) {
        return null;
    }

    if (colorCounts.R === 0 && colorCounts.B > 0) return 'B';
    if (colorCounts.B === 0 && colorCounts.R > 0) return 'R';
    return null;
}

/**
 * Creates an empty board with the specified number of rows and columns.
 * Each cell is initialized to { count: 0, color: null }.
 * Also writes the initial state to gamestate.txt.
 * @param {number} rows - Number of rows in the board.
 * @param {number} cols - Number of columns in the board.
 * @return {Array} - A 2D array representing the empty board.
 */
export async function createEmptyBoard(rows, cols) {
    let release;
    try {
        release = await lockfile.lock(GAME_STATE_PATH, { retries: 5, realpath: false });
        fs.writeFileSync(
            GAME_STATE_PATH,
            [
                'AI Move:',
                `${rows} ${cols}`,
                ...Array.from({ length: rows }, () =>
                    Array.from({ length: cols }, () => '0').join(' ')
                ),
            ].join('\n') + '\n',
            'utf-8'
        );
    } finally {
        if (release) await release();
    }
    return Array.from({ length: rows }, () =>
        Array.from({ length: cols }, () => ({
            count: 0,
            color: null,
        }))
    );
}

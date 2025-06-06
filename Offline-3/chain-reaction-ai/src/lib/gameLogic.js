import fs from 'fs';
import path from 'path';
import lockfile from 'proper-lockfile';

const GAME_STATE_PATH = path.join(process.cwd(), 'gamestate.txt');

/**
 * Reads gamestate.txt and returns an object:
 *   { header: 'Human Move:' | 'AI Move:', board: Array[9][6] }
 * Each cell in board is either:
 *   { count: 0, color: null }    for an empty cell
 *   { count: n, color: 'R' }     for Red
 *   { count: n, color: 'B' }     for Blue
 */
export async function readGameState() {
    let release;
    try {
        release = await lockfile.lock(GAME_STATE_PATH, { retries: 5, realpath: false });
        const raw = fs.readFileSync(GAME_STATE_PATH, 'utf-8').trimEnd();
        const lines = raw.split('\n');
        const header = lines[0].trim();
        const boardLines = lines.slice(1, 10);
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
        return { header, board };
    } finally {
        if (release) await release();
    }
}

/**
 * Writes to gamestate.txt with the exact format:
 *  <header>\n
 *  <row1>\n
 *  <row2>\n
 *  … up to row9\n
 *
 * @param {'Human Move:'|'AI Move:'} header
 * @param {Array[9][6]} board  (same format used in readGameState())
 */
export async function writeGameState(header, board) {
    let release;
    try {
        release = await lockfile.lock(GAME_STATE_PATH, { retries: 5, realpath: false });
        // Build string lines:
        const lines = [];
        lines.push(header);
        for (let r = 0; r < 9; r++) {
            const rowCells = [];
            for (let c = 0; c < 6; c++) {
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
 * Returns the new board array (9×6) after stabilization.
 */

export function applyMoveWithFrames(board, move) {
    const { row, col, color } = move;
    const frames = [];

    const newBoard = cloneBoard(board);
    const numRows = 9;
    const numCols = 6;
    const inBounds = (r, c) => r >= 0 && r < numRows && c >= 0 && c < numCols;

    function criticalMass(r, c) {
        let count = 0;
        if (r > 0) count++;
        if (r < numRows - 1) count++;
        if (c > 0) count++;
        if (c < numCols - 1) count++;
        return count;
    }

    const queue = [{ r: row, c: col }];
    const visited = new Set();

    // Place the initial orb
    newBoard[row][col].count += 1;
    newBoard[row][col].color = color;
    frames.push(cloneBoard(newBoard));

    let stepCount = 0;
    let maxIterations = numRows * numCols * 4;
    while (queue.length > 0 && stepCount < maxIterations) {
        stepCount++;
        const { r, c } = queue.shift();
        let key = `${r},${c}`;
        if (visited.has(key)) continue;
        visited.add(key);

        const cell = newBoard[r][c];

        const cm = criticalMass(r, c);
        if (cell.count < cm) continue;

        cell.count -= cm;
        if (cell.count === 0) {
            cell.color = null;
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

            if (neighbor.count >= criticalMass(nr, nc)) {
                const neighborKey = `${nr},${nc}`;
                if (!visited.has(neighborKey)) {
                    queue.push({ r: nr, c: nc });
                }
            }
        }

        frames.push(cloneBoard(newBoard)); // Add frame after each explosion step
    }

    return frames;
}

/**
 * Check if a player (color = 'R' or 'B') has won: i.e., the opponent has 0 orbs anywhere.
 * Returns: 'R' if Red won, 'B' if Blue won, or null if no winner yet.
 */
export function checkWinner(board) {
    // let hasR = false;
    // let hasB = false;
    // for (const row of board) {
    //     for (const cell of row) {
    //         if (cell.count > 0) {
    //             if (cell.color === 'R') hasR = true;
    //             if (cell.color === 'B') hasB = true;
    //         }
    //         if (hasR && hasB) return null;
    //     }
    // }
    // if (hasR && !hasB) return 'R';
    // if (hasB && !hasR) return 'B';
    // return null;
    return 'G';
}

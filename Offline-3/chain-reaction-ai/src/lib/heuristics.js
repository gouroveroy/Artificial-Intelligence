/**
 * We assume:
 *  - board is a 9×6 array like in gameLogic (each cell = { count, color }).
 *  - myColor is 'R' or 'B' (the player to evaluate for).
 *  - The opponent is the other color.
 *
 * Each heuristic function returns a higher score when the position is better for myColor.
 */

// 1. Simple Orb‐Count Difference:
//    score = (total orbs of myColor) - (total orbs of opponent).
export function heuristicOrbCount(board, myColor) {
    let mySum = 0;
    let oppSum = 0;
    const oppColor = myColor === 'R' ? 'B' : 'R';
    for (const row of board) {
        for (const cell of row) {
            if (cell.count > 0) {
                if (cell.color === myColor) mySum += cell.count;
                if (cell.color === oppColor) oppSum += cell.count;
            }
        }
    }
    return mySum - oppSum;
}

// 2. Weighted by Critical Mass Proximity:
//    For each cell of myColor, add + (count / criticalMass) * 10.
//    For each cell of opponent, subtract same.
//    This rewards cells closer to exploding for you, penalizes opponent’s near‐explosions.
export function heuristicMassProximity(board, myColor) {
    const oppColor = myColor === 'R' ? 'B' : 'R';
    let score = 0;
    const numRows = board.length;
    const numCols = board[0].length;
    const inBounds = (r, c) => r >= 0 && r < numRows && c >= 0 && c < numCols;
    function criticalMass(r, c) {
        let n = 0;
        if (r > 0) n++;
        if (r < numRows - 1) n++;
        if (c > 0) n++;
        if (c < numCols - 1) n++;
        return n;
    }

    for (let r = 0; r < numRows; r++) {
        for (let c = 0; c < numCols; c++) {
            const cell = board[r][c];
            if (cell.count > 0) {
                const cm = criticalMass(r, c);
                const proximity = cell.count / cm; // between 0 and ≥1
                if (cell.color === myColor) {
                    score += proximity * 10;
                } else {
                    score -= proximity * 10;
                }
            }
        }
    }
    return score;
}

// 3. Chain‐Reaction Potential:
//    Count how many adjacent opponent cells a single orb placement by me would convert.
//    Essentially: for each cell of myColor that is one orb away from exploding,
//    count how many neighboring opponent cells exist. Sum over all such cells.
//    We subtract the same for opponent. This estimates immediate takeover potential.
export function heuristicChainPotential(board, myColor) {
    const oppColor = myColor === 'R' ? 'B' : 'R';
    let score = 0;
    const numRows = board.length;
    const numCols = board[0].length;
    const inBounds = (r, c) => r >= 0 && r < numRows && c >= 0 && c < numCols;
    function criticalMass(r, c) {
        let n = 0;
        if (r > 0) n++;
        if (r < numRows - 1) n++;
        if (c > 0) n++;
        if (c < numCols - 1) n++;
        return n;
    }

    for (let r = 0; r < numRows; r++) {
        for (let c = 0; c < numCols; c++) {
            const cell = board[r][c];
            if (cell.count > 0) {
                const cm = criticalMass(r, c);
                // If this cell is one away from exploding:
                if (cell.count === cm - 1) {
                    // Count how many neighbors are opponent color:
                    let neighborsOpp = 0;
                    const dirs = [
                        [1, 0],
                        [-1, 0],
                        [0, 1],
                        [0, -1],
                    ];
                    dirs.forEach(([dr, dc]) => {
                        const nr = r + dr;
                        const nc = c + dc;
                        if (inBounds(nr, nc)) {
                            const nb = board[nr][nc];
                            if (nb.color === oppColor) neighborsOpp++;
                        }
                    });
                    if (cell.color === myColor) {
                        score += neighborsOpp * 5; // each potential conversion is +5
                    } else {
                        score -= neighborsOpp * 5;
                    }
                }
            }
        }
    }
    return score;
}

// 4. Board Control (Corners & Edges):
//    Reward occupying corners and edges for myColor, penalize for opponent.
//    Corners are more “stable” (only 2 neighbors), edges next best (3 neighbors),
//    inner cells (4 neighbors) are least stable. We can weigh: corner=3, edge=2, inner=1.
export function heuristicControlCornersEdges(board, myColor) {
    const oppColor = myColor === 'R' ? 'B' : 'R';
    let score = 0;
    const numRows = board.length;
    const numCols = board[0].length;

    for (let r = 0; r < numRows; r++) {
        for (let c = 0; c < numCols; c++) {
            const cell = board[r][c];
            if (cell.count > 0) {
                // Determine if corner / edge / inner:
                let weight = 1;
                const isCorner =
                    (r === 0 && c === 0) ||
                    (r === 0 && c === numCols - 1) ||
                    (r === numRows - 1 && c === 0) ||
                    (r === numRows - 1 && c === numCols - 1);
                const isEdge =
                    r === 0 || r === numRows - 1 || c === 0 || c === numCols - 1;
                if (isCorner) {
                    weight = 3;
                } else if (isEdge) {
                    weight = 2;
                }
                if (cell.color === myColor) {
                    score += weight * cell.count;
                } else {
                    score -= weight * cell.count;
                }
            }
        }
    }
    return score;
}

// 5. Mobility / Legal Move Count:
//    Count how many legal moves myColor has vs opponent. More mobility = advantage.
//    If myColor has M legal moves and opp has N, score = (M - N) * 10.
export function heuristicMobility(board, myColor) {
    const oppColor = myColor === 'R' ? 'B' : 'R';
    let myLegal = 0;
    let oppLegal = 0;
    const numRows = board.length;
    const numCols = board[0].length;
    for (let r = 0; r < numRows; r++) {
        for (let c = 0; c < numCols; c++) {
            const cell = board[r][c];
            if (cell.count === 0 || cell.color === myColor) {
                myLegal++;
            }
            if (cell.count === 0 || cell.color === oppColor) {
                oppLegal++;
            }
        }
    }
    return (myLegal - oppLegal) * 10;
}

// You can bundle them in an array for easy selection:
export const ALL_HEURISTICS = [
    heuristicOrbCount,
    heuristicMassProximity,
    heuristicChainPotential,
    heuristicControlCornersEdges,
    heuristicMobility,
];

// File: src/pages/api/ai-vs-ai-move.js
import { applyMoveWithFrames, checkWinner } from '../../lib/gameLogic';
import { findBestMove, getRandomMove } from '../../lib/minimax';

export default async function handler(req, res) {
    if (req.method !== 'POST') {
        return res.status(405).json({ error: 'Only POST allowed' });
    }

    const { board, agent, color, depth = 3, heuristic = 0, aiTimeLimit = 2 } = req.body || {};

    if (!Array.isArray(board) || (color !== 'R' && color !== 'B')) {
        return res.status(400).json({ error: 'Invalid board or color' });
    }

    let move = null;
    let timeTaken = 0;
    if (agent === 'random') {
        move = getRandomMove(board, color);
    } else if (agent === 'minimax') {
        const startTime = performance.now();
        move = findBestMove(board, color, depth, heuristic)?.move;
        timeTaken = performance.now() - startTime;
    } else {
        return res.status(400).json({ error: 'Invalid agent type' });
    }

    if (!move) {
        return res.status(200).json({ move: null, frames: [], winner: checkWinner(board) });
    }

    const frames = applyMoveWithFrames(board, { ...move, color });
    const updatedBoard = frames.at(-1);
    const winner = checkWinner(updatedBoard);

    if (timeTaken > aiTimeLimit * 1000) {
        return res.status(400).json({ error: `AI took too long: ${timeTaken.toFixed(2)}ms` });
    }

    return res.status(200).json({ move, frames, winner });
}

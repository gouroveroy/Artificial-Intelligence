// File: src/pages/api/compute-ai-move.js
import {
    readGameState,
    applyMoveWithFrames,
    checkWinner,
    writeGameState,
} from '../../lib/gameLogic';

import { findBestMove, getRandomMove } from '../../lib/minimax';

export default async function handler(req, res) {
    if (req.method !== 'POST') {
        return res.status(405).json({ error: 'Only POST allowed' });
    }

    const { humanMove } = req.body;
    if (!humanMove || typeof humanMove.row !== 'number' || typeof humanMove.col !== 'number') {
        return res.status(400).json({ error: 'Invalid humanMove payload' });
    }

    let { header, board } = await readGameState();
    if (header !== 'AI Move:') {
        return res.status(400).json({ error: 'Expected header "Human Move:" but got ' + header });
    }

    const { row: hr, col: hc } = humanMove;
    if (hr < 0 || hr > 8 || hc < 0 || hc > 5) {
        return res.status(400).json({ error: 'Move out of bounds' });
    }
    const targetCell = board[hr][hc];
    if (targetCell.count > 0 && targetCell.color !== 'R') {
        return res.status(400).json({ error: 'You can only place on empty or your own color (R)' });
    }

    // 3. Apply human move (returns intermediate animation frames)
    const humanFrames = applyMoveWithFrames(board, { row: hr, col: hc, color: 'R' });
    const humanFinalBoard = humanFrames.at(-1);

    await writeGameState('Human Move:', humanFinalBoard);
    // await new Promise((r) => setTimeout(r, 3500));

    const humanWon = checkWinner(humanFinalBoard) === 'R';
    if (humanWon) {
        await writeGameState('AI Move:', humanFinalBoard);
        return res.status(200).json({ aiMove: null, humanFrames, aiFrames: [], aiWon: false });
    }

    const depthLimit = 2;
    const aiResult = findBestMove(humanFinalBoard, 'B', depthLimit);

    if (!aiResult || !aiResult.move) {
        await writeGameState('AI Move:', humanFinalBoard);
        // If no best move found, return a random valid move for AI
        return res.status(200).json({
            aiMove: getRandomMove(humanFinalBoard, 'B'),
            humanFrames,
            aiFrames: [],
            aiWon: false
        });
    }

    const { row: ar, col: ac } = aiResult.move;
    const aiFrames = applyMoveWithFrames(humanFinalBoard, { row: ar, col: ac, color: 'B' });
    const aiFinalBoard = aiFrames.at(-1);

    const aiWon = checkWinner(aiFinalBoard) === 'B';
    await writeGameState('AI Move:', aiFinalBoard);

    return res.status(200).json({
        aiMove: { row: ar, col: ac },
        humanFrames,
        aiFrames,
        aiWon,
    });
}

// File: src/pages/api/write-state.js
import { writeGameState } from '../../lib/gameLogic';

export default async function handler(req, res) {
    if (req.method !== 'POST') {
        return res.status(405).json({ error: 'Only POST allowed' });
    }

    const { header, board } = req.body;
    if (
        (header !== 'Human Move:' && header !== 'AI Move:') ||
        !Array.isArray(board) ||
        board.length !== 9 ||
        !board.every((row) => Array.isArray(row) && row.length === 6)
    ) {
        return res.status(400).json({ error: 'Invalid payload' });
    }

    try {
        await writeGameState(header, board);
        return res.status(200).json({ success: true });
    } catch (err) {
        console.error('Error writing game state:', err);
        return res.status(500).json({ error: 'Failed to write game state' });
    }
}

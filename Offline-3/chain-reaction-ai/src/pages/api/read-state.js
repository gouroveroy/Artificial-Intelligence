// File: src/pages/api/read-state.js
import { readGameState } from '../../lib/gameLogic';

export default async function handler(req, res) {
    if (req.method !== 'GET') {
        return res.status(405).json({ error: 'Only GET allowed' });
    }
    try {
        const { header, board, rows, cols } = await readGameState();
        return res.status(200).json({ header, board, rows, cols });
    } catch (err) {
        console.error('Error reading game state:', err);
        return res.status(500).json({ error: 'Failed to read game state' });
    }
}

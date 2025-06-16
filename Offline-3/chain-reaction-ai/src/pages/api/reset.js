import fs from 'fs';
import path from 'path';

export default function handler(req, res) {
    if (req.method !== 'POST') {
        return res.status(405).json({ error: 'Method not allowed' });
    }
    let { rows = 9, cols = 6 } = req.body || {};
    rows = Number(rows);
    cols = Number(cols);
    const initialState =
        'AI Move:\n' +
        Array(rows)
            .fill(Array(cols).fill('0').join(' '))
            .join('\n');
    const filePath = path.join(process.cwd(), 'gamestate.txt');
    fs.writeFileSync(filePath, initialState, 'utf8');
    res.status(200).json({ ok: true });
}

import fs from 'fs';
import path from 'path';

export default function handler(req, res) {
    if (req.method !== 'POST') {
        return res.status(405).json({ error: 'Method not allowed' });
    }
    const initialState =
        'AI Move:\n' +
        Array(9)
            .fill('0 0 0 0 0 0')
            .join('\n');
    const filePath = path.join(process.cwd(), 'gamestate.txt');
    fs.writeFileSync(filePath, initialState, 'utf8');
    res.status(200).json({ ok: true });
}

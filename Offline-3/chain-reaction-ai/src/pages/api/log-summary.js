import { logGameSummaryAi, logGameSummaryHuman } from '../../lib/gameLog';

export default function handler(req, res) {
    if (req.method === 'POST') {
        const { mode, ...params } = req.body;
        try {
            if (mode === 'ai-vs-ai') {
                logGameSummaryAi(params);
            } else if (mode === 'human-vs-ai') {
                logGameSummaryHuman(params);
            }
            res.status(200).json({ success: true });
        } catch (e) {
            res.status(500).json({ error: e.message });
        }
    } else {
        res.status(405).end();
    }
}

import fs from 'fs';
import path from 'path';

const LOG_PATH = path.join(process.cwd(), 'game_log.txt');

/**
 * Logs a summary for each finished game.
 * @param {Object} params
 * @param {'Red'|'Blue'} winner
 * @param {Object} redAgent - { type: 'human' | 'ai', aiType?: 'random'|'minimax', depth?: number, heuristic?: string, time?: number }
 * @param {Object} blueAgent - same as redAgent
 * @param {number} totalTime - total time taken to end the game (in seconds)
 */
export function logGameSummaryAi({ winner, redAgent, blueAgent, totalTime }) {
    let gameNumber = 1;
    if (fs.existsSync(LOG_PATH)) {
        const content = fs.readFileSync(LOG_PATH, 'utf8');
        const matches = content.match(/Game #(\d+)/g);
        if (matches && matches.length > 0) {
            const last = matches[matches.length - 1];
            const lastNum = parseInt(last.replace('Game #', ''), 10);
            if (!isNaN(lastNum)) gameNumber = lastNum + 1;
        }
    } else {
        return;
    }

    const lines = [];
    lines.push(`Game #${gameNumber}`);
    lines.push(`Winner: ${winner}`);
    lines.push(
        `Red agent: ${redAgent.aiType === 'random'
            ? 'random'
            : `minimax, depth=${redAgent.depth}, heuristic=${redAgent.heuristic}, avg_time=${redAgent.time}s`
        }`
    );
    lines.push(
        `Blue agent: ${blueAgent.aiType === 'random'
            ? 'random'
            : `minimax, depth=${blueAgent.depth}, heuristic=${blueAgent.heuristic}, avg_time=${blueAgent.time}s`
        }`
    );
    lines.push(`Total game time: ${totalTime}s`);
    lines.push('---\n');
    fs.appendFileSync(LOG_PATH, lines.join('\n'), 'utf8');
}

export function logGameSummaryHuman({ winner, blueAgent }) {
    let gameNumber = 1;
    if (fs.existsSync(LOG_PATH)) {
        const content = fs.readFileSync(LOG_PATH, 'utf8');
        const matches = content.match(/Game #(\d+)/g);
        if (matches && matches.length > 0) {
            const last = matches[matches.length - 1];
            const lastNum = parseInt(last.replace('Game #', ''), 10);
            if (!isNaN(lastNum)) gameNumber = lastNum + 1;
        }
    } else {
        return;
    }

    const lines = [];
    lines.push(`Game #${gameNumber}`);
    lines.push(`Winner: ${winner}`);
    lines.push(
        `Red agent: Human`
    );
    lines.push(
        `Blue agent: minimax, depth=${blueAgent.depth}, heuristic=${blueAgent.heuristic}`
    );
    lines.push('---\n');
    fs.appendFileSync(LOG_PATH, lines.join('\n'), 'utf8');
}

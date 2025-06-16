# Chain Reaction AI

A web-based implementation of the classic **Chain Reaction** strategy game, featuring a smart AI opponent and an AI-vs-AI mode. Built with [Next.js](https://nextjs.org), React, and a custom minimax engine.

## Features

- **Human vs AI:** Play as Red against a challenging Blue AI.
- **AI vs AI:** Watch two AI agents battle, with customizable depth and heuristics.
- **Animated Board:** Explosions and chain reactions are visualized step-by-step.
- **Persistent State:** Game state is saved between sessions.
- **Multiple Heuristics:** AI can use different strategies for evaluation.
- **Reset & Resume:** Start a new game or continue your previous one.

## Getting Started

1. **Install dependencies:**
   ```bash
   npm install
   ```

2. **Run the development server:**
   ```bash
   npm run dev
   ```

3. **Open your browser:**
   Visit [http://localhost:3000](http://localhost:3000) to play.

## How to Play

- **Goal:** Eliminate all of your opponent's orbs from the board.
- **Turns:** Players take turns placing an orb in an empty cell or one they already own.
- **Explosions:**
  - Corners explode at 2 orbs
  - Edges at 3 orbs
  - Center cells at 4 orbs
  When a cell explodes, it sends orbs to its neighbors, potentially causing chain reactions and capturing opponent cells.
- **Win:** The last player with orbs remaining wins.

## Project Structure

- `src/pages/` — Next.js pages (game, AI-vs-AI, about, API routes)
- `src/components/` — Board and Cell React components
- `src/lib/` — Game logic, minimax AI, and heuristics
- `gamestate.txt` — File-based persistent game state

## Tech Stack

- **Frontend & Backend:** Next.js (React)
- **AI:** Custom minimax with alpha-beta pruning and multiple heuristics ([src/lib/minimax.js](src/lib/minimax.js), [src/lib/heuristics.js](src/lib/heuristics.js))
- **Animations:** framer-motion
- **State Management:** File-based with locking for safe concurrent access

## Credits

- Developed by **Gourove Roy**
- Inspired by the classic Chain Reaction game

## License

This project is for educational and personal use.

## Deploy on Vercel
You can try the live app here: [https://chain-reaction-game-xi.vercel.app/](https://chain-reaction-game-xi.vercel.app/)

---

Enjoy the game and try to beat the AI!


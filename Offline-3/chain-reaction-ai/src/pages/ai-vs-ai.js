// File: src/pages/ai-vs-ai.js
import { useState, useRef, useEffect } from 'react';
import { useRouter } from 'next/router';
import Board from '../components/Board';
import Link from 'next/link';
import confetti from 'canvas-confetti'; // <-- Add this import

export default function AIVsAILive() {
    const router = useRouter();
    const { rows = 9, cols = 6 } = router.query;

    // Dynamic empty board based on query params
    const makeEmptyBoard = () =>
        Array(Number(rows)).fill(null).map(() =>
            Array(Number(cols)).fill(null).map(() => ({ count: 0, color: null }))
        );

    const [board, setBoard] = useState(makeEmptyBoard());
    const [step, setStep] = useState(0);
    const [header, setHeader] = useState(false);
    const [orbCounts, setOrbCounts] = useState({ R: 0, B: 0 });
    const [playing, setPlaying] = useState(false);
    const [winner, setWinner] = useState(null);
    const [agentA, setAgentA] = useState('minimax');
    const [agentB, setAgentB] = useState('random');
    const [depthA, setDepthA] = useState(3);
    const [depthB, setDepthB] = useState(3);
    const [heuristicA, setHeuristicA] = useState(0);
    const [heuristicB, setHeuristicB] = useState(0);
    const [stopped, setStopped] = useState(false);
    const [aiTimeLimit, setAiTimeLimit] = useState(2000);
    const stoppedRef = useRef(false);

    // Pause/Resume state and ref
    const [paused, setPaused] = useState(false);
    const pausedRef = useRef(false);

    // Confirmation for stopping
    const [showStopConfirm, setShowStopConfirm] = useState(false);

    // Timer state
    const [elapsed, setElapsed] = useState(0);
    const timerRef = useRef(null);

    // Re-create board if grid size changes
    useEffect(() => {
        setBoard(makeEmptyBoard());
        setStep(0);
        setHeader(false);
        setOrbCounts({ R: 0, B: 0 });
        setWinner(null);
        setPlaying(false);
        setStopped(false);
        stoppedRef.current = false;
        setPaused(false);
        pausedRef.current = false;
    }, [rows, cols]);

    function updateOrbCounts(board) {
        let red = 0, blue = 0;
        for (const row of board) {
            for (const cell of row) {
                if (cell.color === 'R') red += cell.count;
                else if (cell.color === 'B') blue += cell.count;
            }
        }
        setOrbCounts({ R: red, B: blue });
    }

    async function getAIMove(board, agent, color, depth, heuristic) {
        const resp = await fetch('/api/ai-vs-ai', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ board, agent, color, depth, heuristic, aiTimeLimit }),
        });
        return await resp.json();
    }

    async function animateFrames(frames) {
        for (const frame of frames) {
            setBoard(frame);
            updateOrbCounts(frame);
            await new Promise(res => setTimeout(res, 250));
        }
    }

    async function runLiveGame() {
        setPlaying(true);
        setWinner(null);
        setStep(1);
        setHeader(false);
        setStopped(false);
        stoppedRef.current = false;
        setPaused(false);
        pausedRef.current = false;
        setElapsed(0);
        if (timerRef.current) clearInterval(timerRef.current);
        timerRef.current = setInterval(() => {
            setElapsed(e => e + 1);
        }, 1000);
        let currentBoard = makeEmptyBoard();
        let currentTurn = 'R';
        updateOrbCounts(currentBoard);

        while (true) {
            // Pause logic
            while (pausedRef.current) {
                await new Promise(res => setTimeout(res, 100));
            }
            if (stoppedRef.current) {
                setPlaying(false);
                break;
            }
            const agent = currentTurn === 'R' ? agentA : agentB;
            const depth = currentTurn === 'R' ? depthA : depthB;
            const heuristic = currentTurn === 'R' ? heuristicA : heuristicB;

            const { move, frames, winner: moveWinner } = await getAIMove(currentBoard, agent, currentTurn, depth, heuristic);

            if (!move) {
                setWinner(moveWinner || 'Draw');
                setPlaying(false);
                break;
            }

            await animateFrames(frames);
            currentBoard = frames.at(-1);

            if (moveWinner) {
                setWinner(moveWinner);
                setPlaying(false);
                // Firework/confetti on win
                if (moveWinner === 'R') {
                    confetti({ particleCount: 200, spread: 100, origin: { y: 0.6 }, colors: ['#e53935', '#ffeb3b'] });
                } else if (moveWinner === 'B') {
                    confetti({ particleCount: 200, spread: 100, origin: { y: 0.6 }, colors: ['#1976d2', '#ffeb3b'] });
                } else {
                    confetti({ particleCount: 150, spread: 80, origin: { y: 0.7 }, colors: ['#ffd600', '#fff'] });
                }
                // Log summary to server
                fetch('/api/log-summary', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({
                        mode: 'ai-vs-ai',
                        winner: moveWinner === 'R' ? 'Red' : 'Blue',
                        redAgent: {
                            aiType: agentA,
                            depth: depthA,
                            heuristic: heuristicA,
                            time: (elapsed / (1.5 * step)).toFixed(2)
                        },
                        blueAgent: {
                            aiType: agentB,
                            depth: depthB,
                            heuristic: heuristicB,
                            time: (elapsed / (1.5 * step)).toFixed(2)
                        },
                        totalTime: elapsed
                    })
                });
                break;
            }

            setHeader(h => !h);
            setStep(s => s + 1);
            currentTurn = currentTurn === 'R' ? 'B' : 'R';
        }
        if (timerRef.current) {
            clearInterval(timerRef.current);
            timerRef.current = null;
        }
    }

    function handlePauseResume() {
        setPaused(p => {
            const nextPaused = !p;
            pausedRef.current = nextPaused;
            if (nextPaused) {
                // Pausing: stop timer
                if (timerRef.current) {
                    clearInterval(timerRef.current);
                    timerRef.current = null;
                }
            } else {
                // Resuming: start timer
                if (!timerRef.current) {
                    timerRef.current = setInterval(() => {
                        setElapsed(e => e + 1);
                    }, 1000);
                }
            }
            return nextPaused;
        });
    }

    function handleStop() {
        setShowStopConfirm(true);
    }

    function confirmStop() {
        setStopped(true);
        stoppedRef.current = true;
        setBoard(makeEmptyBoard());
        setStep(0);
        setHeader(false);
        setOrbCounts({ R: 0, B: 0 });
        setPlaying(false);
        setWinner(null);
        setPaused(false);
        pausedRef.current = false;
        setShowStopConfirm(false);
        setElapsed(0);
        if (timerRef.current) {
            clearInterval(timerRef.current);
            timerRef.current = null;
        }
    }

    function cancelStop() {
        setShowStopConfirm(false);
    }

    return (
        <div style={{
            minHeight: '100vh',
            background: 'linear-gradient(135deg, #1a83b7 0%, #09345f 100%)',
            textAlign: 'center',
            paddingTop: '40px',
            display: 'flex',
            flexDirection: 'row',
            justifyContent: 'center',
            alignItems: 'flex-start',
        }}>
            {/* LEFT PANEL: AI Controls */}
            <div style={{
                width: 230,
                minHeight: 600,
                background: 'rgba(255,255,255,0.06)',
                borderRadius: '12px',
                padding: '20px 18px',
                marginRight: 24,
                boxShadow: '0 4px 20px rgba(0,0,0,0.2)',
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                justifyContent: 'flex-start'
            }}>
                <h3 style={{ color: '#ffeb3b', marginBottom: '16px' }}>🧠 AI Controls</h3>
                <div style={{
                    color: '#fff',
                    fontSize: '1rem',
                    marginBottom: '18px',
                    marginTop: 100,
                    width: '100%'
                }}>
                    <b>Agent A (Red)</b><br />
                    <select value={agentA} onChange={e => setAgentA(e.target.value)} style={{ marginTop: 6, borderRadius: 6, padding: 4, width: '90%' }}>
                        <option value="random">Random</option>
                        <option value="minimax">Minimax</option>
                    </select>
                    <br />Depth: <input
                        type="number"
                        min={1}
                        max={4}
                        value={depthA}
                        onChange={e => {
                            let val = Math.max(1, Math.min(4, Number(e.target.value)));
                            setDepthA(val);
                        }}
                        style={{ width: 48, borderRadius: 6, padding: 2, marginTop: 4 }}
                    />
                    <br />Heuristic: <input
                        type="number"
                        min={0}
                        max={7}
                        value={heuristicA}
                        onChange={e => {
                            let val = Math.max(0, Math.min(7, Number(e.target.value)));
                            setHeuristicA(val);
                        }}
                        style={{ width: 48, borderRadius: 6, padding: 2, marginTop: 4 }}
                    />
                </div>
                <div style={{
                    color: '#fff',
                    fontSize: '1rem',
                    marginBottom: '18px',
                    marginTop: 30,
                    width: '100%'
                }}>
                    <b>Agent B (Blue)</b><br />
                    <select value={agentB} onChange={e => setAgentB(e.target.value)} style={{ marginTop: 6, borderRadius: 6, padding: 4, width: '90%' }}>
                        <option value="random">Random</option>
                        <option value="minimax">Minimax</option>
                    </select>
                    <br />Depth: <input
                        type="number"
                        min={1}
                        max={4}
                        value={depthB}
                        onChange={e => {
                            let val = Math.max(1, Math.min(4, Number(e.target.value)));
                            setDepthB(val);
                        }}
                        style={{ width: 48, borderRadius: 6, padding: 2, marginTop: 4 }}
                    />
                    <br />Heuristic: <input
                        type="number"
                        min={0}
                        max={7}
                        value={heuristicB}
                        onChange={e => {
                            let val = Math.max(0, Math.min(7, Number(e.target.value)));
                            setHeuristicB(val);
                        }}
                        style={{ width: 48, borderRadius: 6, padding: 2, marginTop: 4 }}
                    />
                </div>
                <div style={{
                    marginTop: 30,
                    width: '100%',
                    color: '#fff',
                    fontSize: '1rem',
                    background: 'rgba(255,255,255,0.08)',
                    borderRadius: 8,
                    padding: '10px 8px',
                    boxShadow: '0 2px 8px rgba(25, 118, 210, 0.08)'
                }}>
                    <label style={{ fontWeight: 600, marginBottom: 4, display: 'block' }}>
                        ⏱ Max AI Time (s)
                    </label>
                    <input
                        type="number"
                        min={1}
                        max={5}
                        value={aiTimeLimit / 1000}
                        onChange={e => {
                            let val = Math.max(1, Math.min(5, Number(e.target.value)));
                            setAiTimeLimit(val * 1000);
                        }}
                        style={{
                            width: 80,
                            fontSize: '1rem',
                            borderRadius: 6,
                            padding: '4px 8px',
                            border: '1px solid #bbb',
                            marginTop: 4,
                            textAlign: 'center'
                        }}
                    />
                </div>
            </div>

            {/* MAIN BOARD AREA */}
            <div style={{ flex: 1, minWidth: 0 }}>
                <h1 style={{ color: '#ffeb3b', marginBottom: '16px' }}>AI vs AI: Live Chain Reaction</h1>

                {/* Agent Orbs at Top */}
                <div style={{
                    display: 'flex',
                    justifyContent: 'center',
                    gap: '40px',
                    marginBottom: '20px'
                }}>
                    <div style={{
                        padding: '12px 28px',
                        borderRadius: '12px',
                        background: !header ? '#e53935' : '#bdbdbd',
                        color: '#fff',
                        fontWeight: 'bold',
                        fontSize: '1.2rem',
                        boxShadow: !header ? '0 0 16px 4px rgba(229, 57, 53, 0.5)' : 'none',
                        transform: !header ? 'scale(1.07)' : 'scale(1)',
                        transition: 'all 0.2s ease-in-out'
                    }}>
                        🔴 Agent A (Red): {orbCounts.R} Orbs
                    </div>
                    <div style={{
                        padding: '12px 28px',
                        borderRadius: '12px',
                        background: header ? '#1976d2' : '#bdbdbd',
                        color: '#fff',
                        fontWeight: 'bold',
                        fontSize: '1.2rem',
                        boxShadow: header ? '0 0 16px 4px rgba(25, 118, 210, 0.5)' : 'none',
                        transform: header ? 'scale(1.07)' : 'scale(1)',
                        transition: 'all 0.2s ease-in-out'
                    }}>
                        🔵 Agent B (Blue): {orbCounts.B} Orbs
                    </div>
                </div>

                {/* Winner message just below agent orbs */}
                {winner && <h2 style={{
                    color: winner === 'R' ? 'red' : winner === 'B' ? 'blue' : '#ffd600',
                    marginTop: 0,
                    marginBottom: 16
                }}>
                    {winner === 'Draw' ? 'Draw!' : `${winner === 'R' ? 'Agent A (Red)' : 'Agent B (Blue)'} Wins!`}
                </h2>}

                {/* Board */}
                <div style={{
                    display: 'inline-block',
                    background: 'linear-gradient(to bottom right, #e3f2fd, #b3e5fc)',
                    border: '2px solid #90caf9',
                    borderRadius: '10px',
                    boxShadow: '0 6px 20px rgba(0,0,0,0.2) inset, 0 8px 32px rgba(0,0,0,0.3)',
                    padding: '6px',
                    margin: '10px 0',
                    position: 'relative',
                    transition: 'all 0.4s ease-in-out'
                }}>
                    <Board boardData={board} isDisabled={true} />
                </div>
            </div>

            {/* RIGHT PANEL: Buttons & Info */}
            <div style={{
                width: 220,
                minHeight: 600,
                background: 'rgba(255,255,255,0.06)',
                borderRadius: '12px',
                padding: '20px 18px',
                marginLeft: 24,
                boxShadow: '0 4px 20px rgba(0,0,0,0.2)',
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                justifyContent: 'flex-start'
            }}>
                <h3 style={{ color: '#ffeb3b', marginBottom: '16px' }}>🔗 Actions</h3>
                <div style={{
                    marginBottom: 24,
                    marginTop: 100,
                    color: '#fff',
                    fontSize: '1.1rem',
                    background: 'rgba(255,255,255,0.10)',
                    borderRadius: 10,
                    padding: '14px 10px',
                    width: '100%',
                    boxShadow: '0 2px 8px rgba(25, 118, 210, 0.10)'
                }}>
                    <b>Moves:</b> {step}<br />
                    <b>Total Orbs:</b> {orbCounts.R + orbCounts.B}<br />
                    <b>Time:</b> {Math.floor(elapsed / 60)}:{(elapsed % 60).toString().padStart(2, '0')}
                </div>
                <button
                    onClick={runLiveGame}
                    disabled={playing}
                    style={{
                        background: '#43cea2',
                        backgroundImage: 'linear-gradient(90deg, #43cea2 0%, #185a9d 100%)',
                        color: '#fff',
                        padding: '10px 24px',
                        borderRadius: '8px',
                        border: 'none',
                        fontSize: '1.1rem',
                        cursor: 'pointer',
                        width: '100%',
                        boxShadow: '0 2px 10px rgba(67, 206, 162, 0.4)',
                        transition: 'transform 0.2s ease-in-out',
                        marginBottom: 16
                    }}
                    onMouseEnter={e => e.currentTarget.style.transform = 'scale(1.05)'}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'
                    }
                >
                    {playing ? 'Playing...' : '▶ Start Match'}
                </button>
                <button
                    onClick={handlePauseResume}
                    disabled={!playing}
                    style={{
                        background: paused ? '#ffb300' : '#1976d2',
                        color: '#fff',
                        padding: '10px 24px',
                        borderRadius: '8px',
                        border: 'none',
                        fontSize: '1.1rem',
                        cursor: playing ? 'pointer' : 'not-allowed',
                        width: '100%',
                        boxShadow: paused
                            ? '0 2px 10px rgba(255, 179, 0, 0.2)'
                            : '0 2px 10px rgba(25, 118, 210, 0.2)',
                        transition: 'transform 0.2s ease-in-out',
                        marginBottom: 16
                    }}
                    onMouseEnter={e => { if (playing) e.currentTarget.style.transform = 'scale(1.05)'; }}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'
                    }
                >
                    {paused ? '▶ Resume' : '⏸ Pause'}
                </button>
                <button
                    onClick={handleStop}
                    disabled={!playing}
                    style={{
                        background: '#e53935',
                        color: '#fff',
                        padding: '10px 24px',
                        borderRadius: '8px',
                        border: 'none',
                        fontSize: '1.1rem',
                        cursor: playing ? 'pointer' : 'not-allowed',
                        width: '100%',
                        boxShadow: '0 2px 10px rgba(229, 57, 53, 0.2)',
                        transition: 'transform 0.2s ease-in-out',
                        marginBottom: 16
                    }}
                    onMouseEnter={e => { if (playing) e.currentTarget.style.transform = 'scale(1.05)'; }}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'
                    }
                >
                    ⏹ Stop Match
                </button>
                <Link href="/" style={{
                    display: 'inline-block',
                    marginTop: 20,
                    textDecoration: 'none',
                    background: '#fdd835',
                    color: '#000',
                    fontWeight: 600,
                    padding: '10px 24px',
                    borderRadius: '8px',
                    transition: 'transform 0.2s ease-in-out',
                    width: '100%',
                    boxShadow: '0 2px 10px rgba(253, 216, 53, 0.2)'
                }}
                    onMouseEnter={e => e.currentTarget.style.transform = 'scale(1.05)'}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'} >
                    🏠 Home
                </Link>
            </div>

            {/* Stop confirmation modal */}
            {showStopConfirm && (
                <div style={{
                    position: 'fixed',
                    top: 0, left: 0, right: 0, bottom: 0,
                    background: 'rgba(0,0,0,0.45)',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    zIndex: 1000,
                }}>
                    <div style={{
                        background: '#fff',
                        borderRadius: '18px',
                        padding: '32px 28px',
                        boxShadow: '0 8px 40px rgba(0,0,0,0.25)',
                        minWidth: 320,
                        textAlign: 'center',
                    }}>
                        <h2 style={{ color: '#e53935', marginBottom: 18 }}>Stop Match?</h2>
                        <p style={{ color: '#333', marginBottom: 28 }}>Are you sure you want to stop and reset the match?</p>
                        <button
                            style={{
                                background: '#e53935',
                                color: '#fff',
                                fontSize: '1.1rem',
                                padding: '10px 28px',
                                borderRadius: '8px',
                                border: 'none',
                                fontWeight: 700,
                                cursor: 'pointer',
                                marginRight: 16,
                                transition: 'background 0.2s',
                            }}
                            onClick={confirmStop}
                        >
                            Yes, Stop & Reset
                        </button>
                        <button
                            style={{
                                background: '#bdbdbd',
                                color: '#333',
                                fontSize: '1.1rem',
                                padding: '10px 28px',
                                borderRadius: '8px',
                                border: 'none',
                                fontWeight: 600,
                                cursor: 'pointer',
                                transition: 'background 0.2s',
                            }}
                            onClick={cancelStop}
                        >
                            Cancel
                        </button>
                    </div>
                </div>
            )}
        </div>
    );
}

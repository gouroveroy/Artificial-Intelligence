import { useEffect, useState } from 'react';
import { useRouter } from 'next/router';
import Board from '../components/Board';
import Link from 'next/link';
import confetti from 'canvas-confetti';

export default function HomePage() {
    const router = useRouter();
    const { rows = 9, cols = 6 } = router.query;

    const [board, setBoard] = useState(null);
    const [header, setHeader] = useState(false);
    const [isDisabled, setIsDisabled] = useState(true);
    const [message, setMessage] = useState('');
    const [orbCounts, setOrbCounts] = useState({ R: 0, B: 0 });
    const [showResetConfirm, setShowResetConfirm] = useState(false);
    const [turn, setTurn] = useState(1);

    // Add AI config state
    const [aiDepth, setAiDepth] = useState(2);
    const [aiHeuristic, setAiHeuristic] = useState(0);

    useEffect(() => {
        async function fetchInitial() {
            let resp = await fetch('/api/read-state');
            let data = await resp.json();
            setBoard(data.board);
            setIsDisabled(data.header !== 'AI Move:');
            updateOrbCounts(data.board);
        }
        if (!board && rows && cols) fetchInitial();
    }, [board, rows, cols]);

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

    async function animateFrames(frames) {
        for (const frame of frames) {
            setBoard(frame);
            updateOrbCounts(frame);
            await new Promise((r) => setTimeout(r, 250));
        }
    }

    async function handleCellClick(r, c) {
        if (isDisabled) return;
        setIsDisabled(true);

        const resp = await fetch('/api/compute-ai-move', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ humanMove: { row: r, col: c }, aiDepth, aiHeuristic })
        });

        const result = await resp.json();
        if (resp.status !== 200) {
            alert('Invalid move or server error: ' + JSON.stringify(result));
            setIsDisabled(false);
            return;
        }

        await animateFrames(result.humanFrames);
        setHeader(true);
        setTurn(t => t + 1);

        if (result.aiMove === null && !result.aiWon) {
            setMessage('🎉 Congratulations! You (Red) have won! (AI had no moves)');
            confetti({ particleCount: 200, spread: 100, origin: { y: 0.6 } });
            setIsDisabled(true);
            // Log summary for human win
            fetch('/api/log-summary', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    mode: 'human-vs-ai',
                    winner: 'Red',
                    blueAgent: {
                        depth: aiDepth,
                        heuristic: aiHeuristic
                    }
                })
            });
            return;
        }

        if (result.aiWon) {
            await animateFrames(result.aiFrames);
            setMessage('😞 AI (Blue) has won. Better luck next time!');
            confetti({ particleCount: 150, spread: 70, origin: { y: 0.8 }, colors: ['#1976d2'] });
            setIsDisabled(true);
            // Log summary for AI win
            fetch('/api/log-summary', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    mode: 'human-vs-ai',
                    winner: 'Blue',
                    blueAgent: {
                        depth: aiDepth,
                        heuristic: aiHeuristic
                    }
                })
            });
            return;
        }

        await new Promise((r) => setTimeout(r, 500));
        await animateFrames(result.aiFrames);
        setIsDisabled(false);
        setHeader(false);
        setTurn(t => t + 1);
    }

    return (
        <div style={{
            minHeight: '100vh',
            background: 'linear-gradient(135deg,rgb(26, 131, 183) 0%,rgb(9, 52, 95) 100%)',
            textAlign: 'center',
            paddingTop: '40px',
            display: 'flex',
            flexDirection: 'row',
            justifyContent: 'center',
            alignItems: 'flex-start',
        }}>
            {/* LEFT SIDEBAR: Info & AI Config */}
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
                justifyContent: 'flex-start',
            }}>
                <h3 style={{ color: '#ffeb3b', marginBottom: '16px' }}>ℹ️ Game Info</h3>
                <div style={{
                    background: 'rgba(255,255,255,0.08)',
                    borderRadius: 12,
                    padding: '18px 10px',
                    color: '#fff',
                    fontSize: '1.1rem',
                    marginTop: 50,
                    boxShadow: '0 2px 8px rgba(25, 118, 210, 0.12)',
                    width: '100%',
                }}>
                    <b>Moves:</b> {turn - 1}
                    <br />
                    <b>Total Orbs:</b> {orbCounts.R + orbCounts.B}
                </div>
                {/* AI Depth & Heuristic Controls */}
                <div style={{
                    background: 'rgba(255,255,255,0.10)',
                    borderRadius: 12,
                    padding: '18px 14px',
                    color: '#fff',
                    fontSize: '1.08rem',
                    marginTop: 50,
                    boxShadow: '0 2px 8px rgba(25, 118, 210, 0.10)',
                    textAlign: 'left',
                    width: '100%',
                }}>
                    <b>AI Controls (Blue):</b>
                    <div style={{ marginTop: 10 }}>
                        Depth:&nbsp;
                        <input
                            type="number"
                            min={1}
                            max={4}
                            value={aiDepth}
                            onChange={e => {
                                let val = Math.max(1, Math.min(4, Number(e.target.value)));
                                setAiDepth(val);
                            }}
                            style={{ width: 48, borderRadius: 6, padding: 2 }}
                        />
                    </div>
                    <div style={{ marginTop: 10 }}>
                        Heuristic:&nbsp;
                        <input
                            type="number"
                            min={0}
                            max={4}
                            value={aiHeuristic}
                            onChange={e => {
                                let val = Math.max(0, Math.min(4, Number(e.target.value)));
                                setAiHeuristic(val);
                            }}
                            style={{ width: 48, borderRadius: 6, padding: 2 }}
                        />
                    </div>
                </div>
                <div style={{
                    background: 'rgba(255,255,255,0.10)',
                    marginTop: 50,
                    borderRadius: 10,
                    padding: '12px 10px',
                    color: '#ffe082',
                    fontStyle: 'italic',
                    fontSize: '1rem',
                    boxShadow: '0 2px 8px rgba(255, 215, 0, 0.10)',
                    width: '100%',
                }}>
                    💡 Tip: Corners are the safest spots!
                </div>
            </div>

            {/* MAIN GAME AREA */}
            <div style={{ flex: 1, minWidth: 0 }}>
                {message && <h2 style={{ color: '#ffeb3b' }}>{message}</h2>}
                {!message && (
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
                            🔴 You (Red): {orbCounts.R} Orbs
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
                            🔵 AI (Blue): {orbCounts.B} Orbs
                        </div>
                    </div>
                )}

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
                    {board && (
                        <Board boardData={board} onCellClick={handleCellClick} isDisabled={isDisabled} />
                    )}
                </div>
            </div>

            {/* RIGHT SIDEBAR: Buttons */}
            <div style={{
                width: 230,
                minHeight: 600,
                background: 'rgba(255,255,255,0.06)',
                borderRadius: '12px',
                padding: '20px 18px',
                marginLeft: 24,
                boxShadow: '0 4px 20px rgba(0,0,0,0.2)',
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                justifyContent: 'flex-start',
            }}>
                <h3 style={{ color: '#ffeb3b', marginBottom: '16px' }}>🔗 Actions</h3>
                <button
                    style={{
                        padding: '10px 28px',
                        fontSize: '1.1rem',
                        borderRadius: '8px',
                        border: 'none',
                        background: '#388e3c',
                        color: '#fff',
                        cursor: 'pointer',
                        boxShadow: '0 2px 8px rgba(56, 142, 60, 0.2)',
                        transition: 'all 0.3s ease-in-out',
                        width: '100%',
                        marginTop: 100,
                        marginBottom: 16
                    }}
                    onMouseEnter={e => e.currentTarget.style.transform = 'scale(1.1)'}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'}
                    onClick={() => setShowResetConfirm(true)}
                >
                    🔄 Reset Game
                </button>
                <button
                    style={{
                        padding: '10px 28px',
                        fontSize: '1.1rem',
                        borderRadius: '8px',
                        border: 'none',
                        background: '#1976d2',
                        color: '#fff',
                        cursor: 'pointer',
                        boxShadow: '0 2px 8px rgba(25, 118, 210, 0.2)',
                        transition: 'all 0.3s ease-in-out',
                        width: '100%',
                        marginTop: 16,
                        marginBottom: 16
                    }}
                    onMouseEnter={e => e.currentTarget.style.transform = 'scale(1.1)'}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'}
                    onClick={() => router.push('/setup')}
                >
                    🆕 New Game
                </button>
                <Link href="/about"
                    style={{
                        background: 'linear-gradient(90deg, #f7971e 0%, #ffd200 100%)',
                        color: '#222',
                        fontSize: '1.1rem',
                        padding: '12px 32px',
                        borderRadius: '10px',
                        textAlign: 'center',
                        textDecoration: 'none',
                        fontWeight: 700,
                        boxShadow: '0 4px 16px rgba(255, 215, 0, 0.18)',
                        transition: 'transform 0.2s, box-shadow 0.2s',
                        cursor: 'pointer',
                        marginTop: 16,
                        marginBottom: 16,
                        display: 'inline-block',
                        width: '100%',
                    }}
                    onMouseEnter={e => e.currentTarget.style.transform = 'scale(1.1)'}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'}
                >
                    📘 How to Play
                </Link>
                <Link href="/"
                    style={{
                        padding: '10px 28px',
                        fontSize: '1.1rem',
                        borderRadius: '8px',
                        background: '#fbc02d',
                        color: '#000',
                        fontWeight: 600,
                        textDecoration: 'none',
                        boxShadow: '0 2px 8px rgba(251, 192, 45, 0.4)',
                        display: 'inline-block',
                        transition: 'all 0.3s ease-in-out',
                        width: '100%',
                        marginTop: 16,
                        marginBottom: 16
                    }}
                    onMouseEnter={e => e.currentTarget.style.transform = 'scale(1.1)'}
                    onMouseLeave={e => e.currentTarget.style.transform = 'scale(1)'}
                >
                    🏠 Home
                </Link>
            </div>

            {/* Reset confirmation modal */}
            {showResetConfirm && (
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
                        <h2 style={{ color: '#e53935', marginBottom: 18 }}>Reset Game?</h2>
                        <p style={{ color: '#333', marginBottom: 28 }}>Are you sure you want to reset the game? This cannot be undone.</p>
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
                            onClick={async () => {
                                await fetch('/api/reset', {
                                    method: 'POST',
                                    headers: { 'Content-Type': 'application/json' },
                                    body: JSON.stringify({ rows, cols })
                                });
                                window.location.reload();
                            }}
                        >
                            Yes, Reset
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
                            onClick={() => setShowResetConfirm(false)}
                        >
                            Cancel
                        </button>
                    </div>
                </div>
            )}
        </div>
    );
}

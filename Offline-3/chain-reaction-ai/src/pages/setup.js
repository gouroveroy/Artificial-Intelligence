import { useState } from 'react';
import { useRouter } from 'next/router';

export default function Setup() {
    const [rows, setRows] = useState(9);
    const [cols, setCols] = useState(6);
    const [hovered, setHovered] = useState(null); // Track which button is hovered
    const router = useRouter();

    const handleStart = async (mode) => {
        if (rows < 2 || cols < 2) return alert('Rows and columns must be at least 2.');
        await fetch('/api/reset', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ rows, cols })
        });
        const query = `?rows=${rows}&cols=${cols}`;
        router.push(mode === 'human-vs-ai' ? `/game${query}` : `/ai-vs-ai${query}`);
    };

    return (
        <div
            style={{
                minHeight: '100vh',
                background: 'linear-gradient(135deg, #1a83b7 0%, #09345f 100%)',
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                justifyContent: 'center',
                fontFamily: 'Segoe UI, sans-serif',
                padding: '24px'
            }}
        >
            <h1 style={{
                color: '#ffeb3b',
                fontSize: '2.4rem',
                marginBottom: 28,
                letterSpacing: 1.2,
                textShadow: '0 0 12px rgba(255, 235, 59, 0.5)'
            }}>
                🎮 Configure Your Match
            </h1>

            <form
                style={{
                    background: 'rgba(100, 169, 248, 0.95)',
                    borderRadius: '16px',
                    padding: '36px 32px',
                    boxShadow: '0 12px 36px rgba(0, 0, 0, 0.25)',
                    minWidth: 320,
                    maxWidth: 420,
                    textAlign: 'center',
                    backdropFilter: 'blur(4px)',
                    transition: 'transform 0.3s ease',
                }}
                onSubmit={e => e.preventDefault()}
            >
                <div style={{ marginBottom: 24 }}>
                    <label style={{
                        fontWeight: 600,
                        fontSize: '1.1rem',
                        display: 'block',
                        marginBottom: 6,
                        color: '#333'
                    }}>
                        Rows
                    </label>
                    <input
                        type="number"
                        min="2"
                        max="15"
                        value={rows}
                        onChange={e => setRows(Number(e.target.value))}
                        style={{
                            width: '60px',
                            fontSize: '1.1rem',
                            padding: '6px 10px',
                            borderRadius: '8px',
                            border: '1px solid #bbb',
                            textAlign: 'center',
                            outline: 'none'
                        }}
                    />
                </div>

                <div style={{ marginBottom: 32 }}>
                    <label style={{
                        fontWeight: 600,
                        fontSize: '1.1rem',
                        display: 'block',
                        marginBottom: 6,
                        color: '#333'
                    }}>
                        Columns
                    </label>
                    <input
                        type="number"
                        min="2"
                        max="15"
                        value={cols}
                        onChange={e => setCols(Number(e.target.value))}
                        style={{
                            width: '60px',
                            fontSize: '1.1rem',
                            padding: '6px 10px',
                            borderRadius: '8px',
                            border: '1px solid #bbb',
                            textAlign: 'center',
                            outline: 'none'
                        }}
                    />
                </div>

                <button
                    type="button"
                    onClick={() => handleStart('human-vs-ai')}
                    style={{
                        ...buttonStyle('#43cea2', '#185a9d'),
                        transform: hovered === 'human' ? 'scale(1.1)' : 'scale(1)'
                    }}
                    onMouseEnter={() => setHovered('human')}
                    onMouseLeave={() => setHovered(null)}
                >
                    👤 Human vs 🤖 AI
                </button>

                <button
                    type="button"
                    onClick={() => handleStart('ai')}
                    style={{
                        ...buttonStyle('#8e2de2', '#4a00e0'),
                        marginTop: '16px',
                        transform: hovered === 'ai' ? 'scale(1.1)' : 'scale(1)'
                    }}
                    onMouseEnter={() => setHovered('ai')}
                    onMouseLeave={() => setHovered(null)}
                >
                    🤖 AI vs 🤖 AI
                </button>
            </form>
        </div>
    );
}

// Reusable button style
function buttonStyle(color1, color2) {
    return {
        background: `linear-gradient(90deg, ${color1} 0%, ${color2} 100%)`,
        color: '#fff',
        fontSize: '1.2rem',
        padding: '12px 32px',
        borderRadius: '10px',
        border: 'none',
        fontWeight: 700,
        cursor: 'pointer',
        width: '69%',
        transition: 'all 0.3s ease-in-out',
        boxShadow: '0 4px 16px rgba(0,0,0,0.2)',
        outline: 'none',
        textShadow: '0 1px 2px rgba(0,0,0,0.2)',
        userSelect: 'none',
        display: 'inline-block',
    };
}

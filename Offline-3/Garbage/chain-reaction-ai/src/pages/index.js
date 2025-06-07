// File: src/pages/index.js
import { useEffect, useState } from 'react';
import Board from '../components/Board';

export default function HomePage() {
  const [board, setBoard] = useState(
    Array(9)
      .fill(null)
      .map(() =>
        Array(6)
          .fill(null)
          .map(() => ({ count: 0, color: null }))
      )
  );
  const [header, setHeader] = useState(false);
  const [isDisabled, setIsDisabled] = useState(true);
  const [message, setMessage] = useState('');

  useEffect(() => {
    async function fetchInitial() {
      const resp = await fetch('/api/read-state');
      const data = await resp.json();
      setBoard(data.board);
      setIsDisabled(data.header !== 'AI Move:');
    }
    fetchInitial();
  }, []);

  // useEffect(() => {
  //   const interval = setInterval(async () => {
  //     try {
  //       const resp = await fetch('/api/read-state');
  //       const data = await resp.json();
  //       setHeader(data.header);
  //       setBoard(data.board);
  //       setIsDisabled(data.header !== 'AI Move:');
  //     } catch { }
  //   }, 1000);
  //   return () => clearInterval(interval);
  // }, []);

  async function animateFrames(frames) {
    for (const frame of frames) {
      setBoard(frame);
      await new Promise((r) => setTimeout(r, 250));
    }
  }

  async function handleCellClick(r, c) {
    if (isDisabled) return;
    setIsDisabled(true);

    const resp = await fetch('/api/compute-ai-move', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ humanMove: { row: r, col: c } }),
    });
    const result = await resp.json();
    if (resp.status !== 200) {
      alert('Invalid move or server error: ' + JSON.stringify(result));
      setIsDisabled(false);
      return;
    }

    // Animate human move
    await animateFrames(result.humanFrames);
    setHeader(true);

    if (result.aiMove === null && !result.aiWon) {
      setMessage('Congratulations! You (Red) have won! (AI had no moves)');
      setIsDisabled(true);
      return;
    }

    if (result.aiWon) {
      await animateFrames(result.aiFrames);
      setMessage('AI (Blue) has won. Better luck next time!');
      setIsDisabled(true);
      return;
    }

    // Animate AI move
    await new Promise((r) => setTimeout(r, 500));
    await animateFrames(result.aiFrames);
    setIsDisabled(false);
    setHeader(false);
  }

  return (
    <div
      style={{
        minHeight: '100vh',
        background: 'linear-gradient(135deg,rgb(26, 131, 183) 0%,rgb(9, 52, 95) 100%)',
        textAlign: 'center',
        paddingTop: '40px',
        cursor: isDisabled ? 'wait' : 'default',
      }}
    >
      <h1 style={{ color: '#fff', letterSpacing: '2px', marginBottom: '10px' }}>
        Chain Reaction =&gt; You(Red) vs AI(Blue)
      </h1>
      {message && <h2 style={{ color: '#ffeb3b' }}>{message}</h2>}
      {!message && (
        <h2 style={{ color: '#90caf9' }}>
          {header ? "AI's Turn (Blue)" : "Your Turn (Red)"}
        </h2>
      )}
      <div
        style={{
          display: 'inline-block',
          background: '#96DED1',
          borderRadius: '16px',
          boxShadow: '0 8px 32px 0 rgba(100, 101, 122, 0.37)',
          padding: '32px 24px',
          margin: '32px 0',
          position: 'relative',
        }}
      >
        <Board boardData={board} onCellClick={handleCellClick} isDisabled={isDisabled} />
        <button
          style={{
            position: 'absolute',
            top: '20%',
            right: '-200px',
            transform: 'translateY(-50%)',
            padding: '8px 24px',
            fontSize: '1rem',
            borderRadius: '8px',
            border: 'none',
            background: '#388e3c',
            color: '#fff',
            cursor: 'pointer',
            boxShadow: '0 2px 8px rgba(56, 142, 60, 0.2)',
            marginRight: '12px',
            display: message ? 'none' : 'inline-block',
            minWidth: '120px',
          }}
          onClick={async () => {
            await fetch('/api/reset', { method: 'POST' });
            window.location.reload();
          }}
        >
          Reset Game
        </button>
      </div>
      {message ? (
        <button
          style={{
            marginTop: '24px',
            padding: '10px 28px',
            fontSize: '1.1rem',
            borderRadius: '8px',
            border: 'none',
            background: '#1976d2',
            color: '#fff',
            cursor: 'pointer',
            boxShadow: '0 2px 8px rgba(25, 118, 210, 0.2)',
          }}
          onClick={async () => {
            await fetch('/api/reset', { method: 'POST' });
            window.location.reload();
          }}
        >
          Play Again
        </button>
      ) : null}
    </div>
  );
}

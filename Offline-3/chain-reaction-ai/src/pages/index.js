import Link from 'next/link';
import { useState } from 'react';

export default function Home() {
  const [hovered, setHovered] = useState({ game: false, about: false, aivsai: false });
  const [showGameOptions, setShowGameOptions] = useState(false);
  const [loading, setLoading] = useState(false);

  async function handleStartNewGame() {
    setLoading(true);
    await fetch('/api/reset', { method: 'POST' });
    setLoading(false);
    window.location.href = '/game';
  }

  return (
    <div
      style={{
        minHeight: '100vh',
        background: 'linear-gradient(135deg, #1a83b7 0%, #09345f 100%)',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        paddingTop: '60px',
        fontFamily: 'Segoe UI, sans-serif',
      }}
    >
      <h1 style={{
        fontSize: '3.2rem',
        color: '#00FFFF',
        fontWeight: 900,
        letterSpacing: '2px',
        textShadow: '0 0 16px rgba(199, 255, 59, 0.7)',
        marginBottom: '32px',
        textAlign: 'center',
      }}>
        Chain Reaction AI
      </h1>

      <div style={{ textAlign: 'center', marginBottom: '280px' }}>
        <h1 style={{
          fontSize: '2.6rem',
          color: '#fff',
          fontWeight: 700,
          letterSpacing: '1px',
          textShadow: '0 0 8px rgba(255, 235, 59, 0.6)',
        }}>
          Developed by{' '}
          <span style={{
            color: '#C71585',
            fontWeight: 900,
            textShadow: '0 0 12px rgba(255, 235, 59, 0.9)',
          }}>
            Gourove Roy
          </span>
        </h1>
        <p style={{
          fontSize: '1.3rem',
          fontStyle: 'italic',
          color: '#b3e5fc',
          marginTop: '12px',
        }}>
          “Unleash the chain, ignite the reaction —<br />
          Outsmart the AI in this explosive strategy game.”
        </p>
      </div>

      <div style={{
        display: 'flex',
        gap: '50px',
        flexWrap: 'wrap',
        justifyContent: 'center',
        marginBottom: '500px',
      }}>
        <button
          style={{
            background: 'linear-gradient(90deg, #43cea2 0%, #185a9d 100%)',
            color: '#fff',
            fontSize: '1.5rem',
            padding: '20px 48px',
            borderRadius: '14px',
            textAlign: 'center',
            textDecoration: 'none',
            fontWeight: 700,
            boxShadow: hovered.game
              ? '0 10px 40px rgba(67, 206, 162, 0.4)'
              : '0 6px 24px rgba(67, 206, 162, 0.2)',
            transform: hovered.game ? 'scale(1.08) translateY(-4px)' : 'scale(1)',
            transition: 'transform 0.3s ease, box-shadow 0.3s ease',
            cursor: 'pointer',
            minWidth: 180,
            border: 'none',
          }}
          onMouseEnter={() => setHovered(h => ({ ...h, game: true }))}
          onMouseLeave={() => setHovered(h => ({ ...h, game: false }))}
          onClick={() => setShowGameOptions(true)}
        >
          🎮 Human vs AI
        </button>

        <Link
          href="/ai-vs-ai"
          style={{
            background: 'linear-gradient(90deg, #8e2de2 0%, #4a00e0 100%)',
            color: '#fff',
            fontSize: '1.5rem',
            padding: '20px 48px',
            borderRadius: '14px',
            textAlign: 'center',
            textDecoration: 'none',
            fontWeight: 700,
            boxShadow: hovered.aivsai
              ? '0 10px 40px rgba(142, 45, 226, 0.4)'
              : '0 6px 24px rgba(142, 45, 226, 0.2)',
            transform: hovered.aivsai ? 'scale(1.08) translateY(-4px)' : 'scale(1)',
            transition: 'transform 0.3s ease, box-shadow 0.3s ease',
            cursor: 'pointer',
            minWidth: 180,
            marginLeft: 0,
          }}
          onMouseEnter={() => setHovered(h => ({ ...h, aivsai: true }))}
          onMouseLeave={() => setHovered(h => ({ ...h, aivsai: false }))}
        >
          🤖 AI vs AI
        </Link>

        <Link
          href="/about"
          style={{
            background: 'linear-gradient(90deg, #f7971e 0%, #ffd200 100%)',
            color: '#222',
            fontSize: '1.5rem',
            padding: '20px 48px',
            borderRadius: '14px',
            textAlign: 'center',
            textDecoration: 'none',
            fontWeight: 700,
            boxShadow: hovered.about
              ? '0 10px 40px rgba(255, 210, 0, 0.4)'
              : '0 6px 24px rgba(255, 210, 0, 0.2)',
            transform: hovered.about ? 'scale(1.08) translateY(-4px)' : 'scale(1)',
            transition: 'transform 0.3s ease, box-shadow 0.3s ease',
            cursor: 'pointer',
            minWidth: 180,
          }}
          onMouseEnter={() => setHovered(h => ({ ...h, about: true }))}
          onMouseLeave={() => setHovered(h => ({ ...h, about: false }))}
        >
          📘 About Game
        </Link>
      </div>

      {showGameOptions && (
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
            padding: '40px 32px',
            boxShadow: '0 8px 40px rgba(0,0,0,0.25)',
            minWidth: 320,
            textAlign: 'center',
          }}>
            <h2 style={{ color: '#1976d2', marginBottom: 24 }}>Select</h2>
            <button
              style={{
                background: '#43cea2',
                color: '#fff',
                fontSize: '1.2rem',
                padding: '12px 32px',
                borderRadius: '10px',
                border: 'none',
                margin: '0 0 18px 0',
                fontWeight: 700,
                cursor: loading ? 'wait' : 'pointer',
                width: '100%',
                marginBottom: 16,
                transition: 'background 0.2s',
              }}
              disabled={loading}
              onClick={handleStartNewGame}
            >
              {loading ? 'Resetting...' : 'Start New Game'}
            </button>
            <br />
            <button
              style={{
                background: '#1976d2',
                color: '#fff',
                fontSize: '1.2rem',
                padding: '12px 32px',
                borderRadius: '10px',
                border: 'none',
                fontWeight: 700,
                cursor: 'pointer',
                width: '100%',
                marginBottom: 16,
                transition: 'background 0.2s',
              }}
              onClick={() => window.location.href = '/game'}
            >
              Back to Previous Game
            </button>
            <br />
            <button
              style={{
                background: '#bdbdbd',
                color: '#333',
                fontSize: '1rem',
                padding: '8px 24px',
                borderRadius: '8px',
                border: 'none',
                fontWeight: 600,
                cursor: 'pointer',
                width: '100%',
                marginTop: 8,
              }}
              onClick={() => setShowGameOptions(false)}
            >
              Cancel
            </button>
          </div>
        </div>
      )}
    </div>
  );
}

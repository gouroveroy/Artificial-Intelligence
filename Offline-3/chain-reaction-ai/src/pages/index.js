import Link from 'next/link';
import { useState } from 'react';

export default function Home() {
  // Keep previous hover state for button effects
  const [hovered, setHovered] = useState({ start: false, about: false });

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
        color: '#DC143C',
        fontWeight: 900,
        letterSpacing: '2px',
        textShadow: '0 0 16px rgba(53, 3, 3, 0.7)',
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
            color: '#00008B',
            fontWeight: 900,
            textShadow: '0 0 12px rgba(59, 177, 255, 0.9)',
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
        <Link href="/setup">
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
              boxShadow: hovered.start
                ? '0 10px 40px rgba(67, 206, 162, 0.4)'
                : '0 6px 24px rgba(67, 206, 162, 0.2)',
              transform: hovered.start ? 'scale(1.08) translateY(-4px)' : 'scale(1)',
              transition: 'transform 0.3s ease, box-shadow 0.3s ease',
              cursor: 'pointer',
              minWidth: 180,
              border: 'none',
            }}
            onMouseEnter={() => setHovered(h => ({ ...h, start: true }))}
            onMouseLeave={() => setHovered(h => ({ ...h, start: false }))}
          >
            Start Game
          </button>
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
          About Game
        </Link>
      </div>
    </div>
  );
}

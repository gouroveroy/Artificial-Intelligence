import Link from 'next/link';
import { FaBrain, FaBolt, FaGamepad, FaSyncAlt } from 'react-icons/fa';

export default function About() {
    return (
        <div style={{
            minHeight: '100vh',
            background: 'linear-gradient(135deg, #1a83b7 0%, #09345f 100%)',
            color: '#fff',
            padding: '60px 24px',
            fontFamily: 'Segoe UI, sans-serif',
            lineHeight: 1.7,
        }}>
            <div style={{ maxWidth: '800px', margin: '0 auto' }}>
                <h1 style={{
                    fontSize: '2.8rem',
                    color: '#ffeb3b',
                    textAlign: 'center',
                    marginBottom: '32px',
                    letterSpacing: '1px',
                }}>
                    About <span style={{ color: '#fff' }}>Chain Reaction AI</span>
                </h1>

                <section style={{ marginBottom: '36px' }}>
                    <p style={{ fontSize: '1.2rem', marginBottom: '20px', textAlign: 'center' }}>
                        Chain Reaction is a thrilling turn-based strategy game built with 💡 logic and 💥 explosions.
                        Take on a smart AI and fight for dominance on a 9×6 grid using clever placement and chain reactions.
                    </p>
                </section>

                <section style={{ marginBottom: '40px' }}>
                    <h2 style={{ fontSize: '1.6rem', marginBottom: '16px', color: '#90caf9' }}>
                        <FaGamepad style={{ marginRight: '10px' }} />
                        How to Play
                    </h2>
                    <ul style={{ fontSize: '1.1rem', paddingLeft: '20px' }}>
                        <li>🔴 You play as <b>Red</b>. The AI plays as 🔵 <b>Blue</b>.</li>
                        <li>Each turn, place one orb in an empty or your own cell.</li>
                        <li>When a cell exceeds its <b>critical mass</b>, it explodes:</li>
                        <ul style={{ paddingLeft: '20px' }}>
                            <li>2 in corners</li>
                            <li>3 on edges</li>
                            <li>4 in center cells</li>
                        </ul>
                        <li>Explosions send orbs to neighbors and can capture opponent cells.</li>
                        <li>Chain reactions can continue for multiple steps. Use them wisely!</li>
                        <li>Win by eliminating all opponent's orbs from the board.</li>
                    </ul>
                </section>

                <section style={{ marginBottom: '40px' }}>
                    <h2 style={{ fontSize: '1.6rem', marginBottom: '16px', color: '#90caf9' }}>
                        <FaBrain style={{ marginRight: '10px' }} />
                        The AI Challenge
                    </h2>
                    <p style={{ fontSize: '1.1rem' }}>
                        Our AI uses <b>Minimax</b> with <b>alpha-beta pruning</b> to think ahead. It analyzes multiple future steps,
                        evaluates board positions using heuristics, and picks the most strategic move to trap you or trigger massive explosions.
                    </p>
                </section>

                <section style={{ marginBottom: '60px' }}>
                    <h2 style={{ fontSize: '1.6rem', marginBottom: '16px', color: '#90caf9' }}>
                        <FaBolt style={{ marginRight: '10px' }} />
                        Tech Stack
                    </h2>
                    <p style={{ fontSize: '1.1rem' }}>
                        🚀 Built using <b>Next.js</b> for frontend + backend in one<br />
                        🔒 Uses <b>proper-lockfile</b> for safe file-based game state updates<br />
                        🧠 Smart AI via <b>custom minimax engine</b> and heuristic scoring<br />
                        🎨 Interactive board with animations and explosions
                    </p>
                </section>

                <div style={{ textAlign: 'center' }}>
                    <Link href="/" style={{
                        background: '#43cea2',
                        color: '#09345f',
                        fontSize: '1.2rem',
                        padding: '14px 40px',
                        borderRadius: '12px',
                        textDecoration: 'none',
                        fontWeight: 600,
                        transition: 'all 0.3s ease-in-out',
                        boxShadow: '0 4px 12px rgba(0, 0, 0, 0.2)',
                    }}
                        onMouseEnter={(e) => e.currentTarget.style.background = '#2db8a3'}
                        onMouseLeave={(e) => e.currentTarget.style.background = '#43cea2'}
                    >
                        <FaSyncAlt style={{ marginRight: '10px', verticalAlign: '-2px' }} />
                        Back to Game
                    </Link>
                </div>
            </div>
        </div>
    );
}

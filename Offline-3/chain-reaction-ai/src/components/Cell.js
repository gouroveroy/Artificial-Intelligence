// File: src/components/Cell.js
import React from 'react';
import { motion } from 'framer-motion';

export default function Cell({ row, col, cellData, onClick, isDisabled, numRows, numCols }) {
    const { count, color } = cellData;
    const canClick = !isDisabled && (count === 0 || color === 'R');

    const orbPositions = [
        [{ top: '35%', left: '35%', transform: 'translate(-50%, -50%)' }],
        [
            { top: '35%', left: '20%', transform: 'translate(-50%, -50%)' },
            { top: '35%', left: '50%', transform: 'translate(-50%, -50%)' },
        ],
        [
            { top: '20%', left: '35%', transform: 'translate(-50%, -50%)' },
            { top: '50%', left: '20%', transform: 'translate(-50%, -50%)' },
            { top: '50%', left: '50%', transform: 'translate(-50%, -50%)' },
        ],
    ];

    function criticalMass(r, c) {
        // Use dynamic board size for corners/edges
        if (
            (r === 0 || r === numRows - 1) &&
            (c === 0 || c === numCols - 1)
        ) return 2; // corner
        if (
            r === 0 || r === numRows - 1 ||
            c === 0 || c === numCols - 1
        ) return 3; // edge
        return 4; // center
    }

    return (
        <div
            style={{
                width: '60px',
                height: '60px',
                background: 'linear-gradient(135deg,rgb(87, 177, 219), #81d4fa)',
                borderRadius: '12px',
                boxShadow: 'inset 2px 2px 4px #90caf9, inset -2px -2px 4px #e1f5fe',
                border: '1px solid #90caf9',
                position: 'relative',
                cursor: canClick ? 'pointer' : 'default',
                transition: 'transform 0.15s ease-in-out',
                transform: canClick ? 'scale(1)' : 'scale(1)',
            }}
            onClick={() => {
                if (canClick) onClick(row, col);
            }}
            onMouseEnter={(e) => {
                if (canClick) e.currentTarget.style.transform = 'scale(1.1)';
                const cm = e.currentTarget.querySelector('.cm-text');
                if (cm) cm.style.opacity = '1';
            }}
            onMouseLeave={(e) => {
                if (canClick) e.currentTarget.style.transform = 'scale(1)';
                const cm = e.currentTarget.querySelector('.cm-text');
                if (cm) cm.style.opacity = '0';
            }}
        >
            <div
                className="cm-text"
                style={{
                    position: 'absolute',
                    top: '2px',
                    left: '4px',
                    fontSize: '0.75rem',
                    color: '#01579b',
                    fontWeight: 600,
                    opacity: 0,
                    transition: 'opacity 0.2s ease-in-out',
                    userSelect: 'none',
                }}
            >
                {criticalMass(row, col)}
            </div>

            {color && count > 0 && count <= 3 &&
                Array.from({ length: count }).map((_, idx) => (
                    <motion.div
                        key={idx}
                        initial={{ scale: 0.2, opacity: 0 }}
                        animate={{ scale: 1, opacity: 1 }}
                        transition={{ type: 'spring', stiffness: 300, damping: 20 }}
                        style={{
                            position: 'absolute',
                            width: '18px',
                            height: '18px',
                            borderRadius: '50%',
                            backgroundColor: color === 'R' ? '#e53935' : '#1e88e5',
                            border: '2px solid #fff',
                            boxShadow: `0 2px 6px ${color === 'R' ? 'rgba(229,57,53,0.5)' : 'rgba(30,136,229,0.5)'}`,
                            ...orbPositions[count - 1][idx],
                        }}
                    />
                ))}
        </div>
    );
}

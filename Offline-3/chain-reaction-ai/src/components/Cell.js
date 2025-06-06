// File: src/components/Cell.js
import React from 'react';

/**
 * Props:
 *  - row: number (0–8)
 *  - col: number (0–5)
 *  - cellData: { count: number, color: 'R'|'B'|null }
 *  - onClick: (row, col) => void
 *  - isDisabled: boolean (whether clicking is disabled)
 */
export default function Cell({ row, col, cellData, onClick, isDisabled }) {
    const { count, color } = cellData;
    const canClick = !isDisabled && (count === 0 || color === 'R');

    const orbPositions = [
        // For 1 orb (center)
        [{ top: '50%', left: '50%', transform: 'translate(-50%, -50%)' }],
        // For 2 orbs (left and right)
        [
            { top: '50%', left: '35%', transform: 'translate(-50%, -50%)' },
            { top: '50%', left: '65%', transform: 'translate(-50%, -50%)' },
        ],
        // For 3 orbs (triangle)
        [
            { top: '35%', left: '50%', transform: 'translate(-50%, -50%)' },
            { top: '65%', left: '34%', transform: 'translate(-50%, -50%)' },
            { top: '65%', left: '66%', transform: 'translate(-50%, -50%)' },
        ],
    ];

    return (
        <div
            style={{
                width: '60px',
                height: '60px',
                border: '1px solid #555',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                cursor: canClick ? 'pointer' : 'default',
                backgroundColor: '#f9f9f9',
                position: 'relative',
            }}
            onClick={() => {
                if (canClick) onClick(row, col);
            }}
        >
            {color && count > 0 && count <= 3 &&
                Array.from({ length: count }).map((_, idx) => (
                    <div
                        key={idx}
                        style={{
                            position: 'absolute',
                            width: '18px',
                            height: '18px',
                            borderRadius: '50%',
                            backgroundColor: color === 'R' ? 'red' : color === 'B' ? 'blue' : 'transparent',
                            border: '2px solid #fff',
                            boxShadow: '0 1px 4px rgba(0,0,0,0.15)',
                            ...orbPositions[count - 1][idx],
                        }}
                    />
                ))}
        </div>
    );
}

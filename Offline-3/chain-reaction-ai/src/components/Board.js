// File: src/components/Board.js
import React from 'react';
import Cell from './Cell';

export default function Board({ boardData, onCellClick, isDisabled }) {
    // Guard: if boardData is not ready, show nothing or a loading message
    if (!Array.isArray(boardData)) {
        return <div style={{ color: "#fff", textAlign: "center" }}>Loading board...</div>;
    }

    const boardStyle = {
        display: 'grid',
        gridTemplateRows: 'repeat(9, 60px)',
        gridTemplateColumns: 'repeat(6, 60px)',
        gap: '2px',
        margin: '20px auto',
    };

    return (
        <div style={boardStyle}>
            {boardData.map((rowArr, r) =>
                rowArr.map((cellData, c) => (
                    <Cell
                        key={`${r}-${c}`}
                        row={r}
                        col={c}
                        cellData={cellData}
                        onClick={onCellClick}
                        isDisabled={isDisabled}
                    />
                ))
            )}
        </div>
    );
}

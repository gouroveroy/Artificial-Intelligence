// File: src/components/Board.js
import React from 'react';
import Cell from './Cell';

export default function Board({ boardData, onCellClick, isDisabled }) {
    if (!Array.isArray(boardData)) {
        return <div style={{ color: "#fff", textAlign: "center" }}>Loading board...</div>;
    }

    const numRows = boardData.length;
    const numCols = boardData[0]?.length || 0;

    const boardStyle = {
        display: 'grid',
        gridTemplateRows: `repeat(${numRows}, 60px)`,
        gridTemplateColumns: `repeat(${numCols}, 60px)`,
        gap: '1px',
        margin: '2.5px auto',
        border: '2px rgba(10, 70, 129, 0.3)',
        borderRadius: '16px',
        boxShadow: 'inset 0 0 10px rgba(0, 100, 200, 0.3), 0 8px 32px rgba(0, 100, 200, 0.25)',
        background: 'linear-gradient(135deg,rgb(13, 144, 237),rgb(124, 208, 248))',
        overflow: 'hidden'
    };

    return (
        <div style={boardStyle}>
            {boardData.map((rowArr, r) =>
                rowArr.map((cellData, c) => (
                    <Cell
                        row={r}
                        col={c}
                        cellData={cellData}
                        onClick={onCellClick}
                        isDisabled={isDisabled}
                        numRows={numRows}
                        numCols={numCols}
                    />
                ))
            )}
        </div>
    );
}

//
// Created by CavanasD on 2025/10/6.
//

#ifndef MY_APP_GOMOKULOGIC_H
#define MY_APP_GOMOKULOGIC_H

#include <utility>

class GomokuLogic {
public:
    enum Player { None = 0, Black = 1, White = 2 };
    enum GameState { InProgress, BlackWin, WhiteWin, Draw };

    GomokuLogic();

    void reset();

    // Attempt to place a piece for current player at (x,y). Returns true if success.
    bool placePiece(int x, int y);

    // Current game state
    GameState state() const { return currentState; }

    // Whose turn now
    Player currentPlayer() const { return turn; }

    // Access board (15x15)
    const int (*getBoard() const)[15] { return board; }

    // Last move coordinates (-1 if none)
    int lastX() const { return lastMoveX; }
    int lastY() const { return lastMoveY; }

private:
    bool checkWinFrom(int x, int y) const;
    bool isBoardFull() const;

    int board[15][15]{};
    Player turn{Black};
    GameState currentState{InProgress};
    int lastMoveX{-1}, lastMoveY{-1};
};

#endif //MY_APP_GOMOKULOGIC_H
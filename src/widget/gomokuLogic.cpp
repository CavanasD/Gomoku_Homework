//
// Created by CavanasD on 2025/10/6.
//

#include "gomokuLogic.h"

GomokuLogic::GomokuLogic() {
    reset();
}

void GomokuLogic::reset() {
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            board[i][j] = 0;
        }
    }
    turn = Black;
    currentState = InProgress;
    lastMoveX = -1;
    lastMoveY = -1;
}

bool GomokuLogic::placePiece(int x, int y) {
    if (currentState != InProgress) return false;
    if (x < 0 || x >= 15 || y < 0 || y >= 15) return false;
    if (board[x][y] != 0) return false;

    board[x][y] = static_cast<int>(turn);
    lastMoveX = x;
    lastMoveY = y;

    if (checkWinFrom(x, y)) {
        currentState = (turn == Black) ? BlackWin : WhiteWin;
        return true;
    }

    if (isBoardFull()) {
        currentState = Draw;
        return true;
    }

    // Switch turn only if game continues
    turn = (turn == Black) ? White : Black;
    return true;
}

bool GomokuLogic::checkWinFrom(int x, int y) const {
    const int player = board[x][y];
    if (player == 0) return false;

    auto countDir = [&](int dx, int dy) {
        int c = 1;
        // negative direction
        for (int k = 1; k < 5; ++k) {
            int nx = x - k * dx;
            int ny = y - k * dy;
            if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && board[nx][ny] == player) c++;
            else break;
        }
        // positive direction
        for (int k = 1; k < 5; ++k) {
            int nx = x + k * dx;
            int ny = y + k * dy;
            if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && board[nx][ny] == player) c++;
            else break;
        }
        return c;
    };

    // Four directions
    if (countDir(0, 1) >= 5) return true;   // horizontal
    if (countDir(1, 0) >= 5) return true;   // vertical
    if (countDir(1, 1) >= 5) return true;   // main diagonal
    if (countDir(1, -1) >= 5) return true;  // anti diagonal
    return false;
}

bool GomokuLogic::isBoardFull() const {
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (board[i][j] == 0) return false;
        }
    }
    return true;
}

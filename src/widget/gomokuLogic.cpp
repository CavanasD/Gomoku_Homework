#include "gomokuLogic.h"

GomokuLogic::GomokuLogic() {
    reset();
}

void GomokuLogic::reset() {
    // 清空棋盘与状态（黑先，进行中）
    for (auto& row : board) {
        for (auto& cell : row) {
            cell = 0;
        }
    }
    turn = Black;
    currentState = InProgress;
    lastMoveX = -1;
    lastMoveY = -1;
}

bool GomokuLogic::placePiece(int x, int y) {
    // 终局/越界/占用 -> 失败
    if (currentState != InProgress) return false;
    if (x < 0 || x >= 15 || y < 0 || y >= 15) return false;
    if (board[x][y] != 0) return false;

    // 落子并记录最后一步
    board[x][y] = (int)turn;
    lastMoveX = x;
    lastMoveY = y;

    // 胜负判断（从最近一步进行四向扫描）
    if (checkWinFrom(x, y)) {
        currentState = (turn == Black) ? BlackWin : WhiteWin;
        return true;
    }

    // 检查和棋
    if (isBoardFull()) {
        currentState = Draw;
        return true;
    }

    // 若未结束，则交换执棋方
    turn = (turn == Black) ? White : Black;
    return true;
}

bool GomokuLogic::checkWinFrom(int x, int y) const {
    const int player = board[x][y];
    if (player == 0) return false;

    auto countDir = [&](int dx, int dy) {
        int c = 1;
        // 负方向
        for (int k = 1; k < 5; ++k) {
            int nx = x - k * dx;
            int ny = y - k * dy;
            if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && board[nx][ny] == player) c++;
            else break;
        }
        // 正方向
        for (int k = 1; k < 5; ++k) {
            int nx = x + k * dx;
            int ny = y + k * dy;
            if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && board[nx][ny] == player) c++;
            else break;
        }
        return c;
    };

    // 四方向任一达到 5 即胜
    if (countDir(0, 1) >= 5) return true;   // 横
    if (countDir(1, 0) >= 5) return true;   // 竖
    if (countDir(1, 1) >= 5) return true;   // 主对角
    if (countDir(1, -1) >= 5) return true;  // 副对角
    return false;
}

bool GomokuLogic::isBoardFull() const {
    for (const auto& row : board) {
        for (int cell : row) {
            if (cell == 0) return false;
        }
    }
    return true;
}

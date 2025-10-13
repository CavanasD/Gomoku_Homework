//
// Created by CavanasD on 2025/10/6.
//

#ifndef MY_APP_GOMOKULOGIC_H
#define MY_APP_GOMOKULOGIC_H

#include <utility>

// 逻辑层（Model）：
// - 维护棋盘数组、当前执棋方、对局状态；
// - 对外提供：placePiece(x,y) 推进状态；只读 getBoard() 供视图层渲染；lastX/lastY 标记最后一步；
// - 胜负判断与和棋检测都在内部实现，窗口层不参与逻辑判断；
class GomokuLogic {
public:
    enum Player { None = 0, Black = 1, White = 2 };
    enum GameState { InProgress, BlackWin, WhiteWin, Draw };

    GomokuLogic();

    // 重置到初始局面：棋盘清空、黑先、状态进行中
    void reset();

    // 尝试让“当前执棋方”在 (x,y) 落子；成功返回 true，失败（越界/占用/已终局）返回 false
    bool placePiece(int x, int y);

    // 当前对局状态（进行中/胜/和）
    GameState state() const { return currentState; }

    // 当前执棋方（黑/白）
    Player currentPlayer() const { return turn; }

    // 提供只读棋盘视图（15x15）：视图层用它来绘制，不要直接修改
    const int (*getBoard() const)[15] { return board; }

    // 最后一步坐标（用于视图层高亮）
    int lastX() const { return lastMoveX; }
    int lastY() const { return lastMoveY; }

private:
    // 从最近一步出发的胜负检测（四方向局部扫描）
    bool checkWinFrom(int x, int y) const;
    // 是否已无空位（和棋）
    bool isBoardFull() const;

    int board[15][15]{};     // 棋盘数组：0 空，1 黑，2 白
    Player turn{Black};      // 当前执棋方（黑先）
    GameState currentState{InProgress};
    int lastMoveX{-1}, lastMoveY{-1};
};

#endif //MY_APP_GOMOKULOGIC_H
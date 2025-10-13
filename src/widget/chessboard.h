// chessboard.h
#ifndef MY_APP_CHESSBOARD_H
#define MY_APP_CHESSBOARD_H

#include <QWidget>
#include <QMouseEvent>

// 视图层（View）：
// - 只负责绘制棋盘与棋子、处理鼠标事件，不做规则/胜负判断
// - 通过信号 boardClicked(x,y) 把“用户点击的格子坐标”发给上层窗口（Controller）
// - 与逻辑层（GomokuLogic）解耦：绘制数据通过 updateBoard() 获取的只读快照传入
class ChessBoard : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard() override = default;

    // 清空绘制用的棋盘缓存（不影响逻辑层 Model）
    void resetBoard();
    // 从逻辑层传入 15x15 棋盘与最后一步坐标，刷新渲染
    // 提示：board 的签名是 const int (*board)[15]，表示“行指针”, 访问方式为 board[x][y]
    void updateBoard(const int (*board)[15], int lastX, int lastY);

    signals:
        // 当用户在某个交叉点左键点击时发出（x=行，y=列）
        // 上层窗口应转发给逻辑层：logic.placePiece(x,y)
        void boardClicked(int x, int y);

protected:
    // 所有和绘制、鼠标相关的事件都移到这里
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int chess[15][15]{};    // 绘制缓存：0 空，1 黑，2 白（仅供视图层使用）
    int moveX, moveY;       // 鼠标悬停位置（用于高亮）
    int lastMoveX, lastMoveY; // 最后落子位置（用于标记）
};

#endif //MY_APP_CHESSBOARD_H
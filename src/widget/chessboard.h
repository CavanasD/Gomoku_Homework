// chessboard.h
#ifndef MY_APP_CHESSBOARD_H
#define MY_APP_CHESSBOARD_H

#include <QWidget>
#include <QMouseEvent>

class ChessBoard : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard() override = default;


    void resetBoard();
    void updateBoard(const int (*board)[15], int lastX, int lastY);

    signals:
        void boardClicked(int x, int y);

protected:
    // 所有和绘制、鼠标相关的事件都移到这里
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int chess[15][15]{};    // 棋盘状态数组，用于绘制
    int moveX, moveY;       // 鼠标悬停位置
    int lastMoveX, lastMoveY; // 最后落子位置
};

#endif //MY_APP_CHESSBOARD_H
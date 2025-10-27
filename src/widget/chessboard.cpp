#include "chessboard.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPoint>

// 视图层实现：
// - 仅负责绘制与鼠标事件，不包含规则/胜负逻辑；
// - 通过信号 boardClicked(x,y) 通知上层窗口，由窗口把事件转给逻辑层；
// - updateBoard() 从逻辑层获取只读棋盘快照并刷新渲染；

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent),
      moveX(-1), moveY(-1),
      lastMoveX(-1), lastMoveY(-1)
{
    resetBoard();           // 初始化绘制缓存，不影响逻辑层
    setMouseTracking(true); // 开启鼠标移动高亮
}

void ChessBoard::resetBoard() {
    // 清空绘制缓存与最后一步标记（模型数据不在此处维护）
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            chess[i][j] = 0;
        }
    }
    lastMoveX = -1;
    lastMoveY = -1;
    update();
}

void ChessBoard::updateBoard(const int (*board)[15], int lastX, int lastY) {
    // 从逻辑层传入的只读棋盘复制到本地缓存以便绘制
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            chess[i][j] = board[i][j];
        }
    }
    lastMoveX = lastX;
    lastMoveY = lastY;
    update();
}

void ChessBoard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 自适应布局，保证四周留出足够空间，避免边缘的棋子被裁掉一半
    const int pieceR = 18;                  // 棋子半径（与下方绘制保持一致）
    const int padding = pieceR + 2;         // 预留边距，确保最外侧棋子有空间
    const int availW = width()  - 2 * padding;
    const int availH = height() - 2 * padding;
    const int gridSize = std::max(1, std::min(availW, availH) / 14); // 14 段间距（15 条线）
    const int boardSize = gridSize * 14;
    const int originX = (width()  - boardSize) / 2;  // 动态“边距”：使棋盘居中且四周有 padding
    const int originY = (height() - boardSize) / 2;

    // 棋盘背景（木纹色）
    QBrush brush;
    brush.setColor(QColor(0xEEC085));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawRect(originX, originY, boardSize, boardSize);

    // 网格线（0..14 共 15 条，覆盖到棋盘边缘）
    painter.setPen(Qt::black);
    for (int i = 0; i <= 14; ++i) {
        painter.drawLine(originX, originY + i * gridSize, originX + boardSize, originY + i * gridSize);
        painter.drawLine(originX + i * gridSize, originY, originX + i * gridSize, originY + boardSize);
    }

    // 星位
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawRect(originX + 3 * gridSize - 5,  originY + 3 * gridSize - 5,  10, 10);
    painter.drawRect(originX + 11 * gridSize - 5, originY + 3 * gridSize - 5,  10, 10);
    painter.drawRect(originX + 3 * gridSize - 5,  originY + 11 * gridSize - 5, 10, 10);
    painter.drawRect(originX + 11 * gridSize - 5, originY + 11 * gridSize - 5, 10, 10);
    painter.drawRect(originX + 7 * gridSize - 5,  originY + 7 * gridSize - 5,  10, 10);

    // 棋子绘制（注意：i 对应行 -> y 像素；j 对应列 -> x 像素）
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chess[i][j] == 1) {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint(originX + j * gridSize, originY + i * gridSize), pieceR, pieceR);
            } else if (chess[i][j] == 2) {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint(originX + j * gridSize, originY + i * gridSize), pieceR, pieceR);
            }
        }
    }

    // 悬停高亮（红色角标）
    if (moveX >= 0 && moveY >= 0) {
        QPen pen(Qt::red);
        pen.setWidth(1);
        painter.setPen(pen);
        const int centerX = originX + moveY * gridSize;
        const int centerY = originY + moveX * gridSize;
        const int halfSize = pieceR;    // 与棋子半径一致更直观
        const int cornerLen = 8;
        // TL
        painter.drawLine(centerX - halfSize, centerY - halfSize, centerX - halfSize + cornerLen, centerY - halfSize);
        painter.drawLine(centerX - halfSize, centerY - halfSize, centerX - halfSize, centerY - halfSize + cornerLen);
        // TR
        painter.drawLine(centerX + halfSize, centerY - halfSize, centerX + halfSize - cornerLen, centerY - halfSize);
        painter.drawLine(centerX + halfSize, centerY - halfSize, centerX + halfSize, centerY - halfSize + cornerLen);
        // BL
        painter.drawLine(centerX - halfSize, centerY + halfSize, centerX - halfSize + cornerLen, centerY + halfSize);
        painter.drawLine(centerX - halfSize, centerY + halfSize, centerX - halfSize, centerY + halfSize - cornerLen);
        // BR
        painter.drawLine(centerX + halfSize, centerY + halfSize, centerX + halfSize - cornerLen, centerY + halfSize);
        painter.drawLine(centerX + halfSize, centerY + halfSize, centerX + halfSize, centerY + halfSize - cornerLen);
    }

    // 最后一步标记
    if (lastMoveX != -1 && lastMoveY != -1) {
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        const int centerX = originX + lastMoveY * gridSize;
        const int centerY = originY + lastMoveX * gridSize;
        painter.drawLine(centerX - 5, centerY, centerX + 5, centerY);
        painter.drawLine(centerX, centerY - 5, centerX, centerY + 5);
    }
}

void ChessBoard::mouseMoveEvent(QMouseEvent *event) {
    // 与绘制一致的坐标换算，避免因边距/格距变化导致定位偏差
    const int pieceR = 18;
    const int padding = pieceR + 2;
    const int availW = width()  - 2 * padding;
    const int availH = height() - 2 * padding;
    const int gridSize = std::max(1, std::min(availW, availH) / 14);
    const int boardSize = gridSize * 14;
    const int originX = (width()  - boardSize) / 2;
    const int originY = (height() - boardSize) / 2;
    const int x = static_cast<int>((event->position().y() - originY + gridSize / 2) / gridSize);
    const int y = static_cast<int>((event->position().x() - originX + gridSize / 2) / gridSize);

    if (x >= 0 && x < 15 && y >= 0 && y < 15) {
        moveX = x;
        moveY = y;
        update();
    } else if (moveX != -1 || moveY != -1) {
        moveX = moveY = -1;
        update();
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *event) {
    // 左键点击空位时，发出 boardClicked(x,y) 信号；不直接修改模型
    const int pieceR = 18;
    const int padding = pieceR + 2;
    const int availW = width()  - 2 * padding;
    const int availH = height() - 2 * padding;
    const int gridSize = std::max(1, std::min(availW, availH) / 14);
    const int boardSize = gridSize * 14;
    const int originX = (width()  - boardSize) / 2;
    const int originY = (height() - boardSize) / 2;

    if (event->button() == Qt::LeftButton) {
        const int x = static_cast<int>((event->position().y() - originY + gridSize / 2) / gridSize);
        const int y = static_cast<int>((event->position().x() - originX + gridSize / 2) / gridSize);

        if (x >= 0 && x < 15 && y >= 0 && y < 15 && chess[x][y] == 0) {
            emit boardClicked(x, y);
        }
    }
}

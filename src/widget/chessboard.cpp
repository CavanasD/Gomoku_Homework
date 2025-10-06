#include "chessboard.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPoint>

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent),
      moveX(-1), moveY(-1),
      lastMoveX(-1), lastMoveY(-1)
{
    resetBoard();
    setMouseTracking(true);
}

void ChessBoard::resetBoard() {
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

    const int margin = 30;
    const int gridSize = 40;
    const int boardSize = 14 * gridSize;

    // board background
    QBrush brush;
    brush.setColor(QColor(0xEEC085));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawRect(margin, margin, boardSize, boardSize);

    // grid lines
    painter.setPen(Qt::black);
    for (int i = 0; i <= 14; ++i) {
        painter.drawLine(margin, margin + i * gridSize, margin + boardSize, margin + i * gridSize);
        painter.drawLine(margin + i * gridSize, margin, margin + i * gridSize, margin + boardSize);
    }

    // star points
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawRect(margin + 3 * gridSize - 5,  margin + 3 * gridSize - 5,  10, 10);
    painter.drawRect(margin + 11 * gridSize - 5, margin + 3 * gridSize - 5,  10, 10);
    painter.drawRect(margin + 3 * gridSize - 5,  margin + 11 * gridSize - 5, 10, 10);
    painter.drawRect(margin + 11 * gridSize - 5, margin + 11 * gridSize - 5, 10, 10);
    painter.drawRect(margin + 7 * gridSize - 5,  margin + 7 * gridSize - 5,  10, 10);

    // pieces
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chess[i][j] == 1) {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint(margin + j * gridSize, margin + i * gridSize), 18, 18);
            } else if (chess[i][j] == 2) {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint(margin + j * gridSize, margin + i * gridSize), 18, 18);
            }
        }
    }

    // hover effect
    if (moveX >= 0 && moveY >= 0) {
        QPen pen(Qt::red);
        pen.setWidth(1);
        painter.setPen(pen);
        const int centerX = margin + moveY * gridSize;
        const int centerY = margin + moveX * gridSize;
        const int halfSize = 18;
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

    // last move mark
    if (lastMoveX != -1 && lastMoveY != -1) {
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        const int centerX = margin + lastMoveY * gridSize;
        const int centerY = margin + lastMoveX * gridSize;
        painter.drawLine(centerX - 5, centerY, centerX + 5, centerY);
        painter.drawLine(centerX, centerY - 5, centerX, centerY + 5);
    }
}

void ChessBoard::mouseMoveEvent(QMouseEvent *event) {
    const int margin = 30;
    const int gridSize = 40;

    const int x = static_cast<int>((event->position().y() - margin + gridSize / 2) / gridSize);
    const int y = static_cast<int>((event->position().x() - margin + gridSize / 2) / gridSize);

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
    const int margin = 30;
    const int gridSize = 40;

    if (event->button() == Qt::LeftButton) {
        const int x = static_cast<int>((event->position().y() - margin + gridSize / 2) / gridSize);
        const int y = static_cast<int>((event->position().x() - margin + gridSize / 2) / gridSize);

        if (x >= 0 && x < 15 && y >= 0 && y < 15 && chess[x][y] == 0) {
            emit boardClicked(x, y);
        }
    }
}

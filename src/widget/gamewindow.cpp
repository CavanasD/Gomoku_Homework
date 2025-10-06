#include "gamewindow.h"
#include "ui_gameWindow.h"
#include "gomoku.h"
#include "ui_gomoku.h"
#include <Qpainter>
#include <QDebug>

gameWindow::gameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gameWindow),
    isPlayerOneTurn(true),
    gameActive(false),
    moveX(-0)
{
    ui->setupUi(this);

    // Connet StartButton/ResetButton Signal to Slots
    connect(ui->startBut, &QPushButton::clicked, this, &gameWindow::on_startBut_clicked);
    connect(ui->resetBut, &QPushButton::clicked, this, &gameWindow::on_resetBut_clicked);
    // Initialize chess array
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            chess[i][j] = 0;
        }
    }
    this->setFixedSize(850,660);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0xB1723C));
    this->setPalette(palette);
    setMouseTracking(true);
}

gameWindow::~gameWindow() {
    delete ui;
}

void gameWindow::updateTurnLabel() const {
    if (gameActive) {
        if (isPlayerOneTurn) {
            ui->whoTurn->setText("黑棋");
        }else {
            ui->whoTurn->setText("白棋");
        }
    }else {
        ui->whoTurn->setText("请开始");
    }
}

bool gameWindow::checkWin(int x, int y) {
    int playerColor = chess[x][y];
    if (playerColor == 0) {
        return false; // 如果是空点，直接返回
    }
    int count = 1;

    for (int i = 1; i < 5; ++i) {
        if (y - i >= 0 && chess[x][y - i] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    for (int i = 1; i < 5; ++i) {
        if (y + i < 15 && chess[x][y + i] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    if (count >= 5) {
        return true;
    }
    count = 1;
    for (int i = 1; i < 5; ++i) {
        if (x - i >= 0 && chess[x - i][y] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    for (int i = 1; i < 5; ++i) {
        if (x + i < 15 && chess[x + i][y] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    if (count >= 5) {
        return true;
    }
    count = 1;
    for (int i = 1; i < 5; ++i) {
        if (x - i >= 0 && y - i >= 0 && chess[x - i][y - i] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    for (int i = 1; i < 5; ++i) {
        if (x + i < 15 && y + i < 15 && chess[x + i][y + i] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    if (count >= 5) {
        return true;
    }
    count = 1;
    for (int i = 1; i < 5; ++i) {
        if (x - i >= 0 && y + i < 15 && chess[x - i][y + i] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    for (int i = 1; i < 5; ++i) {
        if (x + i < 15 && y - i >= 0 && chess[x + i][y - i] == playerColor) {
            count++;
        } else {
            break;
        }
    }
    if (count >= 5) {
        return true;
    }
    return false;
}

// ==================== Button Widgets =====================
void gameWindow::on_backToMain_clicked() {
    this->close();
    auto *gw = new Gomoku();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}

void gameWindow::on_startBut_clicked() {
    if (!gameActive) {
         gameActive = true;
        on_resetBut_clicked();
        ui->whoWin->setText("你能打败流萤酱吗");
        ui->whoWin->setText("喜欢流萤酱喵");
        updateTurnLabel();
    }
}

void gameWindow::on_resetBut_clicked() {
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            chess[i][j] = 0;
        }
    }
    isPlayerOneTurn = true;
    lastMoveX = -1;
    lastMoveY = -1;
    // Trigger RePaint Event
    update();
    updateTurnLabel();
    ui->whoWin->setText("你能打败流萤酱吗");
}
// ==================== MainL =====================
void gameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // Define Margin Val:
    int margin = 30;
    int gridSize = 40;
    int boardSize = 14 * gridSize;

    // Draw Board
    QBrush brush;
    brush.setColor(QColor(0xEEC085));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawRect(margin, margin , boardSize, boardSize);
    // Draw Line
    painter.setPen(Qt::black);
    for (int i = 0; i <= 14; ++i) {
        painter.drawLine(margin, margin + i * gridSize, margin + boardSize, margin + i * gridSize); // Horizontal lines
        painter.drawLine(margin + i * gridSize, margin, margin + i * gridSize, margin + boardSize); // Vertical lines
    }
    // Draw 5 jb Pieces
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawRect(margin + 3 * gridSize - 5, margin + 3 * gridSize - 5, 10, 10);
    painter.drawRect(margin + 11 * gridSize - 5, margin + 3 * gridSize - 5, 10, 10);
    painter.drawRect(margin + 3 * gridSize - 5, margin + 11 * gridSize - 5, 10, 10);
    painter.drawRect(margin + 11 * gridSize - 5, margin + 11 * gridSize - 5, 10, 10);
    painter.drawRect(margin + 7 * gridSize - 5, margin + 7 * gridSize - 5, 10, 10);
    //fuck
    // Draw Chess
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chess[i][j] == 1) { // 黑子
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint(margin + j * gridSize, margin + i * gridSize), 18, 18);
            } else if (chess[i][j] == 2) { // 白子
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint(margin + j * gridSize, margin + i * gridSize), 18, 18);
            }
        }
    }
    // SHow hover EFFECT
    if (gameActive && moveX >= 0 && moveY >= 0) {
        QPen pen(Qt::red);
        pen.setWidth(1);
        painter.setPen(pen);
        int centerX = margin + moveY * gridSize;
        int centerY = margin + moveX * gridSize;
        int halfSize = 18; // 方框的半边长
        int cornerLen = 8; // 每个角线的长度
        //左上
        painter.drawLine(centerX - halfSize, centerY - halfSize, centerX - halfSize + cornerLen, centerY - halfSize);
        painter.drawLine(centerX - halfSize, centerY - halfSize, centerX - halfSize, centerY - halfSize + cornerLen);
        // 右上
        painter.drawLine(centerX + halfSize, centerY - halfSize, centerX + halfSize - cornerLen, centerY - halfSize);
        painter.drawLine(centerX + halfSize, centerY - halfSize, centerX + halfSize, centerY - halfSize + cornerLen);
        // 左下
        painter.drawLine(centerX - halfSize, centerY + halfSize, centerX - halfSize + cornerLen, centerY + halfSize);
        painter.drawLine(centerX - halfSize, centerY + halfSize, centerX - halfSize, centerY + halfSize - cornerLen);
        // 右下
        painter.drawLine(centerX + halfSize, centerY + halfSize, centerX + halfSize - cornerLen, centerY + halfSize);
        painter.drawLine(centerX + halfSize, centerY + halfSize, centerX + halfSize, centerY + halfSize - cornerLen);
    }
    // Highlight Last Move
    if (lastMoveX != -1 && lastMoveY != -1) {
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        int centerX = margin + lastMoveY * gridSize;
        int centerY = margin + lastMoveX * gridSize;
        painter.drawLine(centerX - 5, centerY, centerX + 5, centerY);
        painter.drawLine(centerX, centerY - 5, centerX, centerY + 5);
    }
}
void gameWindow::mouseMoveEvent(QMouseEvent *event) {
    if (!gameActive) return;

    int margin = 30;
    int gridSize = 40;
    // 将窗口坐标转换为棋盘格子坐标
    int x = (event->y() - margin + gridSize / 2) / gridSize;
    int y = (event->x() - margin + gridSize / 2) / gridSize;

    if (x >= 0 && x < 15 && y >= 0 && y < 15) {
        moveX = x;
        moveY = y;
        update(); // 触发重绘
    }
}

void gameWindow::fuckingAlgoPart() {

}

void gameWindow::mousePressEvent(QMouseEvent *event) {
    if (!gameActive) return;
    int margin = 30;
    int gridSize = 40;

    if (event->button() == Qt::LeftButton) {
        // 将窗口坐标转换为棋盘格子坐标
        int x = (event->y() - margin + gridSize / 2) / gridSize;
        int y = (event->x() - margin + gridSize / 2) / gridSize;

        // 检查点击是否在棋盘内且该位置没有棋子
        if (x >= 0 && x < 15 && y >= 0 && y < 15 && chess[x][y] == 0) {
            // 设置棋子
            chess[x][y] = isPlayerOneTurn ? 1 : 2;
            // 更新最后落子位置
            lastMoveX = x;
            lastMoveY = y;
            // 切换玩家
            isPlayerOneTurn = !isPlayerOneTurn;
            if (checkWin(x, y)) { // 检查游戏状态
                gameActive = false;
                QString winner;
                if (isPlayerOneTurn) {
                    winner = "白棋";
                } else {
                    winner = "黑棋";
                }
                ui->whoWin->setText(winner + "胜利！");
                ui->whoTurn->setText("游戏结束");
            } else {
                updateTurnLabel();
            }
            update();
        }
    }
}



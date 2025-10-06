#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "gomoku.h"
#include <QDebug>
#include "chessboard.h"


gameWindow::gameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gameWindow)
{
    ui->setupUi(this);

    // Connect Start/Reset buttons
    connect(ui->startBut, &QPushButton::clicked, this, &gameWindow::on_startBut_clicked);
    connect(ui->resetBut, &QPushButton::clicked, this, &gameWindow::on_resetBut_clicked);

    // Connect ChessBoard click signal
    connect(ui->chessBoardWidget, &ChessBoard::boardClicked,
            this, &gameWindow::handleBoardClick);

    this->setFixedSize(850, 660);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0xB1723C));
    this->setPalette(palette);

    // Initially not active until Start is pressed
    gameActive = false;
    logic.reset();
    ui->chessBoardWidget->resetBoard();
    updateTurnLabel();
}

gameWindow::~gameWindow() {
    delete ui;
}

void gameWindow::updateTurnLabel() const {
    if (!gameActive) {
        ui->whoTurn->setText("请开始");
        return;
    }
    ui->whoTurn->setText(logic.currentPlayer() == GomokuLogic::Black ? "黑棋" : "白棋");
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
        updateTurnLabel();
    }
}

void gameWindow::on_resetBut_clicked() {
    logic.reset();
    ui->chessBoardWidget->resetBoard();
    updateTurnLabel();
    ui->whoWin->setText("你能打败流萤酱吗");
}

// ==================== Main Logics =====================
void gameWindow::handleBoardClick(int x, int y) {
    if (!gameActive) return;

    // Try to place a piece for current player
    if (!logic.placePiece(x, y)) {
        return; // invalid move or game ended
    }

    // Update board view
    ui->chessBoardWidget->updateBoard(logic.getBoard(), logic.lastX(), logic.lastY());

    // Check game state
    switch (logic.state()) {
        case GomokuLogic::BlackWin:
            gameActive = false;
            ui->whoWin->setText("黑棋胜利！");
            ui->whoTurn->setText("游戏结束");
            break;
        case GomokuLogic::WhiteWin:
            gameActive = false;
            ui->whoWin->setText("白棋胜利！");
            ui->whoTurn->setText("游戏结束");
            break;
        case GomokuLogic::Draw:
            gameActive = false;
            ui->whoWin->setText("平局！");
            ui->whoTurn->setText("游戏结束");
            break;
        case GomokuLogic::InProgress:
            // Game continues, update turn label
            updateTurnLabel();
            break;
    }
}

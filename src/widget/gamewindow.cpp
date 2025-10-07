#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "gomoku.h"
#include <QDebug>
#include "chessboard.h"
#include "aibrain.h"
#include <QTimer>
#include <memory>
#include <QCoreApplication>
#include <QEventLoop>


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

// 新增：人机模式构造

gameWindow::gameWindow(bool vsAI_, GomokuLogic::Player aiSide_, QWidget *parent)
    : QWidget(parent), ui(new Ui::gameWindow), vsAI(vsAI_), aiSide(aiSide_)
{
    ui->setupUi(this);

    connect(ui->startBut, &QPushButton::clicked, this, &gameWindow::on_startBut_clicked);
    connect(ui->resetBut, &QPushButton::clicked, this, &gameWindow::on_resetBut_clicked);

    connect(ui->chessBoardWidget, &ChessBoard::boardClicked,
            this, &gameWindow::handleBoardClick);

    this->setFixedSize(850, 660);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0xB1723C));
    this->setPalette(palette);

    gameActive = false;
    logic.reset();
    ui->chessBoardWidget->resetBoard();

    if (vsAI) {
        // 默认参数：200ms 思考，迭代上限 10000，启用邻域半径 2
        aiBrain = std::make_unique<MCTSBrain>(250, 12000, 1.2, true, 2);
    }

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
    if (vsAI && aiThinking) {
        ui->whoTurn->setText("AI思考中…");
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
        // 如果是人机且开局轮到 AI，下发 AI 回合
        if (vsAI && logic.currentPlayer() == aiSide) {
            aiThinking = true;
            updateTurnLabel();
            QTimer::singleShot(0, this, &gameWindow::triggerAIMove);
        }
    }
}

void gameWindow::on_resetBut_clicked() {
    logic.reset();
    ui->chessBoardWidget->resetBoard();
    aiThinking = false;
    updateTurnLabel();
    ui->whoWin->setText("你能打败流萤酱吗");
    // 如果重置后轮到 AI，并且是人机模式，触发 AI
    if (gameActive && vsAI && logic.currentPlayer() == aiSide) {
        aiThinking = true;
        updateTurnLabel();
        QTimer::singleShot(0, this, &gameWindow::triggerAIMove);
    }
}

// ==================== Main Logics =====================
void gameWindow::handleBoardClick(int x, int y) {
    if (!gameActive) return;

    // 人机模式下，若当前应由 AI 落子或 AI 正在思考，则屏蔽玩家点击
    if (vsAI && (aiThinking || logic.currentPlayer() == aiSide)) {
        return;
    }

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
            // Game continues
            // 如果人机且轮到 AI，进入 AI 思考
            if (vsAI && logic.currentPlayer() == aiSide) {
                aiThinking = true;
                updateTurnLabel();
                QTimer::singleShot(0, this, &gameWindow::triggerAIMove);
            } else {
                updateTurnLabel();
            }
            break;
    }
}

void gameWindow::triggerAIMove() {
    if (!gameActive || !vsAI) { aiThinking = false; updateTurnLabel(); return; }
    if (!aiBrain) { aiThinking = false; updateTurnLabel(); return; }
    if (logic.currentPlayer() != aiSide) { aiThinking = false; updateTurnLabel(); return; }

    // 显示“AI思考中…”，并强制刷新 UI
    aiThinking = true;
    ui->whoTurn->setText("AI思考中…");
    ui->whoTurn->repaint();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // 调用 AI 搜索
    auto [mx, my] = aiBrain->getBestMove(logic.getBoard());

    // 兜底：若 AI 未给出有效点，随机找一个空位（极少出现）
    if (mx < 0 || my < 0 || mx >= 15 || my >= 15 || logic.getBoard()[mx][my] != 0) {
        // 简单线性查找空位
        for (int i=0;i<15 && (mx<0||my<0);++i)
            for (int j=0;j<15 && (mx<0||my<0);++j)
                if (logic.getBoard()[i][j]==0) { mx=i; my=j; }
    }

    if (mx>=0 && my>=0) {
        // 让逻辑层以“当前玩家”(应为 AI) 落子
        if (logic.placePiece(mx, my)) {
            ui->chessBoardWidget->updateBoard(logic.getBoard(), logic.lastX(), logic.lastY());
        }
    }

    // 思考结束
    aiThinking = false;

    // 检查对局状态
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
            updateTurnLabel();
            break;
    }
}

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

// 窗口层职责：
// - 绑定 UI（按钮/棋盘）事件；
// - 仅通过 GomokuLogic 接口推进状态（不直接修改棋盘数组）；
// - 在合适时机触发 AI（triggerAIMove），并把结果回填到逻辑层再渲染；
// - 棋盘数据读取接口：logic.getBoard()；当前执棋方：logic.currentPlayer()；


gameWindow::gameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gameWindow)
{
    ui->setupUi(this);

    // 初始化倒计时显示与计时器
    initClocks();

    // 绑定按钮事件：开始/重置
    connect(ui->startBut, &QPushButton::clicked, this, &gameWindow::on_startBut_clicked);
    connect(ui->resetBut, &QPushButton::clicked, this, &gameWindow::on_resetBut_clicked);

    // 棋盘（视图层）点击 -> 窗口层槽函数 -> 逻辑层 placePiece(x,y)
    connect(ui->chessBoardWidget, &ChessBoard::boardClicked,
            this, &gameWindow::handleBoardClick);

    // 外观初始化
    this->setFixedSize(850, 660);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0xB1723C));
    this->setPalette(palette);

    // 初始未开始，对局需点击“开始”激活
    gameActive = false;
    logic.reset();              // 逻辑层重置：棋盘清空、黑先、状态进行中
    ui->chessBoardWidget->resetBoard(); // 视图层清空缓存
    updateTurnLabel();
}

// 新增：人机模式构造（仅用于配置 vsAI/aiSide，并在此处实例化 AI 引擎）
// 提示：AI 实现应封装在 AIBrain 子类（如 AlphaBeta/AlphaBetaBrain），窗口层只关心 getBestMove()

gameWindow::gameWindow(bool vsAI_, GomokuLogic::Player aiSide_, QWidget *parent)
    : QWidget(parent), ui(new Ui::gameWindow), vsAI(vsAI_), aiSide(aiSide_)
{
    ui->setupUi(this);

    // 初始化倒计时显示与计时器
    initClocks();

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
        // 【AI 引擎实例化处】改为 AlphaBeta 强化搜索
        aiBrain = std::make_unique<AlphaBeta>(
            300,   // timeLimitMs 每步思考时长
            20000, // maxIterations（占位，不在 AlphaBeta 中直接使用）
            1.2,   // explorationC（占位）
            true,  // useNeighborhood 邻域裁剪
            2      // neighborhoodRadius 裁剪半径
        );
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
        ui->whoTurn->setText("流萤酱正在思考喵~…");
        return;
    }
    // 【当前执棋方获取】从逻辑层读取，不由 UI 自己维护
    ui->whoTurn->setText(logic.currentPlayer() == GomokuLogic::Black ? "黑棋" : "白棋");
}

// ==================== Button Widgets =====================
void gameWindow::on_backToMain_clicked() {
    // 返回主菜单（与对局窗口解耦）
    this->close();
    auto *gw = new Gomoku();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}

void gameWindow::on_startBut_clicked() {
    if (!gameActive) {
        gameActive = true;
        on_resetBut_clicked();   // 复位模型与视图
        ui->whoWin->setText("你能打败流萤酱吗");
        updateTurnLabel();
        startClock();            // 开始倒计时
        // 人机开局：若当前轮到 AI，则异步触发（避免阻塞 UI 线程）
        if (vsAI && logic.currentPlayer() == aiSide) {
            aiThinking = true;
            updateTurnLabel();
            QTimer::singleShot(0, this, &gameWindow::triggerAIMove);
        }
    }
}

void gameWindow::on_resetBut_clicked() {
    // 逻辑层 reset + 视图层 reset：保持职责分离
    logic.reset();
    ui->chessBoardWidget->resetBoard();
    aiThinking = false;
    updateTurnLabel();
    ui->whoWin->setText("你能打败流萤酱吗");

    // 重置双方剩余时间并刷新显示
    whiteRemaining = initialSeconds;
    blackRemaining = initialSeconds;
    refreshClockDisplays();

    // 若重置后先手为 AI，继续触发 AI
    if (gameActive && vsAI && logic.currentPlayer() == aiSide) {
        aiThinking = true;
        updateTurnLabel();
        QTimer::singleShot(0, this, &gameWindow::triggerAIMove);
    }

    // 重置后若处于对局中，重新启动计时
    if (gameActive) startClock(); else stopClock();
}

// ==================== Main Logics =====================
void gameWindow::handleBoardClick(int x, int y) {
    if (!gameActive) return;

    // 人机模式：若轮到 AI 或 AI 正在思考，屏蔽玩家点击
    if (vsAI && (aiThinking || logic.currentPlayer() == aiSide)) {
        qDebug() << "[CLICK BLOCKED] aiThinking=" << aiThinking << " currentPlayer=" << (logic.currentPlayer()==GomokuLogic::Black?"Black":"White") << " aiSide=" << (aiSide==GomokuLogic::Black?"Black":"White");
        return;
    }
    qDebug() << "[USER CLICK] attempt" << x << y << " currentPlayer=" << (logic.currentPlayer()==GomokuLogic::Black?"Black":"White");

    // 【逻辑层落子接口】校验合法性/切换执棋方/判胜负均在 placePiece 内完成
    if (!logic.placePiece(x, y)) {
        return; // 非法或已结束
    }

    // 【获取棋盘信息】logic.getBoard() 为 15x15 const 数组；lastX/lastY 为最后一步
    ui->chessBoardWidget->updateBoard(logic.getBoard(), logic.lastX(), logic.lastY());

    // 根据逻辑层状态更新 UI，不在此写胜负判定逻辑（保持解耦）
    switch (logic.state()) {
        case GomokuLogic::BlackWin:
            gameActive = false;
            ui->whoWin->setText("黑棋胜利！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            break;
        case GomokuLogic::WhiteWin:
            gameActive = false;
            ui->whoWin->setText("白棋胜利！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            break;
        case GomokuLogic::Draw:
            gameActive = false;
            ui->whoWin->setText("平局！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            break;
        case GomokuLogic::InProgress:
            // 若轮到 AI，则异步触发 AI 思考
            if (vsAI && logic.currentPlayer() == aiSide) {
                aiThinking = true;
                updateTurnLabel();
                QTimer::singleShot(0, this, &gameWindow::triggerAIMove);
            } else {
                updateTurnLabel();
            }
            // 不需要切换计时器，onClockTick 会按 currentPlayer 动态扣时
            break;
    }
}

void gameWindow::triggerAIMove() {
    // 保护条件：仅在进行中、人机模式、轮到 AI 且存在 aiBrain 时执行
    if (!gameActive || !vsAI) { aiThinking = false; updateTurnLabel(); return; }
    if (!aiBrain) { aiThinking = false; updateTurnLabel(); return; }
    if (logic.currentPlayer() != aiSide) { aiThinking = false; updateTurnLabel(); return; }

    // UI 提示：AI 思考中（注意：当前实现仍在主线程计算，重度计算建议放到 QThread/QtConcurrent）
    aiThinking = true;
    ui->whoTurn->setText("流萤正在思考喵…");
    ui->whoTurn->repaint();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // 【AI 调用入口 & 剪枝接入点】
    auto [mx, my] = aiBrain->getBestMove(logic.getBoard());
    qDebug() << "[AI MOVE RAW] returned" << mx << my;

    // 新策略：若 AI 返回 {-1,-1} 视为“暂无落子”直接结束，不做兜底填左上角
    bool haveMove = (mx >= 0 && my >= 0 && mx < 15 && my < 15);
    if (haveMove && logic.getBoard()[mx][my] != 0) {
        // 只有在 AI 给出合法范围但该位置非空时，才做线性兜底寻找第一个空位
        bool found=false;
        for (int i=0;i<15 && !found;++i)
            for (int j=0;j<15 && !found;++j)
                if (logic.getBoard()[i][j]==0) { mx=i; my=j; found=true; }
        haveMove = found; // 若没找到空位则视为无着法
    }

    if (haveMove) {
        bool placed = logic.placePiece(mx, my);
        qDebug() << "[AI PLACE]" << (placed?"success":"fail") << "at" << mx << my << " nextPlayer=" << (logic.currentPlayer()==GomokuLogic::Black?"Black":"White");
        if (placed) {
            ui->chessBoardWidget->updateBoard(logic.getBoard(), logic.lastX(), logic.lastY());
        }
    } else {
        qDebug() << "[AI NO MOVE] AI chose to skip move";
    }

    // 思考结束
    aiThinking = false;

    // 刷新状态/结果
    switch (logic.state()) {
        case GomokuLogic::BlackWin:
            gameActive = false;
            ui->whoWin->setText("黑棋胜利！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            break;
        case GomokuLogic::WhiteWin:
            gameActive = false;
            ui->whoWin->setText("白棋胜利！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            break;
        case GomokuLogic::Draw:
            gameActive = false;
            ui->whoWin->setText("平局！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            break;
        case GomokuLogic::InProgress:
            updateTurnLabel();
            // 不需要切换计时器，onClockTick 会按 currentPlayer 动态扣时
            break;
    }
}

// ==================== 倒计时实现 =====================
void gameWindow::initClocks() {
    // QLCDNumber 显示 mm:ss（5 位）
    ui->whiteTimer->setDigitCount(5);
    ui->blackTimer->setDigitCount(5);
    // 初始显示
    whiteRemaining = initialSeconds;
    blackRemaining = initialSeconds;
    refreshClockDisplays();

    // 每秒触发一次
    if (!turnTimer) {
        turnTimer = new QTimer(this);
        turnTimer->setInterval(1000);
        connect(turnTimer, &QTimer::timeout, this, &gameWindow::onClockTick);
    }
}

void gameWindow::startClock() {
    if (!turnTimer) return;
    if (logic.state() != GomokuLogic::InProgress) return;
    if (!turnTimer->isActive()) turnTimer->start();
}

void gameWindow::stopClock() {
    if (turnTimer && turnTimer->isActive()) turnTimer->stop();
}

void gameWindow::onClockTick() {
    if (!gameActive || logic.state() != GomokuLogic::InProgress) {
        stopClock();
        return;
    }
    // 按当前执棋方扣时
    if (logic.currentPlayer() == GomokuLogic::Black) {
        if (blackRemaining > 0) --blackRemaining;
        if (blackRemaining <= 0) {
            blackRemaining = 0;
            refreshClockDisplays();
            // 简单超时处理：停止对局并提示（不变更逻辑层 state）
            gameActive = false;
            ui->whoWin->setText("黑方超时！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            return;
        }
    } else {
        if (whiteRemaining > 0) --whiteRemaining;
        if (whiteRemaining <= 0) {
            whiteRemaining = 0;
            refreshClockDisplays();
            gameActive = false;
            ui->whoWin->setText("白方超时！");
            ui->whoTurn->setText("游戏结束");
            stopClock();
            return;
        }
    }
    refreshClockDisplays();
}

void gameWindow::refreshClockDisplays() const {
    ui->whiteTimer->display(formatMMSS(whiteRemaining));
    ui->blackTimer->display(formatMMSS(blackRemaining));
}

QString gameWindow::formatMMSS(int seconds) {
    if (seconds < 0) seconds = 0;
    const int mm = seconds / 60;
    const int ss = seconds % 60;
    return QString("%1:%2").arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0'));
}

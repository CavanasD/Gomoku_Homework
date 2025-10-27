#ifndef MY_APP_GAMEWINDOW_H
#define MY_APP_GAMEWINDOW_H

#include <QWidget>
#include <memory>
#include <QString>            // 为 formatMMSS 返回值提供声明
#include "gomokuLogic.h"
#include "aibrain.h"  // 直接包含，避免不完整类型问题

QT_BEGIN_NAMESPACE

namespace Ui {
    class gameWindow;
}

QT_END_NAMESPACE

// 窗口层（View + Controller）：
// - 负责把 UI 事件（按钮、棋盘点击）与逻辑层（GomokuLogic）对接
// - 不直接改动棋盘数组，只通过 logic.placePiece(x,y) 推进状态
// - 【如何获取棋盘信息】调用 logic.getBoard()（15x15，0空/1黑/2白），最后一步 logic.lastX()/lastY()
// - 【如何获取当前玩家】logic.currentPlayer()（Black/White）
// - 【AI 触发点/剪枝接入点】在 triggerAIMove() 中调用 aiBrain->getBestMove(board)
class gameWindow : public QWidget {
    Q_OBJECT
public:
    explicit gameWindow(QWidget *parent = nullptr);
    // 新增：支持指定是否为人机模式，以及 AI 扮演的阵营（默认白棋）
    explicit gameWindow(bool vsAI, GomokuLogic::Player aiSide = GomokuLogic::White, QWidget *parent = nullptr);
    ~gameWindow() override;

private slots:
    void on_backToMain_clicked();
    void on_startBut_clicked();
    void on_resetBut_clicked();

    // 来自 ChessBoard 的点击信号（x 行，y 列）；此处仅转发给逻辑层 placePiece
    void handleBoardClick(int x, int y);

private:
    // 仅根据当前状态刷新“轮到谁”标签（不改变模型）
    void updateTurnLabel() const;
    // AI 回合入口：从 logic.getBoard() 读取棋盘，调用 aiBrain->getBestMove()
    // 剪枝算法（如 Alpha-Beta / MCTS 的邻域裁剪）应实现于 AIBrain 的具体类中
    void triggerAIMove();

    // ============ 计时器相关（倒计时 mm:ss） ============
    void initClocks();                 // 初始化时钟显示与 QTimer
    void startClock();                 // 根据当前对局状态开始倒计时
    void stopClock();                  // 停止倒计时
    void onClockTick();                // 每秒回调，给“当前行棋方”减 1 秒
    void refreshClockDisplays() const; // 将秒数格式化为 mm:ss 显示到 QLCDNumber
    static QString formatMMSS(int seconds); // 300 -> "05:00"

    Ui::gameWindow *ui;              // UI 由 Qt Designer 生成

    GomokuLogic logic;               // 逻辑层（Model）：维护棋盘/当前玩家/胜负

    bool gameActive{false};          // 是否已经点击“开始”

    // 人机模式配置：是否 vsAI、AI 执哪方、是否在思考中（用于 UI 提示）
    bool vsAI{false};
    GomokuLogic::Player aiSide{GomokuLogic::White};
    bool aiThinking{false};

    // AI 引擎：通过统一接口解耦具体算法（MCTS、Alpha-Beta 等）
    std::unique_ptr<AIBrain> aiBrain;

    // ============ 时钟数据 ============
    class QTimer* turnTimer{nullptr}; // 每秒触发一次
    int initialSeconds{300};          // 初始时间（总秒数），UI 中原来显示 300
    int whiteRemaining{300};          // 白方剩余秒数
    int blackRemaining{300};          // 黑方剩余秒数
};

#endif //MY_APP_GAMEWINDOW_H
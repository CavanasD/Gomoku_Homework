#ifndef MY_APP_GAMEWINDOW_H
#define MY_APP_GAMEWINDOW_H

#include <QWidget>
#include <memory>
#include "gomokuLogic.h"
#include "aibrain.h"  // 直接包含，避免不完整类型问题

QT_BEGIN_NAMESPACE

namespace Ui {
    class gameWindow;
}

QT_END_NAMESPACE

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

    // New: handle click from ChessBoard
    void handleBoardClick(int x, int y);

private:
    void updateTurnLabel() const;
    // 新增：触发 AI 思考并落子
    void triggerAIMove();

    Ui::gameWindow *ui;

    // Game state manager
    GomokuLogic logic;

    bool gameActive{false};

    // 新增：人机模式与 AI 侧
    bool vsAI{false};
    GomokuLogic::Player aiSide{GomokuLogic::White};
    bool aiThinking{false};

    std::unique_ptr<AIBrain> aiBrain;
};

#endif //MY_APP_GAMEWINDOW_H
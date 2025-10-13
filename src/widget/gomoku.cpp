#include "gomoku.h"
#include "ui_Gomoku.h"
#include "ui_gamewindow.h"
#include "gameWindow.h"

// 主菜单窗口：仅负责模式选择与窗口切换（不参与规则判断）
// - 人人模式：创建不启用 AI 的 gameWindow(false)
// - 人机模式：创建启用 AI 的 gameWindow(true, AI 阵营)

Gomoku::Gomoku(QWidget *parent) : QWidget(parent), ui(new Ui::Gomoku) {
    ui->setupUi(this);
}

Gomoku::~Gomoku() {
    delete ui;
}

void Gomoku::on_vsAIBut_clicked() {
    this->close();
    // 人机模式：默认让 AI 执白（玩家执黑先手）
    auto *gw = new gameWindow(true, GomokuLogic::White);
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}
void Gomoku::on_pvpBut_clicked() {
    this->close();
    // 人人模式：关闭 AI
    auto *gw = new gameWindow(false);
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}
void Gomoku::on_exitBut_clicked() {
    QApplication::quit();
}
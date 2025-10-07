#include "gomoku.h"
#include "ui_Gomoku.h"
#include "ui_gamewindow.h"
#include "gameWindow.h"
Gomoku::Gomoku(QWidget *parent) : QWidget(parent), ui(new Ui::Gomoku) {
    ui->setupUi(this);
}

Gomoku::~Gomoku() {
    delete ui;
}

void Gomoku::on_vsAIBut_clicked() {
    this->close();
    // 人机模式：默认让 AI 扮演白棋（玩家先手）
    auto *gw = new gameWindow(true, GomokuLogic::White);
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}
void Gomoku::on_pvpBut_clicked() {
    this->close();
    // 人人模式：不启用 AI
    auto *gw = new gameWindow(false);
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}
void Gomoku::on_exitBut_clicked() {
    QApplication::quit();
}
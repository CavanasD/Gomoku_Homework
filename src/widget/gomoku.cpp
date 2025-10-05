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
    auto *gw = new gameWindow();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}
void Gomoku::on_pvpBut_clicked() {
    this->close();
    auto *gw = new gameWindow();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();
}
void Gomoku::on_exitBut_clicked() {
    QApplication::quit();
}
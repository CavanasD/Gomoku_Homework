
#include "gamewindow.h"
#include "ui_gameWindow.h"
#include "gomoku.h"
#include "ui_gomoku.h"

gameWindow::gameWindow(QWidget *parent) : QWidget(parent), ui(new Ui::gameWindow) {
    ui->setupUi(this);
}

gameWindow::~gameWindow() {
    delete ui;
}

void gameWindow::on_backToMain_clicked() {
    this->close();
    auto *gw = new Gomoku();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    gw->show();

}

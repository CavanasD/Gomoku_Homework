//
// Created by CavanasD on 2025/10/5.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Gomoku.h" resolved

#include "gomoku.h"
#include "ui_Gomoku.h"


Gomoku::Gomoku(QWidget *parent) : QWidget(parent), ui(new Ui::Gomoku) {
    ui->setupUi(this);
}

Gomoku::~Gomoku() {
    delete ui;
}
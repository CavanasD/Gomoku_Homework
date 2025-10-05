//
// Created by CavanasD on 2025/10/5.
//

#ifndef MY_APP_GOMOKU_H
#define MY_APP_GOMOKU_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class Gomoku;
}

QT_END_NAMESPACE

class Gomoku : public QWidget {
    Q_OBJECT

public:
    explicit Gomoku(QWidget *parent = nullptr);

    ~Gomoku() override;

private slots:   // 槽函数声明区
    void on_vsAIBut_clicked();
    void on_pvpBut_clicked();
    static void on_exitBut_clicked();
private:
    Ui::Gomoku *ui;
};


#endif //MY_APP_GOMOKU_H
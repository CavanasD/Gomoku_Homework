#ifndef MY_APP_GAMEWINDOW_H
#define MY_APP_GAMEWINDOW_H

#include <QWidget>
#include <QMouseEvent>


QT_BEGIN_NAMESPACE

namespace Ui {
    class gameWindow;
}

QT_END_NAMESPACE

class gameWindow : public QWidget {
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *Event) override;
    void mouseMoveEvent(QMouseEvent *Event) override;
    void mousePressEvent(QMouseEvent *Event) override;
    void updateTurnLabel() const;
    bool checkWin(int x, int y);

public:
    explicit gameWindow(QWidget *parent = nullptr);

    ~gameWindow() override;
private slots:
    void on_backToMain_clicked();
    void on_startBut_clicked();
    void on_resetBut_clicked();


private:
    Ui::gameWindow *ui;

    // 成员变量
    int chess[15][15]{};
    bool isPlayerOneTurn;
    bool gameActive;
    int moveX, moveY{};
    int lastMoveX{}, lastMoveY{};
};


#endif //MY_APP_GAMEWINDOW_H
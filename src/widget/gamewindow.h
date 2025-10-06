#ifndef MY_APP_GAMEWINDOW_H
#define MY_APP_GAMEWINDOW_H

#include <QWidget>
#include "gomokuLogic.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class gameWindow;
}

QT_END_NAMESPACE

class gameWindow : public QWidget {
    Q_OBJECT
public:
    explicit gameWindow(QWidget *parent = nullptr);
    ~gameWindow() override;

private slots:
    void on_backToMain_clicked();
    void on_startBut_clicked();
    void on_resetBut_clicked();

    // New: handle click from ChessBoard
    void handleBoardClick(int x, int y);

private:
    void updateTurnLabel() const;

    Ui::gameWindow *ui;

    // Game state manager
    GomokuLogic logic;

    bool gameActive{false};
};

#endif //MY_APP_GAMEWINDOW_H
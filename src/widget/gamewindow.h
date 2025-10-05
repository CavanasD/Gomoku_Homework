#ifndef MY_APP_GAMEWINDOW_H
#define MY_APP_GAMEWINDOW_H

#include <QWidget>


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
private:
    Ui::gameWindow *ui;
};


#endif //MY_APP_GAMEWINDOW_H
#include <QApplication>
#include "widget/gomoku.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Gomoku w;   // 创建自定义窗口
    w.show();   // 显示它

    return QApplication::exec();
}

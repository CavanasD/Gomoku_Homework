/********************************************************************************
** Form generated from reading UI file 'gamewindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAMEWINDOW_H
#define UI_GAMEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "chessboard.h"

QT_BEGIN_NAMESPACE

class Ui_gameWindow
{
public:
    QPushButton *backToMain;
    QPushButton *startBut;
    QPushButton *resetBut;
    QLabel *label;
    QLabel *whoTurn;
    QLabel *whoWin;
    ChessBoard *chessBoardWidget;
    QLabel *label_2;
    QLabel *label_3;
    QLCDNumber *whiteTimer;
    QLCDNumber *blackTimer;

    void setupUi(QWidget *gameWindow)
    {
        if (gameWindow->objectName().isEmpty())
            gameWindow->setObjectName("gameWindow");
        gameWindow->resize(786, 704);
        backToMain = new QPushButton(gameWindow);
        backToMain->setObjectName("backToMain");
        backToMain->setGeometry(QRect(640, 20, 111, 31));
        startBut = new QPushButton(gameWindow);
        startBut->setObjectName("startBut");
        startBut->setGeometry(QRect(640, 60, 111, 31));
        resetBut = new QPushButton(gameWindow);
        resetBut->setObjectName("resetBut");
        resetBut->setGeometry(QRect(640, 100, 111, 31));
        label = new QLabel(gameWindow);
        label->setObjectName("label");
        label->setGeometry(QRect(620, 150, 141, 51));
        QFont font;
        font.setFamilies({QString::fromUtf8("\346\261\211\344\273\252\351\273\221\347\245\236\350\257\235")});
        font.setPointSize(24);
        label->setFont(font);
        whoTurn = new QLabel(gameWindow);
        whoTurn->setObjectName("whoTurn");
        whoTurn->setGeometry(QRect(630, 210, 141, 51));
        whoTurn->setFont(font);
        whoWin = new QLabel(gameWindow);
        whoWin->setObjectName("whoWin");
        whoWin->setGeometry(QRect(280, 600, 291, 51));
        whoWin->setFont(font);
        chessBoardWidget = new ChessBoard(gameWindow);
        chessBoardWidget->setObjectName("chessBoardWidget");
        chessBoardWidget->setGeometry(QRect(20, 10, 571, 571));
        label_2 = new QLabel(gameWindow);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(620, 270, 141, 51));
        label_2->setFont(font);
        label_3 = new QLabel(gameWindow);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(620, 410, 141, 51));
        label_3->setFont(font);
        whiteTimer = new QLCDNumber(gameWindow);
        whiteTimer->setObjectName("whiteTimer");
        whiteTimer->setGeometry(QRect(620, 470, 151, 61));
        whiteTimer->setMidLineWidth(1);
        whiteTimer->setDigitCount(3);
        whiteTimer->setMode(QLCDNumber::Mode::Dec);
        whiteTimer->setProperty("value", QVariant(300.000000000000000));
        whiteTimer->setProperty("intValue", QVariant(300));
        blackTimer = new QLCDNumber(gameWindow);
        blackTimer->setObjectName("blackTimer");
        blackTimer->setGeometry(QRect(620, 340, 151, 61));
        blackTimer->setMidLineWidth(1);
        blackTimer->setDigitCount(3);
        blackTimer->setMode(QLCDNumber::Mode::Dec);
        blackTimer->setSegmentStyle(QLCDNumber::SegmentStyle::Filled);
        blackTimer->setProperty("value", QVariant(300.000000000000000));
        blackTimer->setProperty("intValue", QVariant(300));

        retranslateUi(gameWindow);

        QMetaObject::connectSlotsByName(gameWindow);
    } // setupUi

    void retranslateUi(QWidget *gameWindow)
    {
        gameWindow->setWindowTitle(QCoreApplication::translate("gameWindow", "gameWindow", nullptr));
        backToMain->setText(QCoreApplication::translate("gameWindow", "\303\227", nullptr));
        startBut->setText(QCoreApplication::translate("gameWindow", "\345\274\200\345\247\213\346\270\270\346\210\217", nullptr));
        resetBut->setText(QCoreApplication::translate("gameWindow", "\351\207\215\347\275\256\346\243\213\347\233\230", nullptr));
        label->setText(QCoreApplication::translate("gameWindow", "\345\275\223\345\211\215\345\233\236\345\220\210\357\274\232", nullptr));
        whoTurn->setText(QCoreApplication::translate("gameWindow", "\351\273\221\346\243\213", nullptr));
        whoWin->setText(QCoreApplication::translate("gameWindow", "\344\275\240\350\203\275\346\211\223\350\264\245\346\265\201\350\220\244\345\220\227", nullptr));
        label_2->setText(QCoreApplication::translate("gameWindow", "\345\200\222\350\256\241\346\227\266", nullptr));
        label_3->setText(QCoreApplication::translate("gameWindow", "\345\200\222\350\256\241\346\227\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class gameWindow: public Ui_gameWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEWINDOW_H

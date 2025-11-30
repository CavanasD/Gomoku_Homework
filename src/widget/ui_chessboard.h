/********************************************************************************
** Form generated from reading UI file 'chessboard.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHESSBOARD_H
#define UI_CHESSBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_chessBoard
{
public:

    void setupUi(QWidget *chessBoard)
    {
        if (chessBoard->objectName().isEmpty())
            chessBoard->setObjectName("chessBoard");
        chessBoard->resize(400, 300);

        retranslateUi(chessBoard);

        QMetaObject::connectSlotsByName(chessBoard);
    } // setupUi

    void retranslateUi(QWidget *chessBoard)
    {
        chessBoard->setWindowTitle(QCoreApplication::translate("chessBoard", "chessBoard", nullptr));
    } // retranslateUi

};

namespace Ui {
    class chessBoard: public Ui_chessBoard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHESSBOARD_H

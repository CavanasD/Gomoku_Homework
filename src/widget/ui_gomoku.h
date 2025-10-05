/********************************************************************************
** Form generated from reading UI file 'gomoku.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GOMOKU_H
#define UI_GOMOKU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Gomoku
{
public:
    QLabel *label;
    QPushButton *vsAIBut;
    QLabel *label_2;
    QPushButton *pvpBut;
    QPushButton *exitBut;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;

    void setupUi(QWidget *Gomoku)
    {
        if (Gomoku->objectName().isEmpty())
            Gomoku->setObjectName("Gomoku");
        Gomoku->resize(881, 610);
        label = new QLabel(Gomoku);
        label->setObjectName("label");
        label->setGeometry(QRect(-10, 0, 921, 621));
        label->setPixmap(QPixmap(QString::fromUtf8(":/img/test.png")));
        label->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing);
        vsAIBut = new QPushButton(Gomoku);
        vsAIBut->setObjectName("vsAIBut");
        vsAIBut->setGeometry(QRect(20, 490, 251, 101));
        QFont font;
        font.setFamilies({QString::fromUtf8("\346\261\211\344\273\252\345\233\275\345\233\276\345\210\233\346\226\260\347\272\242\346\245\274\346\242\246")});
        font.setPointSize(22);
        vsAIBut->setFont(font);
        label_2 = new QLabel(Gomoku);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 440, 221, 31));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\346\261\211\344\273\252\345\233\275\345\233\276\345\210\233\346\226\260\347\272\242\346\245\274\346\242\246")});
        label_2->setFont(font1);
        pvpBut = new QPushButton(Gomoku);
        pvpBut->setObjectName("pvpBut");
        pvpBut->setGeometry(QRect(320, 490, 251, 101));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\346\261\211\344\273\252\345\233\275\345\233\276\345\210\233\346\226\260\347\272\242\346\245\274\346\242\246")});
        font2.setPointSize(18);
        pvpBut->setFont(font2);
        exitBut = new QPushButton(Gomoku);
        exitBut->setObjectName("exitBut");
        exitBut->setGeometry(QRect(600, 490, 251, 101));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\346\261\211\344\273\252\351\273\221\347\245\236\350\257\235")});
        font3.setPointSize(24);
        exitBut->setFont(font3);
        label_3 = new QLabel(Gomoku);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(280, 40, 261, 51));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\346\261\211\344\273\252\345\233\275\345\233\276\345\210\233\346\226\260\347\272\242\346\245\274\346\242\246")});
        font4.setPointSize(24);
        label_3->setFont(font4);
        label_4 = new QLabel(Gomoku);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(280, 120, 221, 31));
        label_4->setFont(font1);
        label_5 = new QLabel(Gomoku);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(280, 170, 221, 31));
        label_5->setFont(font1);

        retranslateUi(Gomoku);

        QMetaObject::connectSlotsByName(Gomoku);
    } // setupUi

    void retranslateUi(QWidget *Gomoku)
    {
        Gomoku->setWindowTitle(QCoreApplication::translate("Gomoku", "Gomoku", nullptr));
        label->setText(QString());
        vsAIBut->setText(QCoreApplication::translate("Gomoku", "\346\210\221\350\246\201\345\257\271\346\210\230\346\265\201\350\220\244", nullptr));
        label_2->setText(QCoreApplication::translate("Gomoku", "<html><head/><body><p><span style=\" font-size:18pt; font-weight:700;\">\344\275\240\350\246\201\347\216\251\344\273\200\344\271\210\357\274\237</span></p></body></html>", nullptr));
        pvpBut->setText(QCoreApplication::translate("Gomoku", "\346\210\221\346\230\257\350\231\232b\357\274\214\345\217\252\350\203\275\350\207\252\345\267\261\346\211\223", nullptr));
        exitBut->setText(QCoreApplication::translate("Gomoku", "\344\270\215\347\216\251\344\272\206", nullptr));
        label_3->setText(QCoreApplication::translate("Gomoku", "\344\271\235\351\270\237\347\232\204\344\272\224\345\255\220\346\243\213", nullptr));
        label_4->setText(QCoreApplication::translate("Gomoku", "<html><head/><body><p><span style=\" font-size:18pt; font-weight:700;\">\344\275\240\350\203\275\346\211\223\350\264\245\346\265\201\350\220\244\351\205\261\345\220\227\357\274\237</span></p></body></html>", nullptr));
        label_5->setText(QCoreApplication::translate("Gomoku", "<html><head/><body><p><span style=\" font-size:18pt; font-weight:700;\">\346\265\201\350\220\244\351\205\261\345\217\257\347\210\261\345\226\265</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Gomoku: public Ui_Gomoku {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GOMOKU_H

/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon 20. Jan 21:31:56 2014
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QGraphicsView *graphicsView;
    QLineEdit *lineEdit;
    QPushButton *pushButton_2;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton_3;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *spinBox;
    QLabel *label_5;
    QSpinBox *spinBox_2;
    QLabel *label_6;
    QLabel *label_7;
    QSpinBox *spinBox_3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(763, 1019);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(550, 30, 81, 23));
        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(10, 130, 741, 831));
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(10, 30, 531, 20));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setEnabled(false);
        pushButton_2->setGeometry(QRect(400, 79, 75, 23));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 641, 16));
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 110, 741, 16));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setEnabled(false);
        pushButton_3->setGeometry(QRect(480, 79, 151, 23));
        pushButton_3->setCheckable(false);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 80, 331, 20));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(380, 79, 21, 20));
        spinBox = new QSpinBox(centralWidget);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setEnabled(false);
        spinBox->setGeometry(QRect(330, 80, 42, 21));
        spinBox->setMinimum(1);
        spinBox->setMaximum(1000);
        spinBox->setSingleStep(1);
        spinBox->setValue(55);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 60, 271, 16));
        spinBox_2 = new QSpinBox(centralWidget);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setEnabled(false);
        spinBox_2->setGeometry(QRect(330, 55, 42, 21));
        spinBox_2->setMaximum(999);
        spinBox_2->setValue(1);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(310, 60, 21, 16));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(380, 60, 21, 16));
        spinBox_3 = new QSpinBox(centralWidget);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));
        spinBox_3->setEnabled(false);
        spinBox_3->setGeometry(QRect(400, 55, 42, 21));
        spinBox_3->setMaximum(999);
        spinBox_3->setValue(4);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 763, 20));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(pushButton, SIGNAL(clicked()), MainWindow, SLOT(getPath()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), MainWindow, SLOT(generuj()));
        QObject::connect(pushButton_3, SIGNAL(clicked()), MainWindow, SLOT(derivuj()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Preh\304\276adaj", 0, QApplication::UnicodeUTF8));
        lineEdit->setText(QApplication::translate("MainWindow", "Vlo\305\276te adresu s\303\272boru s PDBP", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainWindow", "Generuj PTS", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "PrimTroS", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        pushButton_3->setText(QApplication::translate("MainWindow", "Zisti parci\303\241lne deriv\303\241cie", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Parameter reprezentat\303\255vnosti: maxim\303\241lna d\304\272\305\276ka strany trojuholn\303\255ka", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "m", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "\304\214\303\255sla bodov tvoriacich prv\303\272 stranu prv\303\251ho trojuholn\303\255ka:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "I =", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "J =", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

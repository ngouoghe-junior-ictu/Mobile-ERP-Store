/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QMenuBar *menubar;
    QMenu *menuERP;
    QMenu *menuDEPARTMENTS_AND_JOBS;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::WindowModal);
        MainWindow->resize(800, 600);
        MainWindow->setTabShape(QTabWidget::Rounded);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 10, 561, 391));
        label->setContextMenuPolicy(Qt::NoContextMenu);
        label->setToolTipDuration(-2);
        label->setStyleSheet(QString::fromUtf8(""));
        label->setFrameShape(QFrame::StyledPanel);
        label->setLineWidth(-2);
        label->setMidLineWidth(-3);
        label->setPixmap(QPixmap(QString::fromUtf8(":/Assets/IMG-20221128-WA0052.jpg")));
        label->setMargin(-1);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(240, 80, 181, 41));
        pushButton->setStyleSheet(QString::fromUtf8("#pushButton {\n"
"border: 2px solid gray;\n"
"border-radius: 10px;\n"
"padding: 0 8px;\n"
"background: gray;\n"
"selection-background-color: darkgray;\n"
"}"));
        pushButton->setIconSize(QSize(18, 16));
        pushButton->setAutoRepeatDelay(298);
        pushButton->setAutoRepeatInterval(96);
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(240, 180, 181, 41));
        pushButton_2->setToolTipDuration(1);
        pushButton_2->setStyleSheet(QString::fromUtf8("#pushButton_2 {\n"
"border: 2px solid gray;\n"
"border-radius: 10px;\n"
"padding: 0 8px;\n"
"background: gray;\n"
"selection-background-color: darkgray;\n"
"}"));
        pushButton_2->setIconSize(QSize(18, 16));
        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(240, 290, 181, 41));
        pushButton_3->setStyleSheet(QString::fromUtf8("#pushButton_3 {\n"
"border: 2px solid gray;\n"
"border-radius: 10px;\n"
"padding: 0 8px;\n"
"background: gray;\n"
"selection-background-color: darkgray;\n"
"}"));
        pushButton_3->setIconSize(QSize(16, 19));
        pushButton_3->setAutoRepeatInterval(102);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuERP = new QMenu(menubar);
        menuERP->setObjectName(QString::fromUtf8("menuERP"));
        menuDEPARTMENTS_AND_JOBS = new QMenu(menubar);
        menuDEPARTMENTS_AND_JOBS->setObjectName(QString::fromUtf8("menuDEPARTMENTS_AND_JOBS"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuERP->menuAction());
        menubar->addAction(menuDEPARTMENTS_AND_JOBS->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QString());
        pushButton->setText(QApplication::translate("MainWindow", "Deparment", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "Jobs", nullptr));
        pushButton_3->setText(QApplication::translate("MainWindow", "Shifts", nullptr));
        menuERP->setTitle(QApplication::translate("MainWindow", "ERP", nullptr));
        menuDEPARTMENTS_AND_JOBS->setTitle(QApplication::translate("MainWindow", "DEPARTMENTS AND JOBS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

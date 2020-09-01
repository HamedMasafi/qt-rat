#include "mainwindow.h"
#include "inputmanager.h"

#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButtonDragMouse_clicked()
{
    auto startPoint = textEdit->mapToGlobal(QPoint(40, 20));
//    QThread::sleep(1);
    INPUTMANAGER->moveMouse(startPoint, Qt::NoButton);
    INPUTMANAGER->pressMouse(startPoint, Qt::LeftButton);

    startPoint.setX(startPoint.x() + 50);
    startPoint.setY(startPoint.y() + 50);
    INPUTMANAGER->moveMouse(startPoint, Qt::NoButton);
    INPUTMANAGER->releaseMouse(startPoint, Qt::LeftButton);
}

void MainWindow::on_pushButtonRightClick_clicked()
{
    auto startPoint = textEdit->mapToGlobal(QPoint(20, 5));
    INPUTMANAGER->moveMouse(startPoint, Qt::NoButton);
    INPUTMANAGER->pressMouse(startPoint, Qt::RightButton);
}

void MainWindow::on_pushButtonKeys_clicked()
{
    textEdit->clear();
    textEdit->setFocus();

    QThread::sleep(1);

    INPUTMANAGER->pressKey(Qt::Key_H);
    INPUTMANAGER->pressKey(Qt::Key_E);
    INPUTMANAGER->pressKey(Qt::Key_L);
    INPUTMANAGER->pressKey(Qt::Key_L);
    INPUTMANAGER->pressKey(Qt::Key_O);
    INPUTMANAGER->pressKey(Qt::Key_Semicolon);
    INPUTMANAGER->pressKey(Qt::Key_Colon);
    INPUTMANAGER->pressKey(Qt::Key_Backspace);

    INPUTMANAGER->pressKey(Qt::Key_Enter);
    INPUTMANAGER->pressKey(Qt::Key_CapsLock);

    INPUTMANAGER->pressKey(Qt::Key_W);
    INPUTMANAGER->pressKey(Qt::Key_O);
    INPUTMANAGER->pressKey(Qt::Key_R, Qt::ShiftModifier);
    INPUTMANAGER->pressKey(Qt::Key_L);
    INPUTMANAGER->pressKey(Qt::Key_D, Qt::ShiftModifier);

    INPUTMANAGER->pressKey(Qt::Key_CapsLock);

}

#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QString>
#include <QVector>
#include <QtWidgets/QPlainTextEdit>

class QConsole : public QPlainTextEdit
{
    Q_OBJECT

public: // methods
    QConsole(QWidget *parent = nullptr);

    void setPrompt(const QString &prompt);
    void printOutput(const QString &text);
    void printError(const QString &text);

    void lock();
    void unlock();

    void write(const QString &str);
    QConsole &operator<<(const QString &str);

    void insertBlock();
    void removeBlock();
    void eraseBlock();

    void insertPrompt();

    void setBaseColor(const QColor &color);
    void setTextColor(const QColor &color);

protected: // methods
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

private: // methods
    void selectBlock();
    int promptSize() const;
    void onReturn();
    void historyAdd(const QString &command);
    void historyBack();
    void historyForward();
    void scrollDown();

signals:
    void onCommand(const QString &command);

private: // fields

    QVector<QString> m_history;
    QString m_prompt;

    int m_history_pos;
    bool m_locked;
};

#endif // CONSOLEWIDGET_H

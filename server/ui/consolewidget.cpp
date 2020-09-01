#include "consolewidget.h"

#include <QTextBlock>
#include <QScrollBar>

QConsole::QConsole(QWidget *parent)
    : QPlainTextEdit{parent}
    , m_history_pos{0}
    , m_locked{false}
{
    setBaseColor(QColor(40, 40, 40));
    setTextColor(QColor(230, 230, 230));

    setFrameShape(QFrame::NoFrame);
    viewport()->setCursor(Qt::ArrowCursor);

    setWordWrapMode(QTextOption::WrapAnywhere);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCursorWidth(2);
}

void QConsole::setPrompt(const QString &prompt)
{
    m_prompt = prompt;
    insertPrompt();
}

void QConsole::lock()
{
    m_locked = true;
}

void QConsole::printOutput(const QString &text)
{
    if (text.endsWith("\n"))
        write(text);
    else
        write(text+ "\n");
    insertPrompt();
}

void QConsole::printError(const QString &text)
{
    if (text.endsWith("\n"))
        write(text);
    else
        write(text+ "\n");
    insertPrompt();
}

void QConsole::unlock()
{
    m_locked = false;
}

void QConsole::write(const QString &str)
{
    insertPlainText(str);
    scrollDown();
}

QConsole &QConsole::operator<<(const QString &str)
{
    write(str);
    return *this;
}

void QConsole::insertBlock()
{
    textCursor().insertBlock();
}

void QConsole::removeBlock()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.removeSelectedText();
}

void QConsole::eraseBlock()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
}

void QConsole::insertPrompt()
{
    insertPlainText(m_prompt);
    scrollDown();
    m_locked = false;
}

void QConsole::setBaseColor(const QColor &c)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, c);
    setPalette(p);
}

void QConsole::setTextColor(const QColor &c)
{
    QPalette p = palette();
    p.setColor(QPalette::Text, c);
    setPalette(p);
}

void QConsole::keyPressEvent(QKeyEvent *e)
{
    if (m_locked)
    {
        return;
    }

    if (e->modifiers() == Qt::ControlModifier)
    {
        if (e->key() == Qt::Key_C || e->key() == Qt::Key_V ||
            e->key() == Qt::Key_X)
        {
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
        else if (e->key() == Qt::Key_A)
        {
            selectBlock();
            return;
        }
    }

    if (e->key() >= Qt::Key_Space && e->key() <= Qt::Key_AsciiTilde &&
        e->key() != Qt::Key_QuoteLeft)
    {
        if (e->modifiers() == Qt::NoModifier ||
            e->modifiers() == Qt::ShiftModifier)
        {
            QPlainTextEdit::keyPressEvent(e);
        }
    }
    else
    {
        switch (e->key())
        {
        case Qt::Key_Return:
            onReturn();
            break;

        case Qt::Key_Up:
            historyBack();
            break;
        case Qt::Key_Down:
            historyForward();
            break;

        case Qt::Key_Backspace:
            if (textCursor().positionInBlock() > promptSize() ||
                textCursor().hasSelection())
            {
                QPlainTextEdit::keyPressEvent(e);
            }
            break;

        case Qt::Key_End:
        case Qt::Key_Delete:
            QPlainTextEdit::keyPressEvent(e);
            break;

        case Qt::Key_Left:
        {
            QPlainTextEdit::keyPressEvent(e);

            QTextCursor cursor = textCursor();

            if (cursor.positionInBlock() < promptSize())
            {
                QTextCursor::MoveMode move_op = cursor.hasSelection()
                                                    ? QTextCursor::KeepAnchor
                                                    : QTextCursor::MoveAnchor;
                cursor.movePosition(QTextCursor::StartOfBlock, move_op);
                cursor.movePosition(QTextCursor::Right, move_op, promptSize());
            }

            setTextCursor(cursor);
        }
        break;

        case Qt::Key_Right:
            QPlainTextEdit::keyPressEvent(e);
            break;

        case Qt::Key_Home:
        {
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(
                QTextCursor::Right, QTextCursor::MoveAnchor, promptSize());
            setTextCursor(cursor);
        }
        break;

        case Qt::Key_PageUp:
        {
            QScrollBar *vbar = verticalScrollBar();
            vbar->setValue(vbar->value() - 20);
        }
        break;

        case Qt::Key_PageDown:
        {
            QScrollBar *vbar = verticalScrollBar();
            vbar->setValue(vbar->value() + 20);
        }
        break;

        default:
            QWidget::keyPressEvent(e);
        }
    }
}

void QConsole::mousePressEvent(QMouseEvent *)
{
    setFocus();
}

void QConsole::mouseDoubleClickEvent(QMouseEvent *)
{
}

void QConsole::contextMenuEvent(QContextMenuEvent *)
{
}

void QConsole::selectBlock()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(
        QTextCursor::Right, QTextCursor::MoveAnchor, promptSize());
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

int QConsole::promptSize() const
{
    return m_prompt.size();
}

void QConsole::onReturn()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::EndOfBlock);
    setTextCursor(cursor);

    QString command = cursor.block().text().mid(promptSize());
    historyAdd(command);

    insertBlock();

    emit onCommand(command);

//    insertPrompt();
}

void QConsole::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void QConsole::historyAdd(const QString &command)
{
    if (!m_history.empty() && m_history.back() == command)
    {
        return;
    }

    m_history.append(command);
    m_history_pos = m_history.size();
}

void QConsole::historyBack()
{
    if (!m_history_pos)
    {
        return;
    }

    eraseBlock();

    insertPlainText((m_prompt + m_history.at(m_history_pos - 1)));

    m_history_pos--;
}

void QConsole::historyForward()
{
    if (m_history_pos == m_history.size())
    {
        return;
    }

    eraseBlock();

    if (m_history_pos == m_history.size() - 1)
    {
        insertPlainText(m_prompt);
    }
    else
    {
        insertPlainText((m_prompt + m_history.at(m_history_pos + 1)));
    }

    m_history_pos++;
}

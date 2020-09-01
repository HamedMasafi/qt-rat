#ifndef ANTIDEBUGGERDIALOG_H
#define ANTIDEBUGGERDIALOG_H

#include "ui_antidebuggerdialog.h"
#include "../core/controllerbase.h"

class AntiDebuggerDialog : public ControllerBase, private Ui::AntiDebuggerDialog
{
Q_OBJECT

public:
explicit AntiDebuggerDialog(AbstractClient *client, QWidget *parent = nullptr);
};

#endif // ANTIDEBUGGERDIALOG_H

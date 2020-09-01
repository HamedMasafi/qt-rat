#ifndef WEBCAMVIEWDIALOG_H
#define WEBCAMVIEWDIALOG_H

#include "ui_webcamviewdialog.h"
#include "../core/controllerbase.h"

class WebcamViewDialog : public ControllerBase, private Ui::WebcamViewDialog
{
    Q_OBJECT
    bool closing;
    bool capturing;

public:
    explicit WebcamViewDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);

private slots:
    void startCapturing();

    // QWidget interface
    void on_pushButtonStart_clicked();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // WEBCAMVIEWDIALOG_H

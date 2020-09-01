#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"

#include <QJsonObject>

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT
    QString binPath;
    QString outputPath;

public:
    explicit Dialog(QWidget *parent = nullptr);

private:
    bool checkFiles() const;

private slots:
    void updatePath();
    void on_comboBoxPlatform_activated(const QString &arg1);
    void on_comboBoxArch_activated(const QString &arg1);

    void copyDir(const QString &from, const QString &to);
    void on_pushButtonBuild_clicked();
};

#endif // DIALOG_H

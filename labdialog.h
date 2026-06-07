#ifndef LABDIALOG_H
#define LABDIALOG_H

#include <QDialog>

namespace Ui {
class LabDialog;
}

class LabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabDialog(QWidget *parent = nullptr);
    ~LabDialog();

private slots:
    void on_close_btn_clicked();

public:
    Ui::LabDialog *ui;
};

#endif // LABDIALOG_H

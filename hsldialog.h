#ifndef HSLDIALOG_H
#define HSLDIALOG_H

#include <QDialog>

namespace Ui {
class HslDialog;
}

class HslDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HslDialog(QWidget *parent = nullptr);
    ~HslDialog();

public:
    Ui::HslDialog *ui;
};

#endif // HSLDIALOG_H

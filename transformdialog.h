#ifndef TRANSFORMDIALOG_H
#define TRANSFORMDIALOG_H

#include <QDialog>

namespace Ui {
class TransformDialog;
}

class TransformDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransformDialog(QWidget *parent = nullptr);
    ~TransformDialog();

public:
    Ui::TransformDialog *ui;
};

#endif // TRANSFORMDIALOG_H

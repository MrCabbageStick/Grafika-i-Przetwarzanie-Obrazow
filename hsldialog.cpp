#include "hsldialog.h"
#include "ui_hsldialog.h"
#include <QSlider>
#include <QPushButton>

HslDialog::HslDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HslDialog)
{
    ui->setupUi(this);

    connect(ui->close_btn, &QPushButton::clicked,
            this, &HslDialog::close);
}

HslDialog::~HslDialog()
{
    delete ui;
}

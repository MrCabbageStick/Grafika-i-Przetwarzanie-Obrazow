#include "labdialog.h"
#include "ui_labdialog.h"

LabDialog::LabDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LabDialog)
{
    ui->setupUi(this);

    connect(ui->close_btn, &QPushButton::clicked,
            this, &LabDialog::close);
}

LabDialog::~LabDialog()
{
    delete ui;
}

void LabDialog::on_close_btn_clicked()
{
    this->close();
}


#include "transformdialog.h"
#include "ui_transformdialog.h"

TransformDialog::TransformDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TransformDialog)
{
    ui->setupUi(this);

    connect(ui->degreeSlider, &QSlider::valueChanged, this, [this](int value){
        std::string text = std::to_string(value) + "°";
        this->ui->degreeLabel->setText(text.data());
    });
}

TransformDialog::~TransformDialog()
{
    delete ui;
}

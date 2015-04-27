#include "signin.h"
#include "ui_signin.h"

signin::signin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signin)
{
    ui->setupUi(this);

    male_ico = "/home/shuang/project/tippi/final/demo/male.ico";
    female_ico = "/home/shuang/project/tippi/final/demo/female.ico";

    QImage* img = new QImage(male_ico);
    male_img = img->scaled(ui->logo->width(), ui->logo->height());
    delete img;
    img = new QImage(female_ico);
    female_img = img->scaled(ui->logo->width(), ui->logo->height());
    delete img;

    ui->logo->setPixmap(QPixmap::fromImage(male_img));
    ui->radioButtonMale->setChecked(true);
}

signin::~signin()
{
    delete ui;
}

void signin::on_pushButton_3_clicked()
{
    close();
}

void signin::on_radioButtonMale_clicked()
{
    ui->logo->setPixmap(QPixmap::fromImage(male_img));
}

void signin::on_radioButtonFemale_clicked()
{
    ui->logo->setPixmap(QPixmap::fromImage(female_img));
}

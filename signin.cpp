#include "signin.h"
#include "ui_signin.h"
#include "progressbar.h"
#include <QMessageBox>
using std::pair;

signin::signin(QWidget *parent, int numSeconds, UserMap *usernameMap, string trainFile1, string trainFile2) :
    QDialog(parent),
    ui(new Ui::signin),
    numSeconds(numSeconds),
    usernameMap(usernameMap),
    trainFile1(trainFile1),
    trainFile2(trainFile2)
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

void signin::on_radioButtonMale_clicked()
{
    ui->logo->setPixmap(QPixmap::fromImage(male_img));
}

void signin::on_radioButtonFemale_clicked()
{
    ui->logo->setPixmap(QPixmap::fromImage(female_img));
}

void signin::on_cancelButton_clicked()
{
    close();
}

void signin::on_doneButton_clicked()
{
    string username = ui->lineEdit->text().toStdString();
    Gender gender;
    if (ui->radioButtonMale->isChecked()) {
        gender = male;
    } else if (ui->radioButtonFemale->isChecked()){
        gender = female;
    } else {
        QMessageBox::information(this, "Info", "Please choose your gender.");
        return;
    }
    if (username == "") {
        QMessageBox::information(this, "Info", "Please input username first.");
        return;
    } else if (usernameMap->find(username) != usernameMap->end()) {
        QMessageBox::information(this, "Info", "Username occupied. Please choose another.");
        return;
    } else {
        usernameMap->insert(pair<string, Gender>(username,gender));
        close();
    }
}

void signin::on_recordAgainButton_clicked()
{
    progressbar recordprogress(this, numSeconds, trainFile1);
    recordprogress.setModal(true);
    recordprogress.exec();
}

void signin::on_recordButton_clicked()
{
    progressbar recordprogress(this, numSeconds, trainFile2);
    recordprogress.setModal(true);
    recordprogress.exec();
}

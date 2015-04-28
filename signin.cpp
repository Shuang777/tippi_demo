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
    trainFile2(trainFile2),
    trainFile1Passed(false),
    trainFile2Passed(false)
{
    ui->setupUi(this);

    ui->checkBox->setFixedSize(21,21);
    ui->checkBox2->setFixedSize(21,21);
    ui->checkBox3->setFixedSize(21,21);

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

    ui->checkBox->setChecked(false);
    ui->checkBox2->setChecked(false);
    ui->checkBox3->setChecked(false);


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
        QMessageBox::information(this, "Info", "Please input username.");
        return;
    } else if (usernameMap->find(username) != usernameMap->end()) {
        QMessageBox::information(this, "Info", "Username occupied. Please choose another.");
        return;
    }

    if (!trainFile1Passed) {
        QMessageBox::information(this, "Info", "Passphrase recording not qualified.");
        return;
    }
    if (!trainFile2Passed) {
        QMessageBox::information(this, "Info", "The 2nd passphrase recording not qualified.");
    }

    Enroll(username, gender);

    close();
}

void signin::Enroll(string username, Gender gender) {
    usernameMap->insert(pair<string, Gender>(username,gender));
}

void signin::on_recordButton_clicked()
{
    progressbar recordprogress(this, numSeconds, trainFile1);
    recordprogress.setModal(true);
    recordprogress.exec();
    trainFile1Passed = CheckRecording(trainFile1);
    if (trainFile1Passed) {
        ui->checkBox->setChecked(true);
    } else {
        ui->checkBox->setChecked(false);
    }
    ui->checkBox->repaint();
}

void signin::on_recordAgainButton_clicked()
{
    progressbar recordprogress(this, numSeconds, trainFile2);
    recordprogress.setModal(true);
    recordprogress.exec();
    trainFile2Passed = CheckRecording(trainFile2);
    if (trainFile2Passed) {
        ui->checkBox2->setChecked(true);
    } else {
        ui->checkBox2->setChecked(false);
    }
    ui->checkBox2->repaint();
}

bool signin::CheckRecording(string trainFile)
{
    return true;
}

void signin::on_lineEdit_returnPressed()
{

}

void signin::on_lineEdit_editingFinished()
{
    string username = ui->lineEdit->text().toStdString();
    if (username != "" && usernameMap->find(username) == usernameMap->end()) {
        ui->checkBox3->setChecked(true);
    } else {
        ui->checkBox3->setChecked(false);
    }
    ui->checkBox3->repaint();
}

void signin::on_checkBox3_clicked()
{
    ui->checkBox3->setChecked(!ui->checkBox3->isChecked());
}

void signin::on_checkBox_clicked()
{
    ui->checkBox->setChecked(!ui->checkBox->isChecked());
}

void signin::on_checkBox2_clicked()
{
    ui->checkBox2->setChecked(!ui->checkBox2->isChecked());
}

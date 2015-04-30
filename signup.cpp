#include "signup.h"
#include "ui_signup.h"
#include "progressbar.h"
#include <QMessageBox>
using std::pair;

signup::signup(QWidget *parent, int milSeconds, UserMap *usernameMap, string trainFile1, string trainFile2,
               IvectorExtraction *ivectorExtraction, bool skipRecording) :
    QDialog(parent),
    ui(new Ui::signup),
    milSeconds(milSeconds),
    usernameMap(usernameMap),
    trainFile1(trainFile1),
    trainFile2(trainFile2),
    ivectorExtraction(ivectorExtraction),
    skipRecording(skipRecording)
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

signup::~signup()
{
    delete ui;
}

void signup::on_radioButtonMale_clicked()
{
    ui->logo->setPixmap(QPixmap::fromImage(male_img));
}

void signup::on_radioButtonFemale_clicked()
{
    ui->logo->setPixmap(QPixmap::fromImage(female_img));
}

void signup::on_cancelButton_clicked()
{
    close();
}

void signup::on_doneButton_clicked()
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
    ivectorExtraction->SetGender(gender);

    if (username == "") {
        QMessageBox::information(this, "Info", "Please input username.");
        return;
    } else if (usernameMap->find(username) != usernameMap->end()) {
        QMessageBox::information(this, "Info", "Username occupied. Please choose another.");
        return;
    }

    if (!ui->checkBox->isChecked()) {
        QMessageBox::information(this, "Info", "Passphrase recording not qualified.");
        return;
    }
    if (!ui->checkBox2->isChecked()) {
        QMessageBox::information(this, "Info", "The 2nd passphrase recording not qualified.");
    }

    Enroll(username, gender);

    emit SendUsername(username);

    close();
}

void signup::Enroll(string username, Gender gender) {
    usernameMap->insert(pair<string, Gender>(username,gender));
    string utt_id = username + "_1";
    compute_feat(utt_id, trainFile1);
    string feature_rspecifier = prep_feat(trainFile1);
    Vector<double> trainIvector1;
    ivectorExtraction->Extract(feature_rspecifier, trainIvector1);
    string ivecFile = prep_ivec_spec(trainFile1);
    ivectorExtraction->WriteIvector(ivecFile, utt_id, trainIvector1);

    utt_id = username + "_2";
    compute_feat(utt_id, trainFile2);
    feature_rspecifier = prep_feat(trainFile2);
    Vector<double> trainIvector2;
    ivectorExtraction->Extract(feature_rspecifier, trainIvector2);
    ivecFile = prep_ivec_spec(trainFile2);
    ivectorExtraction->WriteIvector(ivecFile, utt_id, trainIvector2);
}

void signup::on_recordButton_clicked()
{
    if (!skipRecording) {
        progressbar recordprogress(this, milSeconds, trainFile1);
        recordprogress.setModal(true);
        recordprogress.exec();
    }
    if (CheckRecording(trainFile1)) {
        ui->checkBox->setChecked(true);
    } else {
        ui->checkBox->setChecked(false);
    }
    ui->checkBox->repaint();
}

void signup::on_recordAgainButton_clicked()
{
    if (!skipRecording) {
        progressbar recordprogress(this, milSeconds, trainFile2);
        recordprogress.setModal(true);
        recordprogress.exec();
    }
    if (CheckRecording(trainFile2)) {
        ui->checkBox2->setChecked(true);
    } else {
        ui->checkBox2->setChecked(false);
    }
    ui->checkBox2->repaint();
}

bool signup::CheckRecording(string trainFile)
{
    return true;
}

void signup::on_lineEdit_editingFinished()
{
    string username = ui->lineEdit->text().toStdString();
    if (username != "" && usernameMap->find(username) == usernameMap->end()) {
        ui->checkBox3->setChecked(true);
    } else {
        ui->checkBox3->setChecked(false);
    }
    ui->checkBox3->repaint();
}

void signup::on_checkBox3_clicked()
{
    ui->checkBox3->setChecked(!ui->checkBox3->isChecked());
}

void signup::on_checkBox_clicked()
{
    ui->checkBox->setChecked(!ui->checkBox->isChecked());
}

void signup::on_checkBox2_clicked()
{
    ui->checkBox2->setChecked(!ui->checkBox2->isChecked());
}

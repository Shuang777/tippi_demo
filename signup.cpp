#include "signup.h"
#include "ui_signup.h"
#include "progressbar.h"
#include <QMessageBox>
#include <fstream>
#include <QStatusBar>
#include <QtConcurrent>
using std::pair;
using std::ifstream;

signup::signup(QWidget *parent, int milSeconds, UserMap *usernameMap, string trainFile1, string trainFile2,
               IvectorExtraction *ivectorExtraction, bool skipRecording, bool changeMode, double ivecDisThreshold) :
    QDialog(parent),
    ui(new Ui::signup),
    milSeconds(milSeconds),
    usernameMap(usernameMap),
    trainFile1(trainFile1),
    trainFile2(trainFile2),
    ivectorExtraction(ivectorExtraction),
    skipRecording(skipRecording),
    changeMode(changeMode),
    ivecDisThreshold(ivecDisThreshold)
{
    ui->setupUi(this);

    ui->checkBox->setFixedSize(21,21);
    ui->checkBox2->setFixedSize(21,21);
    ui->checkBox3->setFixedSize(21,21);

    QImage* img = new QImage(":/images/male.ico");
    male_img = img->scaled(ui->logo->width(), ui->logo->height());
    delete img;
    img = new QImage(":/images/female.ico");
    female_img = img->scaled(ui->logo->width(), ui->logo->height());
    delete img;

    ui->logo->setPixmap(QPixmap::fromImage(male_img));
    ui->radioButtonMale->setChecked(true);

    ui->checkBox->setChecked(false);
    ui->checkBox2->setChecked(false);
    ui->checkBox3->setChecked(false);

    ui->enrollBar->hide();
    connect(this, SIGNAL(EnrollProgress(int)), ui->enrollBar, SLOT(setValue(int)));
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
    } else if (username == "super") {
        QMessageBox::information(this, "Info", "Username super is reserved.");
        return;
    } else if (!changeMode && usernameMap->find(username) != usernameMap->end()) {
        QMessageBox::information(this, "Info", "Username occupied. Please choose another.");
        return;
    } else if (changeMode && usernameMap->find(username) == usernameMap->end()) {
        QMessageBox::information(this, "Info", "Username does not exist. Please check.");
        return;
    }

    if (!ui->checkBox->isChecked()) {
        QMessageBox::information(this, "Info", "Passphrase recording not qualified.");
        return;
    }
    if (!ui->checkBox2->isChecked()) {
        QMessageBox::information(this, "Info", "The 2nd passphrase recording not qualified.");
    }

    if (!Enroll(username, gender)) {
        ui->checkBox->setChecked(false);
        ui->checkBox2->setChecked(false);
        return;
    }

    emit SendUsername(username);

    close();
}

bool signup::Enroll(string username, Gender gender) {
    ui->enrollBar->setValue(0);
    ui->enrollBar->show();
    ui->enrollBar->repaint();

    usernameMap->insert(pair<string, Gender>(username,gender));
    string utt_id = username + "_1";
    compute_feat(utt_id, trainFile1);
    string feature_rspecifier = prep_feat(trainFile1);
    Vector<double> trainIvector1;
    Posterior trainPost1;
    ivectorExtraction->Extract(feature_rspecifier, trainIvector1, trainPost1);
    emit EnrollProgress(30);
    string ivecFile = prep_ivec_spec(trainFile1);
    ivectorExtraction->WriteIvector(ivecFile, utt_id, trainIvector1);
    string postFile = prep_post_spec(trainFile1);
    ivectorExtraction->WritePost(postFile, utt_id, trainPost1);    

    emit EnrollProgress(50);

    utt_id = username + "_2";
    compute_feat(utt_id, trainFile2);
    feature_rspecifier = prep_feat(trainFile2);
    Vector<double> trainIvector2;
    Posterior trainPost2;
    ivectorExtraction->Extract(feature_rspecifier, trainIvector2, trainPost2);
    emit EnrollProgress(80);
    ivecFile = prep_ivec_spec(trainFile2);
    ivectorExtraction->WriteIvector(ivecFile, utt_id, trainIvector2);
    postFile = prep_post_spec(trainFile2);
    ivectorExtraction->WritePost(postFile, utt_id, trainPost2);

    emit EnrollProgress(100);

    double score = ivectorExtraction->Scoring(trainIvector1, trainIvector2);

    ui->enrollBar->hide();

    qDebug() << "score between training files: " << score;

    if (score > ivecDisThreshold) {
        QMessageBox::information(this, "Info", "Password does not match. Please record again.");
        return false;
    } else {
        return true;
    }
}

void signup::on_recordButton_clicked()
{
    if (ui->lineEdit->text() == "") {
        QMessageBox::information(this, "Info", "Please input username.");
        return;
    }
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
    if (ui->lineEdit->text() == "") {
        QMessageBox::information(this, "Info", "Please input username.");
        return;
    }
    if (!ui->checkBox->isChecked()) {
        QMessageBox::information(this, "Info", "Please press Record passphrase first.");
        return;
    }
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

bool signup::CheckRecording(string recordingFile)
{
    ifstream is(recordingFile);
    if (!is.good()) {
        QMessageBox::information(this, "Info", "Recording not found, please record again.");
        return false;
    } else {
        return true;
    }
}

bool signup::PassNameCheck(string username) {
    if (changeMode) {
        return usernameMap->find(username) != usernameMap->end();
    } else {
        return usernameMap->find(username) == usernameMap->end();
    }
}

void signup::on_lineEdit_editingFinished()
{
    string username = ui->lineEdit->text().toStdString();
    if (username != "" && PassNameCheck(username) && username != "super") {
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

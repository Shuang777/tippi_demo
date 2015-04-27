#include "login.h"
#include "ui_login.h"
#include "signin.h"
#include "progressbar.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

    QString filename = "/home/shuang/project/tippi/final/demo/chat.png";
    QImage* img=new QImage(filename);
    QImage imgScaled=img->scaled(ui->logo->width(), ui->logo->height());
    ui->logo->setPixmap(QPixmap::fromImage(imgScaled));

    QIcon qicon(QPixmap::fromImage(imgScaled));
    QWidget::setWindowIcon(qicon);

    wavDir = "/home/shuang/project/tippi/final/demo/data/wav";

    testFile = wavDir + "/tmp/test.wav";
    trainFile1 = wavDir + "/tmp/train1.wav";
    trainFile2 = wavDir + "/tmp/train1.wav";

    numSeconds = 3;
}

login::~login() {
    delete ui;
}

void login::on_loginButton_clicked() {
    progressbar recordprogress(this, numSeconds, testFile);
    recordprogress.setModal(true);
    recordprogress.exec();
}

void login::on_signupButton_clicked() {
    signin signin_diag;
    signin_diag.setModal(true);
    this->setVisible(false);
    signin_diag.exec();
    this->setVisible(true);
}

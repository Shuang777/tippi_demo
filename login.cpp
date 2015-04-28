#include "login.h"
#include "ui_login.h"
#include "signup.h"
#include "progressbar.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <QDesktopWidget>
using std::pair;

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

    dataDir = "/home/shuang/project/tippi/final/demo/data";
    wavDir = dataDir + "/wav";

    userInfoFile = dataDir + "/user_info";
    testFile = wavDir + "/tmp/test.wav";
    trainFile1 = wavDir + "/tmp/train1.wav";
    trainFile2 = wavDir + "/tmp/train1.wav";

    numSeconds = 2;

    LoadUserInfo();

    connect(ui->lineEdit, SIGNAL(returnPressed()),ui->loginButton,SLOT(click()));

    SetCenterOfApplication();
}

void login::SetCenterOfApplication()
{
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->width();
    int height = desktop->height();
    int mw = this->width();
    int mh = this->height();
    int centerW = (width/2) - (mw/2);
    int centerH = (height/2) - (mh/2);
    this->move(centerW, centerH);
}

login::~login() {
    delete ui;
}

void login::LoadUserInfo() {
    std::ifstream infile(userInfoFile);
    string username;
    char genderchar;
    Gender gender;
    while(infile >> username >> genderchar) {
        if (genderchar == 'm') {
            gender = male;
        } else {
            gender = female;
        }
        usernameMap.insert(pair<string, Gender>(username,gender));
    }
}

bool login::HasUser(string username) {
    return usernameMap.find(username) != usernameMap.end();
}

void login::on_loginButton_clicked() {
    string username = ui->lineEdit->text().toStdString();
    if (username == "") {
        QMessageBox::information(this, "Info", "Please input username first.");
        return;
    } else if (usernameMap.find(username) == usernameMap.end()) {
        QMessageBox::information(this, "Info", "Username not found.");
        return;
    }
    progressbar recordprogress(this, numSeconds, testFile);
    recordprogress.setModal(true);
    recordprogress.exec();
    bool checkPassed = Validate(username);     // validate if the recored wav file match user's info
    if (checkPassed) {
        QMessageBox::information(this, "Info", "Login succeed!");
    } else {
        QMessageBox::information(this, "Info", "Login failed, please try again.");
    }
}

bool login::Validate(std::string username) {
    return true;
}

void login::on_signupButton_clicked() {
    signup signup_diag(this, numSeconds, &usernameMap, trainFile1, trainFile2);
    signup_diag.setModal(true);
    this->setVisible(false);
    signup_diag.exec();
    ui->lineEdit->setText("");
    this->setVisible(true);
}


void login::on_lineEdit_returnPressed()
{

}

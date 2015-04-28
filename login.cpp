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
#include <sys/stat.h>
#include <fstream>

using std::pair;
using std::endl;
using std::ofstream;

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login),
    ivectorExtraction(true)
{
    ui->setupUi(this);

    QString filename = "/home/shuang/project/tippi/final/demo/chat.png";
    QImage* img=new QImage(filename);
    QImage imgScaled=img->scaled(ui->logo->width(), ui->logo->height());
    ui->logo->setPixmap(QPixmap::fromImage(imgScaled));

    QIcon qicon(QPixmap::fromImage(imgScaled));
    QWidget::setWindowIcon(qicon);

    dataDir = "/home/shuang/project/tippi/final/demo/data";
    userInfoFile = dataDir + "/user_info";

    wavDir = dataDir + "/wav";
    tmpWavDir = wavDir + "/tmp";

    testFile = tmpWavDir + "/test.wav";
    trainFile1 = tmpWavDir + "/train1.wav";
    trainFile2 = tmpWavDir + "/train2.wav";

    fileDir = dataDir + "/files";
    tmpFileDir = fileDir + "/tmp";
    trainScp = tmpFileDir + "/train.scp";
    testScp = tmpFileDir + "/test.scp";

    modelDir = dataDir + "/models";
    string femaleIvecMdl = modelDir + "/female_final_derived.ie";
    string femaleUbm = modelDir + "/female_final.ubm";
    string maleIvecMdl = modelDir + "/male_final_derived.ie";
    string maleUbm = modelDir + "/male_final.ubm";

    ivectorExtraction.SetModels(femaleIvecMdl, femaleUbm, maleIvecMdl, maleUbm);

    mkdir(tmpFileDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(tmpWavDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    numSeconds = 2;

    PrepScpFile();

    LoadUserInfo();

    connect(ui->lineEdit, SIGNAL(returnPressed()),ui->loginButton,SLOT(click()));

    SetCenterOfApplication();
}

void login::PrepScpFile() {
    ofstream os;
    os.open(trainScp);
    os << "train1" << '\t' << trainFile1 << endl;
    os << "train2" << '\t' << trainFile2 << endl;
    os.close();
    os.open(testScp);
    os << "test" << '\t' << testFile << endl;
    os.close();
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
    signup signup_diag(this, numSeconds, &usernameMap, trainFile1, trainFile2, &ivectorExtraction);
    signup_diag.setModal(true);
    this->setVisible(false);
    signup_diag.exec();
    ui->lineEdit->setText("");
    this->setVisible(true);
}


void login::on_lineEdit_returnPressed()
{

}

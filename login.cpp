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
#include <QDebug>
#include "base/kaldi-common.h"

using std::pair;
using std::endl;
using std::ofstream;
using std::max;

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

    tmpDir = dataDir + "/tmp";
    fileDir = dataDir + "/files";
    modelDir = "/home/shuang/project/tippi/final/demo/models";

    testFile = tmpDir + "/test.wav";
    trainFile1 = tmpDir + "/train1.wav";
    trainFile2 = tmpDir + "/train2.wav";

    string femaleIvecMdl = modelDir + "/female_final_derived.ie";
    string femaleUbm = modelDir + "/female_final.ubm";
    string maleIvecMdl = modelDir + "/male_final_derived.ie";
    string maleUbm = modelDir + "/male_final.ubm";

    ivectorExtraction.SetModels(femaleIvecMdl, femaleUbm, maleIvecMdl, maleUbm);

    mkdir(tmpDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(fileDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    milSeconds = 2500;

    LoadUserInfo();

    connect(ui->lineEdit, SIGNAL(returnPressed()),ui->loginButton,SLOT(click()));

    SetCenterOfApplication();

    set_kaldi_env();

    tol = 0.08;
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
        if (genderchar == '0') {
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

    progressbar recordprogress(this, milSeconds, testFile);
    recordprogress.setModal(true);
    recordprogress.exec();

    compute_feat("test", testFile);
    string feature_rspecifier = prep_feat(testFile);
    ivectorExtraction.SetGender(usernameMap[username]);
    ivectorExtraction.Extract(feature_rspecifier, testIvector);

    bool checkPassed = Validate(username);     // validate if the recored wav file match user's info

    if (checkPassed) {
        QMessageBox::information(this, "Info", "Login succeed!");
    } else {
        QMessageBox::information(this, "Info", "Login failed, please try again.");
    }
}

bool login::Validate(std::string username) {
    string userDir = fileDir + "/" + username;
    string userIvec1 = userDir + "/train1.ivec.ark";
    ivectorExtraction.ReadIvector(userIvec1, trainIvector1);
    string userIvec2 = userDir + "/train2.ivec.ark";
    ivectorExtraction.ReadIvector(userIvec2, trainIvector2);
    double score1 = ivectorExtraction.Score(testIvector, trainIvector1);
    double score2 = ivectorExtraction.Score(testIvector, trainIvector2);
    double baseScore = ivectorExtraction.Score(trainIvector1, trainIvector2);
    qDebug() << "score against train1: " << score1;
    qDebug() << "score against train2: " << score2;
    qDebug() << "base score: " << baseScore;
    qDebug() << "max / base: " << max(score1, score2) / baseScore;
    if (max(score1, score2) > (1-tol) * baseScore)
        return true;
    else
        return false;
}

void login::on_signupButton_clicked() {
    signup signup_diag(this, milSeconds, &usernameMap, trainFile1, trainFile2, &ivectorExtraction);
    signup_diag.setModal(true);
    this->setVisible(false);
    connect(&signup_diag, SIGNAL(SendUsername(string)), this, SLOT(SetNewUsername(string)));
    signup_diag.exec();
    ui->lineEdit->setText("");
    this->setVisible(true);
    SaveNewUser();
    UpdateUserInfo();
}

void login::SaveNewUser() {
    string userDir = fileDir + "/" + newUsername;
    mkdir(userDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    string trainFileSave1 = userDir + "/train1.wav";
    move_files(trainFile1, trainFileSave1);
    string trainFileSave2 = userDir + "/train2.wav";
    move_files(trainFile2, trainFileSave2);
}

void login::SetNewUsername(string username) {
    newUsername = username;
}

void login::UpdateUserInfo() {
    ofstream os;
    os.open(userInfoFile);
    for ( auto it = usernameMap.begin(); it != usernameMap.end(); ++it )
        os << it->first << '\t' << it->second << endl;
    os.close();
}

void login::on_lineEdit_returnPressed()
{

}

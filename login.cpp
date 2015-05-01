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
using std::min;

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login),
    ivectorExtraction(true)
{
    ui->setupUi(this);

    QImage* img=new QImage(":/images/chat.png");
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

    mkdir(dataDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(tmpDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(fileDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    milSeconds = 2500;

    LoadUserInfo();

    connect(ui->lineEdit, SIGNAL(returnPressed()),ui->loginButton,SLOT(click()));

    ui->menuBar->hide();

    SetCenterOfApplication();

    set_kaldi_env();

    //ivecScoreTol = 0.08;
    ivecScoreTol = 0.3;
    ivecDisThreshold = 20;
    postScoreTol = 0.20;

    skipRecording = false;
    saveWavFile = false;
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
    if (username == "super"){
        ui->menuBar->show();
        ui->lineEdit->setText("");
        return;
    }
    if (username == "") {
        QMessageBox::information(this, "Info", "Please input username first.");
        return;
    } else if (usernameMap.find(username) == usernameMap.end()) {
        QMessageBox::information(this, "Info", "Username not found.");
        return;
    }

    if (!skipRecording) {
        progressbar recordprogress(this, milSeconds, testFile);
        recordprogress.setModal(true);
        recordprogress.exec();
        if (saveWavFile) {
            string testFileDir = fileDir + "/" + username +  "/tests";
            mkdir(testFileDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            check_and_copy_files(testFile, testFileDir);
        }
    }

    compute_feat("test", testFile);
    string feature_rspecifier = prep_feat(testFile);
    ivectorExtraction.SetGender(usernameMap[username]);
    ivectorExtraction.Extract(feature_rspecifier, testIvector, testPost);

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

    string userPost1 = userDir + "/train1.post.ark";
    ivectorExtraction.ReadPost(userPost1, trainPost1);
    string userPost2 = userDir + "/train2.post.ark";
    ivectorExtraction.ReadPost(userPost2, trainPost2);

    double score1 = ivectorExtraction.Scoring(testIvector, trainIvector1);
    double score2 = ivectorExtraction.Scoring(testIvector, trainIvector2);
    double baseScore = ivectorExtraction.Scoring(trainIvector1, trainIvector2);

    qDebug() << "score against train1: " << score1;
    qDebug() << "score against train2: " << score2;
    qDebug() << "base score: " << baseScore;
    qDebug() << "min / base: " << min(score1, score2) / baseScore;

    double scorePost1 = ivectorExtraction.Scoring(testPost, trainPost1);
    double scorePost2 = ivectorExtraction.Scoring(testPost, trainPost2);
    double baseScorePost = ivectorExtraction.Scoring(trainPost1, trainPost2);

    qDebug() << "scorePost1: " << scorePost1;
    qDebug() << "scorePost2: " << scorePost2;
    qDebug() << "baseScorePost: " << baseScorePost;
    qDebug() << "max / base: " << max(scorePost1, scorePost2) / baseScorePost;

    if ((min(score1, score2) < ivecDisThreshold) && (max(scorePost2, scorePost1) > (1 - postScoreTol) * baseScorePost))
        return true;
    else
        return false;
}

void login::on_signupButton_clicked() {
    bool changePasswdMode = false;
    signup signup_diag(this, milSeconds, &usernameMap, trainFile1, trainFile2,
                       &ivectorExtraction, skipRecording,
                       changePasswdMode, ivecDisThreshold);
    signup_diag.setModal(true);
    this->setVisible(false);
    newUsername = "";
    connect(&signup_diag, SIGNAL(SendUsername(string)), this, SLOT(SetNewUsername(string)));
    signup_diag.exec();
    ui->lineEdit->setText("");
    this->setVisible(true);
    if (newUsername != "") {        // canceled
        SaveNewUser(changePasswdMode);
        UpdateUserInfo();
    }
}

void login::on_changeButton_clicked()
{
    bool changePasswdMode = true;
    signup signup_diag(this, milSeconds, &usernameMap, trainFile1, trainFile2,
                       &ivectorExtraction, skipRecording, changePasswdMode, ivecDisThreshold);        // change mode
    signup_diag.setModal(true);
    this->setVisible(false);
    newUsername = "";
    connect(&signup_diag, SIGNAL(SendUsername(string)), this, SLOT(SetNewUsername(string)));
    signup_diag.exec();
    ui->lineEdit->setText("");
    this->setVisible(true);
    if (newUsername != "") {        // canceled
        SaveNewUser(changePasswdMode);
        UpdateUserInfo();
    }
}

void login::SaveNewUser(bool changePasswdMode) {
    string userDir = fileDir + "/" + newUsername;
    mkdir(userDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (changePasswdMode && saveWavFile) {
        string saveDir = userDir + "/trains";
        mkdir(saveDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        check_and_copy_files(trainFile1, saveDir);
        check_and_copy_files(trainFile2, saveDir);
    }
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


void login::on_actionSave_wave_files_triggered()
{
    saveWavFile = !saveWavFile;
    if (saveWavFile) {
        ui->actionSave_wave_files->setText("Overwrite wave files");
    } else {
        ui->actionSave_wave_files->setText("Save wave files");
    }
}

void login::on_actionSkip_recording_triggered()
{
    skipRecording = !skipRecording;
    if (skipRecording) {
        ui->actionSkip_recording->setText("Record");
    } else {
        ui->actionSkip_recording->setText("Skip Recording");
    }
}

#ifndef LOGIN_H
#define LOGIN_H

#include "signup.h"
#include <QMainWindow>
#include "basics.h"
#include "ivectorextraction.h"
using std::string;

namespace Ui {
class login;
}

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    bool HasUser(string username);
    ~login();

private slots:

    void on_loginButton_clicked();

    void on_signupButton_clicked();

    void on_lineEdit_returnPressed();

    void SetNewUsername(string username);

private:
    Ui::login *ui;
    signup *signup_diag;

    string userInfoFile;
    string testFile;
    string trainFile1;
    string trainFile2;

    /// Directory for all the data/file
    string dataDir;
    /// Subdirectory for wav files
    string wavDir;
    /// Subdirectory for speaker files
    string fileDir;

    string modelDir;

    string tmpDir;

    string newUsername;

    /// If the models are with derived variables
    const bool derived_in = true;
    /// class for ivector extraction and evaluation
    IvectorExtraction ivectorExtraction;

    /// length of the passphrase
    int milSeconds;

    UserMap usernameMap;

    void LoadUserInfo();

    /// Validate if the recorded testfile name the username
    bool Validate(string username);

    /// Set the window location
    void SetCenterOfApplication();

    /// rewrite the userinfo file
    void UpdateUserInfo();

    void SaveNewUser();

};

#endif // LOGIN_H

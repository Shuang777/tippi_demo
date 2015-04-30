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

    /// accept username from signup window
    void SetNewUsername(string username);

    void on_changeButton_clicked();

    void on_actionSave_wave_files_triggered();

    void on_actionSkip_recording_triggered();

private:
    Ui::login *ui;
    signup *signup_diag;

    string userInfoFile;
    string testFile;
    string trainFile1;
    string trainFile2;

    Vector<double> testIvector;
    Vector<double> trainIvector1;
    Vector<double> trainIvector2;

    Posterior testPost;
    Posterior trainPost1;
    Posterior trainPost2;

    /// Directory for all the data/file
    string dataDir;
    /// Subdirectory for speaker files
    string fileDir;

    string modelDir;

    string tmpDir;

    string newUsername;

    // Tolerance for scoring against baseline
    double ivecScoreTol;
    double postScoreTol;

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

    void SaveNewUser(bool changePasswdMode);

    /// For debug usage
    bool skipRecording;

    bool saveWavFile;
};

#endif // LOGIN_H

#ifndef LOGIN_H
#define LOGIN_H

#include "signin.h"
#include <QMainWindow>
#include "basics.h"
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

private:
    Ui::login *ui;
    signin *signin_diag;

    string userInfoFile;
    string testFile;
    string trainFile1;
    string trainFile2;

    string wavDir;
    string dataDir;

    int numSeconds;

    UserMap usernameMap;

    void LoadUserInfo();

    bool Validate(string username);

    void SetCenterOfApplication();
};

#endif // LOGIN_H

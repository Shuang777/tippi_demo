#ifndef LOGIN_H
#define LOGIN_H

#include "signin.h"
#include <QMainWindow>
using std::string;

namespace Ui {
class login;
}

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

private slots:

    void on_loginButton_clicked();

    void on_signupButton_clicked();

private:
    Ui::login *ui;
    signin *signin_diag;

    string testFile;
    string trainFile1;
    string trainFile2;

    string wavDir;
    string dataDir;

    int numSeconds;
};

#endif // LOGIN_H

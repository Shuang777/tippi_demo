#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include "basics.h"
using std::string;

namespace Ui {
class signin;
}

class signin : public QDialog
{
    Q_OBJECT

public:
    explicit signin(QWidget *parent, int numSeconds, UserMap *usernameMap, string trainFile1, string trainFile2);
    ~signin();

private slots:
    void on_radioButtonMale_clicked();

    void on_radioButtonFemale_clicked();

    void on_cancelButton_clicked();

    void on_doneButton_clicked();

    void on_recordAgainButton_clicked();

    void on_recordButton_clicked();

private:
    Ui::signin *ui;
    QString male_ico;
    QString female_ico;
    QImage male_img;
    QImage female_img;

    int numSeconds;
    UserMap *usernameMap;
    string trainFile1;
    string trainFile2;
};

#endif // SIGNIN_H

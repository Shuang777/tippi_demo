#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include "basics.h"
#include "ivectorextraction.h"
using std::string;

namespace Ui {
class signup;
}

class signup : public QDialog
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent, int milSeconds, UserMap *usernameMap, string trainFile1,
                    string trainFile2, IvectorExtraction *ivectorExtraction, bool skipRecording, bool changeMode = false);
    ~signup();

private slots:
    void on_radioButtonMale_clicked();

    void on_radioButtonFemale_clicked();

    void on_cancelButton_clicked();

    void on_doneButton_clicked();

    void on_recordAgainButton_clicked();

    void on_recordButton_clicked();

    void on_lineEdit_editingFinished();

    void on_checkBox3_clicked();

    void on_checkBox_clicked();

    void on_checkBox2_clicked();

signals:
    void SendUsername(string username);

private:
    Ui::signup *ui;
    QString male_ico;
    QString female_ico;
    QImage male_img;
    QImage female_img;

    bool changeMode;        // change a passwd rather than sign up a new one

    /// for debug usage
    bool skipRecording;

    int milSeconds;
    UserMap *usernameMap;
    string trainFile1;
    string trainFile2;
    IvectorExtraction *ivectorExtraction;

    /// Check if the recorded wav file meets criteria (e.g. speech length)
    bool CheckRecording(string trainFile);

    /// Enroll the user
    void Enroll(string username, Gender gender);

    /// if username pass the check
    bool PassNameCheck(string username);
};

#endif // SIGNUP_H

#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>

namespace Ui {
class signin;
}

class signin : public QDialog
{
    Q_OBJECT

public:
    explicit signin(QWidget *parent = 0);
    ~signin();

private slots:
    void on_pushButton_3_clicked();

    void on_radioButtonMale_clicked();

    void on_radioButtonFemale_clicked();

private:
    Ui::signin *ui;
    QString male_ico;
    QString female_ico;
    QImage male_img;
    QImage female_img;

};

#endif // SIGNIN_H

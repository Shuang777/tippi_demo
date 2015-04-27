#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QDialog>
#include <QtCore>

namespace Ui {
class progressbar;
}

class progressbar : public QDialog
{
    Q_OBJECT

public:
    explicit progressbar(QWidget *parent = 0);
    ~progressbar();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::progressbar *ui;

    QTimer *timer;

    int num_seconds;
};

#endif // PROGRESSBAR_H

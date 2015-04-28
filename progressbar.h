#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QDialog>
#include <QtCore>
using std::string;

namespace Ui {
class progressbar;
}

class progressbar : public QDialog
{
    Q_OBJECT

public:
    explicit progressbar(QWidget *parent, int numSeconds, string recordFileName);
    ~progressbar();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::progressbar *ui;

    QTimer *timer;

    int numSeconds;

    string recordFileName;

    void RecordAndWrite();

};

#endif // PROGRESSBAR_H

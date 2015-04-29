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
    explicit progressbar(QWidget *parent, int milSeconds, string recordFileName);
    ~progressbar();

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void ValueChanged(int newValue);

private:
    Ui::progressbar *ui;

    QFutureWatcher<void> FutureWatcher;

    int milSeconds;

    string recordFileName;

    void RecordAndWrite();


};

#endif // PROGRESSBAR_H

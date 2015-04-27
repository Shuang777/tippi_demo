#ifndef INCPROGRESSBAR_H
#define INCPROGRESSBAR_H

#include <QThread>
#include <QProgressBar>

class IncProgressBar : public QThread
{
public:
    IncProgressBar();
    IncProgressBar(QProgressBar *progressBar, int num_seconds);
    ~IncProgressBar();
    void run();
private:
    QProgressBar *progressBar;
    int num_seconds;
    int timestep;
};

#endif // INCPROGRESSBAR_H

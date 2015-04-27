#include "progressbar.h"
#include "ui_progressbar.h"
#include "incprogressbar.h"
#include <QKeyEvent>

progressbar::progressbar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressbar),
    num_seconds(3)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

void progressbar::keyPressEvent(QKeyEvent * e) {
    QString lastKey = e->text();
    if (lastKey == "\r") {
        IncProgressBar incProgressBar(ui->progressBar, 3);
        incProgressBar.run();
        // Record();
    }
    // Check();
}

progressbar::~progressbar()
{
    delete ui;
}

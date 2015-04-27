#include "incprogressbar.h"

IncProgressBar::IncProgressBar(QProgressBar *progressBar, int num_seconds) : timestep(200)
{
    this->progressBar = progressBar;
    this->num_seconds = num_seconds;
}

IncProgressBar::~IncProgressBar()
{

}

void IncProgressBar::run() {
    int mil_seconds = 0;
    int tot_mil_seconds = num_seconds * 1000;
    while(mil_seconds <= tot_mil_seconds) {
        progressBar->setValue(mil_seconds*100/tot_mil_seconds);
        struct timespec ts = { timestep / 1000, (timestep % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
        mil_seconds += timestep;
    }
}

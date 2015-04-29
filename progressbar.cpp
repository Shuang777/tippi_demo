#include "progressbar.h"
#include "ui_progressbar.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <portaudio.h>
#include "portaudiocallback.h"
#include <sndfile.h>

progressbar::progressbar(QWidget *parent, int milSeconds, string recordFileName) :
    QDialog(parent),
    ui(new Ui::progressbar),
    milSeconds(milSeconds),
    recordFileName(recordFileName)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->label->setText("Press Enter and record passphrase (" + QString::number(milSeconds/1000) + " seconds)");
    connect(this, SIGNAL(ValueChanged(int)), ui->progressBar, SLOT(setValue(int)));
}

void progressbar::keyPressEvent(QKeyEvent * e) {
    QString lastKey = e->text();
    if (lastKey == "\r") {      // Check if is Enter
        RecordAndWrite();
        close();
    } else if (lastKey == "\033") {     // if is Esc
        close();
    }
}

void progressbar::RecordAndWrite() {
    int totalFrames = milSeconds * SAMPLE_RATE / 1000;
    paTestData data(totalFrames);

    PaError err = Pa_Initialize();
    if( err != paNoError ) {
        QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );
    }

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        QMessageBox::critical(0,"Error", "No default input device.");
    }
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    PaStream* stream;
    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        NULL,               //
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,          //
                        recordCallback,
                        &data );

    err = Pa_StartStream( stream );
    if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );

    int countMilSec = 0;
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) {
        Pa_Sleep(200);
        countMilSec += 200;
        emit ValueChanged(int(countMilSec*100/milSeconds));
    }

    if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );

    err = Pa_CloseStream( stream );
    if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );

    // Write to file
    SNDFILE *file;
    SF_INFO sfinfo;
    memset (&sfinfo, 0, sizeof (sfinfo)) ;
    sfinfo.samplerate = SAMPLE_RATE ;
    sfinfo.frames   = totalFrames;
    sfinfo.channels = 1;
    sfinfo.format   = (SF_FORMAT_WAV | SF_FORMAT_PCM_16);
    if (! (file = sf_open (recordFileName.c_str(), SFM_WRITE, &sfinfo))) {
        QMessageBox::critical(0,"Error", "Not able to open output file") ;
    }
    if (sf_write_float (file, data.recordedSamples, sfinfo.channels * totalFrames) != sfinfo.channels * totalFrames) {
        QMessageBox::critical(0,"Error",sf_strerror(file));
    }
    sf_close (file) ;

    emit ValueChanged(100);

    /*  // Now playing back
    data.frameIndex = 0;
    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        QMessageBox::critical(0,"Error", "No default input device.");
    }
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream,
                        NULL,
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,
                        playCallback,
                        &data );

    if( stream ) {
        err = Pa_StartStream( stream );
        if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
        if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );

        err = Pa_CloseStream( stream );
        if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );
    }*/

    err = Pa_Terminate();
    if( err != paNoError ) QMessageBox::critical(0,"Error", Pa_GetErrorText( err ) );
}

progressbar::~progressbar()
{
    delete ui;
}

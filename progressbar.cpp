#include "progressbar.h"
#include "ui_progressbar.h"
#include "incprogressbar.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <portaudio.h>
#include "portaudiocallback.h"
#include <sndfile.h>

progressbar::progressbar(QWidget *parent, int numSeconds, string recordFileName) :
    QDialog(parent),
    ui(new Ui::progressbar),
    numSeconds(numSeconds),
    recordFileName(recordFileName)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->label->setText("Press Enter and record passphrase (" + QString::number(numSeconds) + " seconds)");
}

void progressbar::keyPressEvent(QKeyEvent * e) {
    QString lastKey = e->text();
    if (lastKey == "\r") {
        IncProgressBar incProgressBar(ui->progressBar, 3);
        incProgressBar.start();
        RecordAndWrite();
        incProgressBar.wait();
        //Check();
    }
}

void progressbar::RecordAndWrite() {
    int totalFrames = numSeconds * SAMPLE_RATE;
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

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) { Pa_Sleep(1000); }
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

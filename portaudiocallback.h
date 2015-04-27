#ifndef PORTAUDIOCALLBACK
#define PORTAUDIOCALLBACK
#endif // PORTAUDIOCALLBACK
#include <portaudio.h>

#define SAMPLE_RATE 8000
#define FRAMES_PER_BUFFER (512)
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)

struct PaTestData{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE       *recordedSamples;
    PaTestData(int numFrames) : frameIndex(0), maxFrameIndex(numFrames) {
        recordedSamples = (SAMPLE*) malloc(numFrames * sizeof(SAMPLE));
    }
    ~PaTestData() {
        free(recordedSamples);
    }
};

typedef struct PaTestData paTestData;


static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData ) {
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->recordedSamples[data->frameIndex];
    int framesToCalc;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer ) {
        framesToCalc = framesLeft;
        finished = paComplete;
    } else {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if( inputBuffer == NULL ) {
        for(int i=0; i<framesToCalc; i++ ) {
            *wptr++ = SAMPLE_SILENCE;  /* left */
        }
    }
    else {
        for(int i=0; i<framesToCalc; i++ ) {
            *wptr++ = *rptr++;  /* left */
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
}

static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData ) {
    paTestData *data = (paTestData*)userData;
    SAMPLE *rptr = &data->recordedSamples[data->frameIndex];
    SAMPLE *wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )  {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ ) {
            *wptr++ = *rptr++;
        }
        for( ; i<framesPerBuffer; i++ ) {
            *wptr++ = 0;
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    } else {
        for( i=0; i<framesPerBuffer; i++ ) {
            *wptr++ = *rptr++;
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}

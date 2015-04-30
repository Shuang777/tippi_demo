#ifndef IVECTOREXTRACTION_H
#define IVECTOREXTRACTION_H
#include "basics.h"
#include "ivector/ivector-extractor.h"
#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "gmm/full-gmm.h"

using namespace kaldi;

class IvectorExtraction
{
public:
    IvectorExtraction(bool derived_in);
    ~IvectorExtraction();
    void SetModels(string femaleIvecMdl, string femaleUbm, string maleIvecMdl, string maleUbm);
    void SetGender(Gender gender);
    void Extract(string feature_rspecifier, Vector<double> &ivector, Posterior &post);
    static void WriteIvector(string ivecFile, string utt_id, const Vector<double> & ivector);
    static void WritePost(string postFile, string utt_id, const Posterior & post);
    static void ReadIvector(string ivecFile, Vector<double> & ivector);
    static void ReadPost(string postFile, Posterior &post);
    static double Score(const Vector<double> & ivec1, const Vector<double> & ivec2);

private:
    IvectorExtractor extractor;
    FullGmm maleFgmm;
    FullGmm femaleFgmm;

    Gender gender;

    string femaleIvecMdl;
    string maleIvecMdl;
    string femaleUbm;
    string maleUbm;

    bool derived_in;
    int num_gselect;
    double min_post;
};

#endif // IVECTOREXTRACTION_H

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
    void Extract(string feature_rspecifier, string ivector_wspecifier);

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

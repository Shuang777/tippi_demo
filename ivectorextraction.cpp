#include "ivectorextraction.h"
#include <vector>
#include <QDebug>
using std::vector;
using std::pair;
using std::max;

IvectorExtraction::IvectorExtraction(bool derived_in) : derived_in(derived_in),
                                                        extractor() {
    num_gselect = 20;
    min_post = 0.025;
}

IvectorExtraction::~IvectorExtraction() { }

void IvectorExtraction::SetModels(string femaleIvecMdl, string femaleUbm, string maleIvecMdl, string maleUbm) {
    this->femaleIvecMdl = femaleIvecMdl;
    this->maleIvecMdl = maleIvecMdl;
    this->femaleUbm = femaleUbm;
    this->maleUbm = maleUbm;

    {
        bool binary_in;
        Input ki(maleIvecMdl, &binary_in);
        extractor.Read(ki.Stream(), binary_in, derived_in);
        gender = male;
    }
    {
          bool binary_read;
          Input ki(maleUbm, &binary_read);
          maleFgmm.Read(ki.Stream(), binary_read);
    }
    {
          bool binary_read;
          Input ki(femaleUbm, &binary_read);
          femaleFgmm.Read(ki.Stream(), binary_read);
    }
}

void IvectorExtraction::SetGender(Gender gender) {
    if (gender == this->gender) {
        // Do nothing
        return;
    } else {
        // Set the gender and change model (because we don't have enough memory now)
        this->gender = gender;
        bool binary_in;
        if (gender == male) {
            Input ki(maleIvecMdl, &binary_in);
            extractor.Read(ki.Stream(), binary_in, derived_in);
        } else {
            Input ki(femaleIvecMdl, &binary_in);
            extractor.Read(ki.Stream(), binary_in, derived_in);
        }
    }
}

void IvectorExtraction::Extract(std::string feature_rspecifier, Vector<double> &ivector, Posterior &post) {
    // assuming there is only one file in ivector_wspecifier
    // mostly copied from Kaldi codes
    SequentialBaseFloatMatrixReader feature_reader(feature_rspecifier);
    const Matrix<BaseFloat> &mat = feature_reader.Value();
    int32 num_frames = mat.NumRows();
    const FullGmm &fgmm = (gender == male) ? maleFgmm : femaleFgmm;
    double tot_posterior = 0;
    post.resize(num_frames);
    for (int32 i = 0; i < mat.NumRows(); i++) {
        tot_posterior += fgmm.GaussianSelection(mat.Row(i), num_gselect, post[i]);
    }
    qDebug() << "tot_posterior: " << tot_posterior / num_frames << "per frame"
             << "on " << num_frames << " frames";

    // Extract ivector
    bool need_2nd_order_stats = false;
    IvectorExtractorUtteranceStats utt_stats(extractor.NumGauss(),
                                             extractor.FeatDim(),
                                             need_2nd_order_stats);
    utt_stats.AccStats(mat, post);

    ivector.Resize(extractor.IvectorDim());

    ivector(0) = extractor.PriorOffset();
    extractor.GetIvectorDistribution(utt_stats, &ivector, NULL);
    ivector(0) -= extractor.PriorOffset();

    // Normalize length
    double norm = ivector.Norm(2.0);
    double ratio = norm / sqrt(ivector.Dim());
    ivector.Scale(1.0 / ratio);

}

void IvectorExtraction::WriteIvector(string ivecFile, string utt_id, const Vector<double> & ivector) {
    BaseFloatVectorWriter ivector_writer("ark:"+ivecFile);
    ivector_writer.Write(utt_id, (const Vector<BaseFloat>)ivector);
}

void IvectorExtraction::ReadIvector(string ivecFile, Vector<double> & ivector) {
    SequentialBaseFloatVectorReader ivector_reader("ark:"+ivecFile);
    ivector = (Vector<double>) ivector_reader.Value();
}

void IvectorExtraction::WritePost(string postFile, string utt_id, const Posterior & post) {
    PosteriorWriter post_writer("ark:" + postFile);
    post_writer.Write(utt_id, post);
}

void IvectorExtraction::ReadPost(string postFile, Posterior & post) {
    SequentialPosteriorReader posteriors_reader("ark:"+postFile);
    post = posteriors_reader.Value();
}

double IvectorExtraction::Scoring(const Vector<double> & ivec1, const Vector<double> & ivec2) {
    return VecVec(ivec1, ivec2);
}

double IvectorExtraction::Scoring(const Posterior & post1, const Posterior & post2) {
    double mismatchPenalty = -0.01;
    int numFrames1 = post1.size();
    int numFrames2 = post2.size();
    Matrix<double> bestPath (numFrames1, numFrames2);
    bestPath(0,0) = Distance(post1[0], post2[0]);
    for (int i = 1; i < numFrames1; i++) {
        bestPath(i,0) = Distance(post1[i], post2[0]) + bestPath(i-1,0) + mismatchPenalty;
    }
    for (int j = 1; j < numFrames2; j++) {
        bestPath(0,j) = Distance(post1[0], post2[j]) + bestPath(0, j-1) + mismatchPenalty;
    }
    for (int i = 1; i < numFrames1; i++) {
        for (int j = 1; j < numFrames2; j++) {
            double path1Score = bestPath(i,j-1) + mismatchPenalty;  // horizontal path
            double path2Score = bestPath(i-1,j) + mismatchPenalty;  // vertical path
            double path3Score = bestPath(i-1,j-1);                  // diagnoal path
            double bestScore = max(max(path1Score, path2Score), path3Score);
            bestPath(i,j) = bestScore + Distance(post1[i],post2[j]);
        }
    }
    return bestPath(numFrames1-1, numFrames2-1);
}

double IvectorExtraction::Distance(const vector<pair<int,float> > &post1, const vector<pair<int,float> > &post2) {
    int firstN = 5;
    double score = 0;
    for (int i = 0; i < firstN && i < post1.size(); i++) {
        for (int j = 0; j < firstN && j < post2.size(); j++) {
            if (post1[i].first == post2[j].first)
                score += post1[i].second * post2[j].second;
        }
    }
    return score;
}

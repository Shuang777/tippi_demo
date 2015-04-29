#include "basics.h"
#include <fstream>
#include <QDebug>
#include <QString>
using std::ofstream;
using std::endl;

string replace_str(const string& str, const string& from, const string& to) {
   string replaced = str;
   size_t start_pos = replaced.find(from);
   if(start_pos != std::string::npos)
       replaced.replace(start_pos, from.length(), to);
   return replaced;
}

void prep_scp_file(string scpFile, string utt_id, string wavFile) {
    ofstream os;
    os.open(scpFile);
    os << utt_id << '\t' << wavFile << endl;
    os.close();
}

void compute_feat(string utt_id, string wavFile) {
    string featArk = replace_str(wavFile, ".wav", ".mfcc.ark");
    string cmdline = "compute-mfcc-feats --config=/home/shuang/project/tippi/final/demo/conf/mfcc.conf 'scp:echo "
                     + utt_id + " " + wavFile + "|' ark:" + featArk;
    system(cmdline.c_str());

    string cmvnArk = replace_str(wavFile, ".wav", ".cmvn.ark");
    cmdline = "compute-cmvn-stats ark:" + featArk + " ark:" + cmvnArk;
    system(cmdline.c_str());

    string vadArk = replace_str(wavFile, ".wav", ".vad.ark");
    cmdline = "compute-vad --config=/home/shuang/project/tippi/final/demo/conf/vad.conf ark:" + featArk
              + " ark:" + vadArk;
    system(cmdline.c_str());
}

string prep_feat(string wavFile) {
    string featArk = replace_str(wavFile, ".wav", ".mfcc.ark");
    string cmvnArk = replace_str(wavFile, ".wav", ".cmvn.ark");
    string vadArk = replace_str(wavFile, ".wav", ".vad.ark");
    string feats = "ark:apply-cmvn --norm-vars=false ark:" + cmvnArk + " ark:" + featArk + " ark:- | add-deltas ark:- ark:- | select-voiced-frames ark:- ark:"
            + vadArk + " ark:- |";
    return feats;
}

string prep_ivec_spec(string wavFile) {
    string ivecArk = replace_str(wavFile, ".wav", ".ivec.ark");
    return ivecArk;
}


void set_kaldi_env() {
    string path = getenv("PATH");
    string kaldiRoot = "/home/shuang/project/kaldi/trunk";
    path = kaldiRoot + "/src/online2bin:"
                   + kaldiRoot + "/src/bin:"
                   + kaldiRoot + "/tools/openfst/bin"
                   + kaldiRoot + "/src/fstbin/:"
                   + kaldiRoot + "/src/gmmbin/:"
                   + kaldiRoot + "/src/featbin/:"
                   + kaldiRoot + "/src/lm/:"
                   + kaldiRoot + "/src/sgmmbin/:"
                   + kaldiRoot + "/src/sgmm2bin/:"
                   + kaldiRoot + "/src/fgmmbin/:"
                   + kaldiRoot + "/src/latbin/:"
                   + kaldiRoot + "/src/nnetbin:"
                   + kaldiRoot + "/src/nnet2bin:"
                   + kaldiRoot + "/src/online2bin/:"
                   + kaldiRoot + "/src/ivectorbin/:"
                   + path;
    setenv("PATH", path.c_str(), 1);
}

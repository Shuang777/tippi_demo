#include "basics.h"
#include <fstream>
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
    string wavScp = wavFile + ".scp";
    prep_scp_file(wavScp, utt_id, wavFile);
    string featArk = replace_str(wavScp, ".wav.scp", ".ark");
    string featScp = replace_str(wavScp, ".wav.scp", ".scp");
    string cmdline = ". " + pathFile + "; compute-mfcc-feats --config=/home/shuang/project/tippi/final/demo/conf/mfcc.conf scp:"
                     + wavScp + " ark,scp:" + featArk + "," + featScp;
    system(cmdline.c_str());

    string cmvnArk = replace_str(wavScp, ".wav.scp", ".cmvn.ark");
    string cmvnScp = replace_str(wavScp, ".wav.scp", ".cmvn.scp");
    cmdline = ". " + pathFile + "; compute-cmvn-stats scp:" + featScp + " ark,scp:" + cmvnArk + "," + cmvnScp;
    system(cmdline.c_str());

    string vadArk = replace_str(wavScp, ".wav.scp", ".vad.ark");
    string vadScp = replace_str(wavScp, ".wav.scp", ".vad.scp");
    cmdline = ". " + pathFile + "; compute-vad --config=/home/shuang/project/tippi/final/demo/conf/vad.conf scp:" + featScp
              + " ark,scp:" + vadArk + "," + vadScp;
    system(cmdline.c_str());
}

#ifndef BASICS_H
#define BASICS_H

#include <unordered_map>
using std::unordered_map;
using std::string;

enum Gender {
    male,
    female
};

const string pathFile = "/home/shuang/project/tippi/final/demo/path.sh";

typedef unordered_map<string, Gender> UserMap;

string replace_str(const string& str, const string& from, const string& to);

void compute_feat(string utt_id, string wavFile);

void prep_scp_file(string scpFile, string utt_id, string wavFile);

string prep_feat(string wavFile);

string prep_ivec_spec(string wavFile);

void set_kaldi_env();

void move_files(string oriFile, string newFile);

#endif // BASICS_H

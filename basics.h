#ifndef BASICS_H
#define BASICS_H

#include <unordered_map>
using std::unordered_map;
using std::string;

enum Gender {
    male,
    female
};

typedef unordered_map<string, Gender> UserMap;

#endif // BASICS_H

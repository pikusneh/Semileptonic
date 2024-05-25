#ifndef SKIM_H
#define SKIM_H

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class skim {
public:
    skim(bool isMC, const std::string &year, const std::string &filelist);
    bool startsWith(const char *string, const char *prefix);
    std::vector<std::string> readInputFiles(const std::string &filename);
    void myskim();

private:
    bool isMC;
    std::string year;
    std::string filelist;
    std::vector<std::string> inputFiles;
    ROOT::RDataFrame df;
};

#endif // SKIM_H
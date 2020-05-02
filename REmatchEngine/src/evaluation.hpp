#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>
#include <vector>
#include <string>
#include <stack>
#include <utility>
#include <map>
#include <list>

#include "automata/eva.hpp"
#include "automata/detautomaton.hpp"
#include "enumeration.hpp"
#include "det/detmanager.hpp"

#include "structures.hpp"
#include "factories.hpp"
#include "memmanager.hpp"
#include "mapping.hpp"
#include "structs/vector.hpp"

class DetState;

class Evaluation {
  public:
    DetAutomaton *detA;

    std::istream &inputStream;
    std::string inputString;
    std::ostream &outputStream;

    MemManager *memManager;
    DetManager *detManager;

    long i;
    long il;

    bool earlyOutput;
    bool lineByLine;
    bool kNoCaptures;

    rematch::vector<DetState*> currentStates;
    rematch::vector<DetState*> newStates;
    rematch::vector<DetState*> captureStates;

    Enumerator *enumerator;
    NodeList outputList;

    Evaluation(ExtendedVA *A, std::istream &is, std::ostream &os,
               bool c, bool l);

    void initLineByLine();
    void initNoLineByLine();

    void initAutomaton(long i);
    void capture(long i);
    void reading(char a, long i);

    void readingEO(char a, long i);
    void captureEO(long i);

    std::map<std::string, std::pair<size_t, size_t>> next();
    std::string pnext();

    bool hasNext00();
    bool hasNext10();
    bool hasNext01();
    bool hasNext11();

    void evaluate();
    bool match();

    std::vector<std::string> getOutputSchema();

  private:
    std::map<uint32_t, uint32_t> histogram_;
    void addToHistogram(uint32_t key) {
      auto it = histogram_.find(key);
      if(it != histogram_.end())
        it->second += 1;
      else
        histogram_[key] = 1;
    }

    void dumpHistogram(std::string path) {
      std::ofstream outfile;
      outfile.open(path);
      for(auto const& element: histogram_)
        outfile << element.first << ',' << element.second << '\n';
      outfile.close();
    }


};


#endif
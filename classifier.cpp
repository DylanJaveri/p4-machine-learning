#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "csvstream.hpp"
#include <map>
#include <set>
#include <cmath>
#include <utility>
using namespace std;
using std::cout;

class Classifier {
private:
        map <string, int> word; //For each word , the number of posts in the entire training set that contain . 
        map <string, int> label; //For each label the number of posts with that label.
        map <pair<string, string>, int> numPosts; //For each label and word, the number of posts with label that contain w.     
        double totalPosts = 0; //The total number of posts in the entire training set. int totalPosts;
        int vocabSize = 0; //The number of unique words in the entire training set. (The vocabulary size.) int numUniWords;
        string allWords;
        set<string> uniWords;
public:
    set<string> unique_words(const string &str) {
        istringstream source(str);
        set<string> words;
        string word;
        while (source >> word) {
        words.insert(word);
        }
        return words;
    }
    
    double logPrior(string C) {
        return log(label[C] / totalPosts);
    }

    double logLikelihood(string C, string w) {
        pair<string, string> pair = {C, w};
        if (uniWords.find(w) != uniWords.end()) {
            return log(1 / totalPosts);
        }
        else if (numPosts[pair] == 0) {
            return log(word[w] / totalPosts);
        }
        else {
          return log(numPosts[pair]/totalPosts);
        }
    }

    double logProb(string C, set<string> bag) {
        double sum = logPrior(C);
        for (string element : bag) {
            sum += logLikelihood(C, element); 
        }
        return sum;
    }
};

int main(int argc, char** argv) {
    
    cout.precision(3);
    
    if (argc != 2 && argc != 3) {
        cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    }
    Classifier Classifier;
    string trainData = argv[1];
    try {
        csvstream csvin(trainData);
        map<string, string> row;
    while (csvin >> row) {
      cout << row["animal"] << "\n";
    }
    } catch(const csvstream_exception &e) {
    cerr << e.what() << "\n";
    return 1;
    }
  return 0;

}

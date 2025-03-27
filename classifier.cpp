#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "csvstream.hpp"
#include <map>
#include <set>
#include <cmath>
#include <utility>
#include <unordered_set>
using namespace std;
using std::cout;

class Classifier {
private:
        //For each word, the number of posts in the training set that contain word.
        map <string, int> word; 
        
        //For each label the number of posts with that label.
        map <string, int> label;
        
        //For each label and word, the number of posts with label that contain w.
        map <pair<string, string>, int> numPosts; 
        
        //The total number of posts in the entire training set. int totalPosts;
        double totalPosts = 0;
        
        //The number of unique words in the entire training set. (The vocabulary size.)
        string allWords;
        set<string> testWords;
        set<string> uniWords;
        set<string> labels;
        int correctMatch = 0;
public:
    void unique_words(const string &str) {
        
      istringstream source(str);
        
        string word;
        
        while (source >> word) {
          
          uniWords.insert(word);
        
        }
    
      }

    void setTestWords(const string &str) {
        istringstream source(str);
        string word;
        while (source >> word) {
            testWords.insert(word);
        }
    }

    set<string> getTestWords() {
        return testWords;
    }
    
    double logPrior(string C) {
        return log(label[C] / totalPosts);
    }

    double logLikelihood(string C, string w) {
        pair<string, string> p = {C, w};
        if (numPosts[p] > 0){
            return log(numPosts[p] / static_cast<double>(label[C]));
        }
        else if (word[w] > 0){
            return log(word[w] / totalPosts);
        }
        else {
            return log(1 / totalPosts);
        }
    }

    void logProb(set<string> bag, string tag) {
        double finalSum = -INFINITY;
        set<string> bestLabels;
        for (string currentTag : labels) {       
            double sum = logPrior(currentTag);
            for (string element : bag) {
                sum += logLikelihood(currentTag, element); 
            }
            if (sum > finalSum) {
                bestLabels.clear();
                bestLabels.insert(currentTag);
                finalSum = sum;
            }
            else if (sum == finalSum) {
                bestLabels.insert(currentTag);
            }
        }
        cout << *bestLabels.begin() << ", log-probability score = " << finalSum << endl;
        if (tag == *bestLabels.begin()){
            correctMatch++;
        }

        testWords.clear();
    }

    double getTotalPosts() {
        return totalPosts;
    }

    set<string> getUniqueWord() {
        return uniWords;
    }

    double getVocabSize() {
        return uniWords.size();
    }

    void setLabel(string label){
      labels.insert(label);
    }

    set<string> getLabel() {
        return labels;
    }
    
    void setTotalPosts (){
      totalPosts++;
    }

    int getCorrect(){
        return correctMatch;
    }

    void SetLabelPosts(string tag, string content) {
        if (content.find(tag) != string::npos) {
            label[tag]++;
        }
    }

    void SetWordPosts(string w, string content) {
        istringstream source(content);
        
        string wordz;
        
        while (source >> wordz) {
        if (wordz == w) {
            word[w]++;
            break;
        }        
        }
      }
        

    void SetBoth(pair<string, string> labelWord, string contentTag, string contentWord){
        if (contentTag.find(labelWord.first) != string::npos) {

            istringstream source(contentWord);
        
            string wordz;
        
            while (source >> wordz) {
            if (wordz == labelWord.second) {
                numPosts[labelWord]++;
                break;
            }        
            } 
        }
    }

    void printInfo(){
        cout << "classes:" << endl;
        for (string element : labels) {
            cout << "  " << element << ", " << label[element] 
            << " examples, log-prior = " << logPrior(element) << endl;
        }
        cout << "classifier parameters:" << endl;
        for ( pair<const pair<string, string>, int>& element : numPosts){
            cout << "  " << element.first.first << ":" << element.first.second 
            << ", count = " << element.second << ", log-likelihood = " << 
            logLikelihood(element.first.first, element.first.second) << endl;
        }
        cout << endl;
    }
};

int main(int argc, char** argv) {
    cout.precision(3);
    int numTestPosts = 0;
    if (argc != 2 && argc != 3) {
        cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    }
    
    Classifier Classifier;
    string filename = argv[1];
    
    Classifier.SetWordPosts("a", "this is assert");
    try {
        csvstream csvin(filename);
        map<string, string> row;
        if (argc == 2) {
        cout << "training data:" << endl;}
    while (csvin >> row) {
        if (argc == 2) {
            cout << "  label = " << row["tag"] << ", content = " 
            << row["content"] << endl;
        }
        Classifier.unique_words(row["content"]);
        Classifier.setLabel(row["tag"]);
        Classifier.setTotalPosts();
        for (string element : Classifier.getLabel()) {
            Classifier.SetLabelPosts(element, row["tag"]);
            for (string w : Classifier.getUniqueWord()) {
                pair<string, string> p1 = {element, w};
                Classifier.SetBoth(p1, row["tag"], row["content"]);
            }
        }
        for (string w : Classifier.getUniqueWord()) {
            Classifier.SetWordPosts(w, row["content"]);
        }
    }
    }
    catch(const csvstream_exception &e) {
      cout << "Error opening file: " << filename << endl;
      return 1;
    }
    
    cout << "trained on " << Classifier.getTotalPosts() << " examples" << endl << endl; 
    
    if (argc == 2) {
        cout << "vocabulary size = " << Classifier.getVocabSize() << endl << endl;
        Classifier.printInfo(); 
        
    }
    if (argc == 3) {
            filename = argv[2];
            try {
            csvstream csvin(filename);
            map<string, string> row;
            cout << "test data:" << endl;
        while (csvin >> row) {
            Classifier.setTestWords(row["content"]);
            cout << "  correct = " << row["tag"] << ", predicted = ";
            Classifier.logProb(Classifier.getTestWords(), row["tag"]); 
            cout << "  content = " << row["content"] << endl << endl; 
            numTestPosts++;
            for (string element : Classifier.getLabel()) {
                Classifier.SetLabelPosts(element, row["tag"]);
                for (string w : Classifier.getUniqueWord()) {
                    pair<string, string> p1 = {element, w};
                    Classifier.SetBoth(p1, row["tag"], row["content"]);
                }
            }
            
            for (string w : Classifier.getUniqueWord()) {
                Classifier.SetWordPosts(w, row["content"]);
            }
        }
        }
        catch(const csvstream_exception &e) {
        cout << "Error opening file: " << filename << endl;
        return 1;
        }
        cout << "performance: " << Classifier.getCorrect() << " / " 
        << numTestPosts << " posts predicted correctly" << endl;
    }
    
  return 0;
}
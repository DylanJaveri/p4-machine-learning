#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class bagOfWords {

    private:
            struct wordDetails {
                string word;
                int wordCount;
            }; 

            struct labelWord {
                string label;
                vector <wordDetails> allWords;
            };
            
            int totalPosts = 0;
            int vocabSize = 0;
            vector <string> differingLabels;
            vector <int> postCount;
            vector <wordDetails> uniqueWords;
            vector <labelWord> words_labeled;
};

int main() {
  cout << "Hello World!\n";
}
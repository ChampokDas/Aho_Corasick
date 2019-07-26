#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm> // std::if_remove() and std::transform() for cleaning the string

using namespace std;

// *** Required to be AT LEAST the sum of all lengths of strings in the dictionary ***
// This represents all the possible connections between words
const int SUM_WORDS = 200; 

// The following 3 variables is the basic logic of the Trie data structure in Aho_Corasick
// Each represents a 'connection' between nodes
// See https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm
vector<int> out(SUM_WORDS, 0);
vector<int> failure(SUM_WORDS, -1);
vector<vector<int> > moveToNode(SUM_WORDS, vector<int>(26, -1));

int createTrieFromDictionary(vector<string> &dictionary) {
    
    int states = 1;
    // Create a path from root 'node' consisting of all dictionary words
    for (int i = 0; i < dictionary.size(); i++) {
        string word = dictionary[i];
        int currentState = 0;

        // Each character in the word will be placed along the path from the root
        for (int j = 0; j < word.size(); j++) {
            int c = word[j] - 'a';
            // If the node does not exist, create a new node attached to root
            if (moveToNode[currentState][c] == -1) { 
                moveToNode[currentState][c] = states++;
            }
            currentState = moveToNode[currentState][c];
        }
        // Add the word to out
        out[currentState] |= (1 << i); 
    }
    
    // State 0 should have an outgoing edge for all characters.
    for (int i = 0; i < 26; i++) {
        if (moveToNode[0][i] == -1) {
            moveToNode[0][i] = 0;
        }
    }

    // Create 'failure' paths
    queue<int> q;
    for (int c = 0; c <= 'z' - 'a'; ++c) {  // Iterate over every possible input
        // All nodes s of depth 1 have f[s] = 0
        if (moveToNode[0][c] != -1 && moveToNode[0][c] != 0) {
            failure[moveToNode[0][c]] = 0;
            q.push(moveToNode[0][c]);
        }
    }

    // Move through the 'Trie'
    // Basic finite-state-machine logic applied here with respect to Aho_Corasick
    while (!q.empty()) {
        int state = q.front();
        q.pop();

        // Move through path
        for (int c = 0; c <= 'z' - 'a'; c++) {
            if (moveToNode[state][c] != -1) {
                int fail = failure[state];

                // Try to find a deepest matched node in the Trie to the char 
                while (moveToNode[fail][c] == -1)
                    fail = failure[fail];

                fail = moveToNode[fail][c];
                failure[moveToNode[state][c]] = fail;

                out[moveToNode[state][c]] |= out[fail]; // merge out values
                q.push(moveToNode[state][c]); // Move to next level
            }
        }
    }
    return states;
}

// Finds the next state the machine will transition to.
int findNextState(int currentState, char nextChar) {
    int answer = currentState;
    int c = nextChar - 'a';

    // Attempt to find the corresponding node in moveToNode
    // by referencing failure
    while (moveToNode[answer][c] == -1) 
        answer = failure[answer];

    return moveToNode[answer][c];
}

// Find all matches in given text using the dictionary
void matchToDictionary(vector<string> &dictionary, string text) {
    createTrieFromDictionary(dictionary);
    
    int currState = 0;

    for (int i = 0; i < text.size(); i++) {
        currState = findNextState(currState, text[i]);

        if (out[currState] == 0) 
          continue; // until a match is found
      
        // Promising node found
        for (int j = 0; j < dictionary.size(); j++) {
            if (out[currState] & (1 << j)) { 
              cout << "Matched word \'" << dictionary[j] << "\' between " << i - dictionary[j].size() + 1 << " to " << i << endl;
            }
        }
    }
}

void cleanString(string &text) {
    // remove any char that is not A-Z or a-z
    text.erase(remove_if(text.begin(), text.end(), []( auto const& c ) -> bool { return !isalpha(c); } ), text.end());
    // all chars are made lowercase
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    cout << "Cleaned string: " << text << endl;
}

int main(){
    // Initialize a dictionary of 'words' to match in the Aho_Corasick algorithm
    vector<string> dictionary;
    dictionary.push_back("ban");
    dictionary.push_back("banana");
    dictionary.push_back("bananana");
    dictionary.push_back("kj");
    
    // Create some kind of text, it may be garbled as you desire
    // Aho_Corasick will match every occurence in string to the words in dictionary
    // Any character that is not lowercase and not alphabetic will be pruned
    string text = "I am a ban happy banana with bananana ban okj1?!?.";
    cleanString(text);
    
    matchToDictionary(dictionary, text);
   
    return 0;
}
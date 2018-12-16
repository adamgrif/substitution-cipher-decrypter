#include "provided.h"
#include "MyHash.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

class WordListImpl
{
public:
    WordListImpl();
    ~WordListImpl();
    bool loadWordList(string filename);
    bool contains(string word) const;
    vector<string> findCandidates(string cipherWord, string currTranslation) const;
    
private:
    
    MyHash<string, vector<string>>* mh;
    
    bool viableWord(string& s);
    string getKey(string s) const;
};

WordListImpl::WordListImpl()
: mh(new MyHash<string, vector<string>>) {}

WordListImpl::~WordListImpl()
{
    delete mh;
}

bool WordListImpl::loadWordList(string filename)
{
    // Discard old list of words if one exists
    if (mh != nullptr){
        mh->reset();
    }
    
    ifstream words(filename);
    
    if ( ! words){
        return false;
    }
    
    string currWord;
    while (getline(words, currWord)){
        if ( ! viableWord(currWord)){
            // viableWord also changes the word to lower case, as all functions for seach are case-insensitive
            // If the word isn't viable, continue onto the next line
            continue;
        }
        
        // Get the key (letter pattern) for the current word
        string key = getKey(currWord);
        
        // Get the vector in the specific bucket with the specific key
        vector<string>* wordVector = mh->find(key);
        
        if (wordVector == nullptr){
            // If the pointer is nullptr, meaning no words with that key have been added,
            // then create a new vector and add the new word to it
            vector<string> newWordVector;
            newWordVector.push_back(currWord);
            // Put that vector into the hash table using the key
            mh->associate(key, newWordVector);
        } else {
            // Else, there has been at least one word with the same key (letter pattern) added to the table.
            // Create a new vector and set it to the vector of words with that key retrieved by the find function,
            // then push back the new word onto the vector and update that key in the table
            // (Can't just update the current vector because not all hash tables are modifiable)
            vector<string> newWordVector = *wordVector;
            newWordVector.push_back(currWord);
            mh->associate(key, newWordVector);
        }
    }

    return true;
}

bool WordListImpl::contains(string word) const
{
    // Get the key for the given word
    string key = getKey(word);
    // Get the pointer to the vector for the given key
    vector<string>* words = mh->find(key);
    
    // If that pointer is nullptr, the item is not in the hash table
    if (words == nullptr){
        return false;
    }
    
    // Make the word all lower case, contains should be case-insensitive
    int wordSize = (int)word.size();
    for (int i = 0; i < wordSize; i++){
        word[i] = tolower(word[i]);
    }
    
    // For every word in the vector at that bucket with that key
    int vSize = (int)(*words).size();
    for (int i = 0; i < vSize; i++){
        
        // Make all words that were found totally lower case so that the comparison is case-insensitive
        for (int j = 0; j < wordSize; j++){
            (*words)[i][j] = tolower((*words)[i][j]);
        }
        
        // If the word given equals a word in the vector, return true because it is found
        if ((*words)[i] == word){
            return true;
        }
    }
    
    // Otherwise, if not found in that bucket or in that vector, return false
    return false;
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const
{
    // cipherWord must be all letters and apostrophes
    // currTranslation must be all letters, apostrophes, and ?s
    
    int len = (int)cipherWord.size();
    // Return empty vector if the two parameters aren't the same length
    if (len != currTranslation.size()){
        return vector<string>();
    }
    
    // Return false if either string contains a character that isn't a letter nor an apostrophe
    for (int i = 0; i < len; i++){
        // Changes any letters in the current translation to lower case because all words in the table are lower case
        // and it is supposed to be case-insensitive
        currTranslation[i] = tolower(currTranslation[i]);
        // Changes any letters in cipherWord to upper case because that is how the letter patterns, keys, are organized
        cipherWord[i] = toupper(cipherWord[i]);
        
        // If either string doesn't abide by the requirements (stated at top of function), return empty vector
        if ((! isupper(cipherWord[i]) && cipherWord[i] != '\'') || ( ! islower(currTranslation[i]) && currTranslation[i] != '\'' && currTranslation[i] != '?')){
            return vector<string>();
        }
    }
    
    // Resets any cipher letter pattern to an equal pattern that matches the keys used in the word list has table
    cipherWord = getKey(cipherWord);
    
    // Find the vector of words matching the letter pattern
    vector<string>* initialCandidates = mh->find(cipherWord);
    
    if (initialCandidates == nullptr){
        // If the poiter is nullptr, no words with that pattern were found, return empty vector
        return vector<string>();
    }
    
    // Vector of final candidates to be returned
    vector<string> finalCandidates;
    
    int nCandidates = (int)(*initialCandidates).size();
    for (int i = 0; i < nCandidates; i++){
        // For every word in the vector of possible words
        
        bool possibleCandidate = true;
        
        string curr = (*initialCandidates)[i];
        
        for (int j = 0; j < len; j++){
            // For every letter in the three words
            if (isalpha(currTranslation[j])){
                // If the current translation character is a letter (known letter)
                
                if ( ! isalpha(cipherWord[j])){
                    // If the cipher word isn't a letter at this character, return empty vector
                    return vector<string>();
                }
                
                if (curr[j] != currTranslation[j]){
                    // If the current character of the current word isn't the same letter, break the loop and
                    // set possibleCandidate to false so that the word will not be added to the finalCandidates
                    possibleCandidate = false;
                    break;
                }
                
            } else if (currTranslation[j] == '?'){
                // If the current translation is a ? (unknown letter)
                
                if ( ! isalpha(cipherWord[j])){
                    // If the cipher word isn't a letter at this character, return empty vector
                    return vector<string>();
                }
                
                if ( ! isalpha(curr[j])){
                    // If the current character of the current word isn't a letter, break the loop and
                    // set possibleCandidate to false so that the word will not be added to the finalCandidates
                    possibleCandidate = false;
                    break;
                }
                
            } else if (currTranslation[j] == '\'') {
                // If the current translation is an apostrophe
                
                if (cipherWord[j] != '\''){
                    // If the cipher word isn't an apostrophe at this character, return empty vector
                    return vector<string>();
                }
                
                if (curr[j] != '\''){
                    // If the current character of the current word isn't also an apostrophe, break the loop and
                    // set possibleCandidate to false so that the word will not be added to the finalCandidates
                    possibleCandidate = false;
                    break;
                }
                
            } else {
                // If there is somehow some other character here, return an empty vector
                return vector<string>();
            }
        }
        
        // For each possible word, if it made it through every letter without anything making it not a possible
        // candidate, add it to the finalCandidates vector
        if (possibleCandidate){
            finalCandidates.push_back(curr);
        }
        
    }
    
    // Return all candidates that match the current translation and cipher word
    return finalCandidates;
}


bool WordListImpl::viableWord(string &s)
{
    for (int i = 0; i < s.size(); i++){
        // Change every letter in the word to lower case, as all functions are case-insensitive and this makes
        // comparisons easier
        s[i] = tolower(s[i]);
        if ( ! islower(s[i]) && s[i] != '\''){
            // If a character isn't a letter (checking lower works) and also isn't an apostrophe, return false
            // to signify it is not a valid word
            return false;
        }
    }
    
    return true;
}


string WordListImpl::getKey(string s) const
{
    int strLength = (int)s.size();
    string key = "";
    
    // For every character in the word
    for (int i = 0; i < strLength; i++){
        if (s[i] == '\''){
            key += '\'';
            continue;
        }
        // Set that character to A-Z based on its location in the word
        key += ('A' + i);
        for (int j = 0; j < i; j++){
            // Check all previous letters in the word
            // If equal to a previous letter, change the letter in the key to the same as the one given to the earlier
            // letter.  Compare both as lower case letters as it should be case-insensitive
            if (tolower(s[i]) == tolower(s[j])){
                key[i] = key[j];
                break;
            }
        }
    }
    
    // Returns a key that is of a certain letter pattern
    return key;
}



// HASH FUNCTIONS

unsigned int hash(const    std::string&    s)
{
    return (int)std::hash<std::string>()(s);
}
unsigned int hash(const    int&    i)
{
    return (int)std::hash<int>()(i);
}
unsigned int hash(const    char&    c)
{
    return (int)std::hash<char>()(c);
}


//******************** WordList functions ************************************
// This class simply delegates all tasks to the WordListImpl class.
// Done this way since this was a class project, and this allowed for simpler and universal testing.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
   return m_impl->findCandidates(cipherWord, currTranslation);
}

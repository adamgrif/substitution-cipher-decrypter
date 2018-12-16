#include "provided.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const string SEPARATORS = "0123456789 ,;:.!()[]{}-\"#$%^&";

class DecrypterImpl
{
public:
    DecrypterImpl();
    ~DecrypterImpl();
    bool load(string filename);
    vector<string> crack(const string& ciphertext);
private:
    WordList* m_wl;
    Translator* m_translator;
    Tokenizer* m_tokenizer;
    
    void decryptWords(vector<string> cipherWords, vector<string>& solutions, const string& message);
    int largestUnknownIndex(const vector<string>& cipherwords) const;
    bool possibleTranslation(const vector<string>& translatedWords, bool& solution);
    
};

DecrypterImpl::DecrypterImpl()
: m_wl(new WordList), m_translator(new Translator), m_tokenizer(new Tokenizer(SEPARATORS)){}

DecrypterImpl::~DecrypterImpl()
{
    delete m_wl;
    delete m_translator;
    delete m_tokenizer;
}

bool DecrypterImpl::load(string filename)
{
    // The loadWordList() function first discards any old list of words, then returns true if it can load the words,
    // false otherwise
    return m_wl->loadWordList(filename);
}

vector<string> DecrypterImpl::crack(const string& ciphertext)
{
    // Break up the message into the words
    vector<string> cipherWords = m_tokenizer->tokenize(ciphertext);
    
    vector<string> solutions;   // To hold all possible solutions
    
    // If there are only separators (or empty string), just return the message
    if (cipherWords.size() == 0){
        solutions.push_back(ciphertext);
        return solutions;
    }
    
    // Call recursive function to find all possible solutions
    decryptWords(cipherWords, solutions, ciphertext);
    
    // Put in alphabetical order
    sort(solutions.begin(), solutions.end());
    
    return solutions;  // Return all possible solutions
}


void DecrypterImpl::decryptWords(vector<string> cipherWords, vector<string>& solutions, const string& message)
{
    // Step 2
    // Choose the word with the largest number of unknown letters and erase it from the words that haven't been chosen
    int index = largestUnknownIndex(cipherWords);
    string current = cipherWords[index];
    cipherWords.erase(cipherWords.begin() + index);
    
    // Step 3
    // Translate the current word with the current mapping
    string translation = m_translator->getTranslation(current);
    
    // Step 4
    // Find all possible words that match the cipher pattern and the partial translation
    vector<string> candidates = m_wl->findCandidates(current, translation);
    
    // Step 5
    // If there are no possible candidates, return to previous call
    int nCandidates = (int)candidates.size();
    if (nCandidates == 0){
        return;
    }
    
    // Step 6
    for (int i = 0; i < nCandidates; i++){
        // For every candidate word
        
        // part a
        // Try to add a new mapping based on the current candidate word
        if ( ! m_translator->pushMapping(current, candidates[i])){
            // If adding the mapping for the current word can't be done (overlapping letter mapping), move onto next word
            continue;
        }
        
        // part b
        // Translate the whole message based on the current mapping
        string msgTranslation = m_translator->getTranslation(message);
        
        // Break the now (partially) translated message into its words
        vector<string> translatedWords = m_tokenizer->tokenize(msgTranslation);
        bool isSolution = true;
        
        // part c
        // Check all fully translated words
        if ( ! possibleTranslation(translatedWords, isSolution)){
            // case i
            // A fully translated word is not in the list of words, get rid of that mapping and move to the next cadidate
            m_translator->popMapping();
            continue;
        }
        
        
        if (isSolution){
            // case iii
            // If all words were fully translated and in the word list, add the message to the vector of solutions
            // and get rid of the current mapping
            solutions.push_back(msgTranslation);
            m_translator->popMapping();
        } else {
            // case ii
            // Otherwise, some words are not fully translated but those that are are in the map,
            // recursively call this function, building upon the current map
            decryptWords(cipherWords, solutions, message);
            // Once the recursive call returns, get rid of the current mapping
            m_translator->popMapping();
        }
    }
}


int DecrypterImpl::largestUnknownIndex(const vector<string>& cipherWords) const
{
    // Find the word with the largest number of untranslated letters
    int largestIndex = 0;
    int nWords = (int)cipherWords.size();
    int nUnknownMax = 0;
    for (int j = 0; j < cipherWords[0].size(); j++){
        nUnknownMax++;
    }
    for (int i = 1; i < nWords; i++){
        int nUnknownCurr = 0;
        for (int j = 0; j < cipherWords[i].size(); j++){
            nUnknownCurr++;
        }
        if (nUnknownCurr > nUnknownMax){
            largestIndex = i;
            nUnknownMax = nUnknownCurr;
        }
    }
    
    // Return the index of the word with the most unknown letters
    return largestIndex;
}

bool DecrypterImpl::possibleTranslation(const vector<string>& translatedWords, bool& solution)
{
    int words = (int)translatedWords.size();
    for (int j = 0; j < words; j++){
        // For every word in the message
        bool fullyTranslated = true;
        string currWord = translatedWords[j];
        for (int k = 0; k < translatedWords[j].size(); k++){
            // For every letter in the word
            if (currWord[k] == '?'){
                // If part of the word isn't translated yet, we haven't reached a solution and this word is not
                // fully translated
                solution = false;
                fullyTranslated = false;
                // Move onto the next word
                break;
            }
        }
        
        if (fullyTranslated){
            // If the current word is fully translated
            if ( ! m_wl->contains(currWord)){
                // If the word is not in the list, we have not reached a solution, and return false to signify the
                // current mapping is wrong
                solution = false;
                return false;
            }
        }
    }
    
    // All fully translated words in the map are in the list, this mapping is still a possibility
    return true;
}



//******************** Decrypter functions ************************************
// This class simply delegates all tasks to the DecrypterImpl class.
// Done this way since this was a class project, and this allowed for simpler and universal testing.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
   return m_impl->crack(ciphertext);
}

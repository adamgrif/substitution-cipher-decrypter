#include "provided.h"
#include <string>
#include <vector>
using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);
    vector<string> tokenize(const std::string& s) const;
private:
    string m_separators;
    int m_nSeparators;
    
    bool isSeparator(const char a) const;
};

TokenizerImpl::TokenizerImpl(string separators)
{
    m_separators = separators;
    m_nSeparators = (int)separators.size();
}

vector<string> TokenizerImpl::tokenize(const std::string& s) const
{
    int strLength = (int)s.size();
    vector<string> v;           // Vector to be returned
    string curr = "";           // Current stretch of string since last separator
    for (int i = 0; i < strLength; i++){
        // For every character in the string
        if (isSeparator(s[i])){
            // If it is a separator
            if (curr != ""){
                // If the current stretch of string since the last separator is not empty, add it to the vector
                v.push_back(curr);
                curr = "";          // Set the current stretch of string to empty
                continue;
            }
            // Otherwise, the stretch of string is empty, just continue onto the next character
            continue;
        }
        
        // If it isn't a separator, add it to the current stretch of string
        curr += s[i];
    }
    
    // In the case that the string does not end in a separator, add the last part to the vector
    if (curr != ""){
        v.push_back(curr);
    }
    
    return v;
}


bool TokenizerImpl::isSeparator(const char a) const
{
    // For every separator checks if the character is equal to the separator
    for (int i = 0; i < m_nSeparators; i++){
        if (m_separators[i] == a){
            return true;
        }
    }
    return false;
}

//******************** Tokenizer functions ************************************
// This class simply delegates all tasks to the TokenizerImpl class.
// Done this way since this was a class project, and this allowed for simpler and universal testing.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const std::string& s) const
{
    return m_impl->tokenize(s);
}

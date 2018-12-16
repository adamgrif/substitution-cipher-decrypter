#include "provided.h"
#include <string>
#include <list>
using namespace std;


class TranslatorImpl
{
public:
    TranslatorImpl();
    bool pushMapping(string ciphertext, string plaintext);
    bool popMapping();
    string getTranslation(const string& ciphertext) const;
    
private:
    string m_currMap;
    
    list<string> m_maps;
    
};

TranslatorImpl::TranslatorImpl()
{
    for (int i = 0; i < 26; i++){
        m_currMap += '?';
    }
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
    int letters = (int)ciphertext.size();
    if (letters != plaintext.size()){
        // Both parameters should be the same length to do any mapping
        return false;
    }
    
    string newMap = m_currMap;       // To hold the new mapping
    
    for (int i = 0; i < letters; i++){
        // For every letter to be mapped
        
        // No need to check for mapping on an apostrophe
        if (ciphertext[i] == '\''){
            continue;
        }
        
        // The mapping is done as an indexing of a string 0-25 : A-Z
        // Meaning that whatever is at position 0 is what A maps to, whatever is at spot 2 is what C maps to, etc
        int index = toupper(ciphertext[i]) - 'A';
        
        if ( ! isalpha(ciphertext[i]) || ! isalpha(plaintext[i])){
            // If there is a non-letter/apostrophe in either string, return false
            return false;
        }
        
        
        // Checking for overlapping letter mapping
        for (int j = 0; j < 26; j++){
            // For every letter in the current mapping
            
            if (m_currMap[j] == '?'){
                // If the current mapping is a ?, continue onto the next character
                continue;
            }
            
            if ((m_currMap[index] != '?' && toupper(plaintext[i]) != m_currMap[index]) || (toupper(plaintext[i]) == m_currMap[j] && index != j)){
                // Protects against multiple letters mapping to the same letter, and also one letter mapping to multiple
                return false;
            }
        }
        
        // Make all uppercase to be consistent, should be case-insensitive anyway
        ciphertext[i] = toupper(ciphertext[i]);
        plaintext[i] = toupper(plaintext[i]);
        
        
        newMap[index] = plaintext[i];
    }
    
    // Add the current map to the front of the list
    m_maps.push_front(m_currMap);
    
    // Change the current map to the new one, which has the additional mappings
    m_currMap = newMap;
    
    return true;
}

bool TranslatorImpl::popMapping()
{
    // You cannot pop more than you can push/you cannot pop when the list is empty
    if (m_maps.size() == 0){
        return false;
    }
    
    // Change the current map to the one at the front of the list
    m_currMap = m_maps.front();
    m_maps.erase(m_maps.begin());
    
    return true;
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
    string result;
    
    int len = (int)ciphertext.size();
    for (int i = 0; i < len; i++){
        // For every character in the cipher text
        
        if (isalpha(ciphertext[i])){
            // If it's a letter, check it against the current mapping
            
            // The index is where the mapping is located for a given letter.
            // If the current letter (ciphertext[i]) is A (or a), index will be 0, which is where the mapping for
            // A is located in the currMap
            int index = toupper(ciphertext[i]) - 'A';
            if (m_currMap[index] != '?'){
                // If there is a mapping for the current ciphertext letter, meaning there isn't a ? in that index,
                // change the result letter to the plaintext letter
                result += m_currMap[index];
                
                // If the ciphertext letter was lower case, change the result letter to lower case (default in upper)
                // Case sensitive string will be returned
                if (islower(ciphertext[i])){
                    result[i] = tolower(result[i]);
                }
                
            } else {
                // Otherwise it is a ?, so just add a ? to the result
                result += '?';
            }
        } else {
            // Otherwise it is some other character, add that character to the result
            result += ciphertext[i];
        }
    }
    
    // Return case sensitive translated word
    return result;
}


//******************** Translator functions ************************************
// This class simply delegates all tasks to the TranslatorImpl class.
// Done this way since this was a class project, and this allowed for simpler and universal testing.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}

#ifndef PROVIDED_INCLUDED
#define PROVIDED_INCLUDED

#include <string>
#include <vector>


/*
This header file was provided in the development of this project.  It merely regulated how the classes were to be developed.
The <className>Impl were used to make grading easier.
*/

class TokenizerImpl;

class Tokenizer
{
public:
    Tokenizer(std::string separators);
    ~Tokenizer();
    std::vector<std::string> tokenize(const std::string& s) const;
    // Tokenizer objects cannot be copied or assigned
    Tokenizer(const Tokenizer&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;
private:
    TokenizerImpl* m_impl;
};

class WordListImpl;

class WordList
{
public:
    WordList();
    ~WordList();
    bool loadWordList(std::string filename);
    bool contains(std::string word) const;
    std::vector<std::string> findCandidates(std::string cipherWord, std::string currTranslation) const;
    // WordList objects cannot be copied or assigned
    WordList(const WordList&) = delete;
    WordList& operator=(const WordList&) = delete;
private:
    WordListImpl* m_impl;
};

class TranslatorImpl;

class Translator
{
public:
    Translator();
    ~Translator();
    bool pushMapping(std::string ciphertext, std::string plaintext);
    bool popMapping();
    std::string getTranslation(const std::string& ciphertext) const;
    // Translator objects cannot be copied or assigned
    Translator(const Translator&) = delete;
    Translator& operator=(const Translator&) = delete;
private:
    TranslatorImpl* m_impl;
};

class DecrypterImpl;

class Decrypter
{
public:
    Decrypter();
    ~Decrypter();
    bool load(std::string filename);
    std::vector<std::string> crack(const std::string& ciphertext);
    // Decrypter objects cannot be copied or assigned
    Decrypter(const Decrypter&) = delete;
    Decrypter& operator=(const Decrypter&) = delete;
private:
    DecrypterImpl* m_impl;
};

#endif // PROVIDED_INCLUDED

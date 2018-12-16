#include "provided.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <random>
#include <algorithm>
#include <numeric>
using namespace std;

const string WORDLIST_FILE = "/Users/adamgriffin/Desktop/CS 32/Project 4/Project 4/wordlist.txt";

// Encrypt plaintext string with random substitution permutation
string encrypt(string plaintext)
{
    // Generate a to z
    char plaintextAlphabet[26+1];
    iota(plaintextAlphabet, plaintextAlphabet+26, 'a');
    plaintextAlphabet[26] = '\0';
    
    // Generate permutation
    string ciphertextAlphabet(plaintextAlphabet);
    default_random_engine e((random_device()()));
    shuffle(ciphertextAlphabet.begin(), ciphertextAlphabet.end(), e);
    
    // Run translator (opposite to the intended direction)
    Translator t;
    t.pushMapping(plaintextAlphabet, ciphertextAlphabet);
    return t.getTranslation(plaintext);
}

// Decrypt cipertext into all possible permutations
bool decrypt(string ciphertext, int& count)
{
    Decrypter d;
    if ( ! d.load(WORDLIST_FILE))
    {
        // If not able to load list of words
        cout << "Unable to load word list file " << WORDLIST_FILE << endl;
        return false;
    }
    
    // Crack text to produce all possible translations
    for (const auto& s : d.crack(ciphertext)){
        cout << s << endl;
        // Count number of translations
        count++;
    }
    return true;
}

int main()
{
    // Encrypt the message being entered? Anything besides 'y' means no
    cout << "Encrypt the message first? (y/n) ";
    string encryptFirst;
    getline(cin, encryptFirst);
    
    // Enter message to be (encrypted and) decrypted
    cout << "Enter your message: ";
    string message;
    getline(cin, message);
    
    if (encryptFirst == "y"){
        message = encrypt(message);
        cout << "Randomly encrypted message: " << message << endl;
    }
    
    cout << "Possible translations:" << "\n\n";
    int count = 0;
    decrypt(message, count);
    cout << "\nThere are " << count << " possible translations." << endl;
}


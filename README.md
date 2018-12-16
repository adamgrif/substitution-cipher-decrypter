# substitution-cipher-decrypter
Takes a substitution cipher and deciphers it to print out all possible English translations.

Subsitution cipher: Every letter has a pairing to a (not necessarily different) random letter in the alphabet.  Message is encrypted so that every later is replaced with its paired letter.

Program can be used with a non-encrypted message input, and the program will encrypt the message using a randomly generated substitution cipher (provides a random permutation of the alphabet to pair each letter).  The program can also be used with already-encrypted messages.  The input message will be decrypted and all possible translations (as there can be many possible translations for smaller messages) will be printed to standard output, along with the final number of possible translations.

Developed in C++ as part of a class project. Makefile was created separately, as code was developed and built in Xcode, but should function on Linux operating systems.

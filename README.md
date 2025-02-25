# Preface
The goal of this assignment was divided into two parts: first, to decipher a monoalphabetic substitution cipher using cryptanalysis, and second, to create two functions to encrypt and decrypt a given text with the same key.

This whole assignment was written in C++20 using the standard library, and should not rely on outside dependencies. The compilation instructions should be right below, and the rest of this document outlines my thought process while working on the whole assignment.
# Environments & Compilation Instructions
### General Environment
Should be able to compile and run in C++20 on most systems.
```sh
clang -std=c++20 main.cpp -o main
./main

clang -std=c++20 ciphertools.cpp -o ciphertools
./ciphertools
```
### Personal Environment (Apple Silicon)
Working on an Apple Silicon M2 Chip device.
- Compiled in C++20 by linking standard C++ libraries via LLVM and Homebrew.
```sh
clang-19 -std=c++20 main.cpp -L/opt/homebrew/opt/llvm/lib -lc++ -o main
./main

clang-19 -std=c++20 ciphertools.cpp -L/opt/homebrew/opt/llvm/lib -lc++ -o ciphertools
./ciphertools
```
# Part One: Solving the Cipher
### The Intuition
My first idea was to find the letter and n-gram frequencies found in large English texts (such as modern literature and novels), substitute the cipher by matching frequencies of letters, then to use a sliding window to dictionary lookup and mutate the cipher as it goes.

I separated the processing and main program into two files: `process.cpp` and `main.cpp`, as I assumed that processing the data was going to unnecessarily take a lot of time (it really only took 3 seconds anyway). 

When selecting the text, I primarily used novels from the 19th and 20th century, and manually stripped the table of contents and additional credits to preserve n-gram accuracy. I decided to process the frequencies of n-grams up to 5-grams, which covered letter frequencies (unigrams), bigrams, trigrams, and etc.
# Part Two: Simple Substitution Cipher Tools
This part was straightforward, since all I needed to do was encrypt the plaintext and revert that substitution for the decryption part. 
- For each of the encryption and decryption functions, I provided the alphabet, its permutation (key), and the plaintext to be passed in.
- To mutate the plaintext, I simply iterated through each character in the string, matched the indexes to where that character is found in the alphabet or key (dependent on function), then replaced that character from one to the other.
	- Encryption: Mutated characters by index in `alphabet -> key`
	- Decryption: Mutated characters by index in `key -> alphabet`
	- All non-alphabetic characters were ignored and unchanged.
TODO link to ciphertools
# The Program
### Main Program
The main program (`main.cpp`) runs through the program.
### Cipher Tools (Encryption and Decryption)
The program `ciphertools.cpp` showcases the encryption and decryption of the ciphertext provided in `plaintext_code.txt`.
- For each entry from the file, the program outputs the original text, encrypted text, and decrypted text in order.
- The alphabet used for both functions was the string `ABCDEFGHIJKLMNOPQRSTUVWXYZ`, for the purpose of index matching with the key.
- The selected key used for encryption is `QWERTYUIOPASDFGHJKLZXCVBNM` (keyboard layout), since it was simple and provided a sufficient permutation of the alphabet.
### Data Processing
The program `process.cpp` was used to process the n-gram frequencies for literature texts in `/data/books/` via hash tables. These tables were then stored individually by n-grams in `/data/frequencies/`.

Files in `/data/frequencies/` are named in format `[n-gram]_freq.txt`, by their common alias.
- Table entries are stored by line, where each `[key]` and `[value]` pair are separated by a space. The contents are handled by custom functions within my main program.
# Resources
The English dictionary used for lookups was stored in the file `words_alpha.txt`, which was imported from https://github.com/dwyl/english-words.
- This file contains a list of english words that contained only alphabetic characters.

All plaintext book files used for processing in `/data/books/` were retrieved from [Project Gutenberg](https://www.gutenberg.org/). My method of selection was primarily preference, targeting popular modern (19th century or later) books and texts. 
- The table of contents and credits were stripped from each plaintext file for better n-gram accuracy.

List of texts used:
1. [Moby Dick; Or, The Whale](https://www.gutenberg.org/ebooks/2701) by Herman Melville
2. [The Great Gatsby](https://www.gutenberg.org/ebooks/64317) by F. Scott Fitzgerald
3. [Adventures of Huckleberry Finn](https://www.gutenberg.org/ebooks/76) by Mark Twain
4. [Pride and Prejudice](https://www.gutenberg.org/ebooks/1342) by Jane Austen
5. [Alice's Adventures in Wonderland](https://www.gutenberg.org/ebooks/11) by Lewis Carroll

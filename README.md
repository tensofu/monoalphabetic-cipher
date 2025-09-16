# Preface
The goal of this assignment was divided into two parts: first, to decipher a simple substitution cipher using cryptanalysis, and second, to create two functions to encrypt and decrypt a given text with the same key.

This whole assignment was written in C++20 using the standard library, and should not rely on outside dependencies. The compilation instructions should be right below, and the rest of this document outlines my thought process while working on the whole assignment.

The working solution for part 1 is in `main.cpp`, and the working solution for part 2 is in `ciphertools.cpp`. I left my initial attempt in `old.cpp`.
# Environments & Compilation Instructions
### General Environment
Should be able to compile and run in C++20 on most systems (untested). 
```sh
# final solution
clang -std=c++20 main.cpp -o main
./main

# cipher tools (part 2)
clang -std=c++20 ciphertools.cpp -o ciphertools
./ciphertools
```
### Personal Environment (Apple Silicon)
Working on an Apple Silicon M2 Chip device.
- Compiled in C++20 by linking standard C++ libraries via LLVM and Homebrew. (workaround for arm64 linker errors)
```sh
clang-19 -std=c++20 main.cpp -L/opt/homebrew/opt/llvm/lib -lc++ -o main
./main

clang-19 -std=c++20 ciphertools.cpp -L/opt/homebrew/opt/llvm/lib -lc++ -o ciphertools
./ciphertools
```
# Part One: Solving the Cipher
### The Intuition
The assumption was that all of the cipher text were a form of a mono-alphabetic simple substitution encryption. So, I wanted to make a single program that attempted to solve all provided ciphertext.
### First Approach
My first idea was to find the letter and n-gram frequencies found in large English texts (such as modern literature and novels), substitute the cipher by matching frequencies of letters, then to use a sliding window to dictionary lookup and lock mappings, then mutate the cipher as it goes.
 - The problem with this idea was that by locking the mappings, I was not able to explore other combinations, and there was a high possibility that I could lock in incorrect mappings, which could leave the decrypted text still look like a mix of scrambled words. 
 - I didn't really implement this through, as the extent to which I tried decoding the message was through matching letter frequencies alone.
### Second Approach
So, the next approach was to then create a bigram matrix that stored frequency values from 0-1, where rows signified the first letter of the bigram, and columns that signified the second letter. I would create one for the expected frequencies, generated from the bigram frequencies I processed beforehand, and processed another for a testing key. Then, I scored the keys based on the sum of differences between the testing bigram and the expected bigram.
- This implementation and algorithm followed an approach I read from a research paper[^1].
- This managed to produce slightly better results, but still only a few words were even recognizable. This may be due to two possible reasons: the cipher text may have been too short to come to an accurate analysis (the paper found better results the longer the text is, in which the length > 100), or the logic in my program was flawed.
- The code for this implementation is in `old.cpp`.
### Final Solution
Lastly, I did some more research online to discover more methods and algorithms to perform the cryptanalysis. The method I chose to implement was a simulated annealing technique, which mimicked the heating and cooling of a metal in order to come to a stable (accurate) form. Through studying the implementation of the algorithm with research articles[^2], along with coding assistance from AI, I came to a working implementation of the technique as my final solution.
- The scoring method utilized log likelihood by matching each n-gram between the cipher text and expected text.
	- Based on the length of the n-gram, they were assigned a certain weight (higher n-grams get more weight). If there is a match, then this weight is multiplied by the log of its probability. We will accumulate the score for every n-gram present in the ciphertext, and use that as the score for a given key.
	- Higher values (less negative) are intended to be more similar to English text.
- To implement simulated annealing, the program basically went through `n` iterations, and randomized the key slightly based off the best one, and reevaluates the score to find an improvement. If there are no iterations in 5,000 iterations, then the program significantly randomizes the key and attempts to move out of the local optimum.
	- The rate at which the program takes worse solution decreases over time until it is reset (so you do not get stuck in one spot)
- With five texts, every time the program ran, it had the chance to produce random results. In most of the runs tested, it managed to accurately solve ciphertext #3 and #4 >90% of the time, while #1 and #2 were left only partially decrypted with <30% accuracy.
	- When processing with ten different texts, the results were practically the same as before.

The best decrypted text generated for each cipher were the following (also found in `decrypted_[n].txt`, where only #3 and #4 were consistently eligible text:
1. `BSSIFANIGRAISHAIVEMYRIWARDSENSITHOUSEEORATALBANYOSPONSINLOCKSENSITHOUSEEORMRBATHN`
2. `AMASANISFROMPANDTREEDICANTOIKNEGODICANTODOMYGEUODTHYAMAMUSTERAMAMUSTERICANTOINCEGGHBYITROICANTOOLOGRAMYINK`
3. `WEAREASSEMBLEDHERETODAYTOPAYFINALRESPECTSTOOURHONOREDDEADANDYETITSHOULDBENOTEDTHATINTHEMIDSTOFOURSORROWTHISDEATHTAKESPLACEINTHESHADOWOFNEWLIFETHESUNRISEOFANEWWORLDAWORLDTHATOURBELOVEDCOMRADEGAVEHISLIFETOPROTECTANDNOURISHHEDIDNOTFEELTHISSACRIFICEAVAINOREMPTYONEANDWEWILLNOTDEBATEHISPROFOUNDWISDOMATTHESEPROCEEDINGSOFMYFRIENDICANONLYSAYTHISOFALLTHESOULSIHAVEENCOUNTEREDINMYTRAVELSHISWASTHEMOSTHUMAN`
4. `TOBEORNOTTOBETHATISTHEQUESTIONWHETHERTISNOBLERINTHEMINDTOSUFFERTHESLINGSANDARROWSOFOUTRAGEOUSFORTUNEORTOTAKEARMSAGAINSTASEAOFTROUBLESANDBYOPPOSINGENDTHEMTODIETOSLEEPNOMOREANDBYASLEEPTOSAYWEENDTHEHEARTACHEANDTHETHOUSANDNATURALSHOCKSTHATFLESHISHEIRTOTISACONSUMMATIONDEVOUTLYTOBEWISH`
### Limitations
The sample size I used to process the data was relatively small, since I only processed the n-gram frequencies for ten texts. If I processed more frequencies, the program may be able to more accurately score the decrypted text to come towards a better result. However, I didn't want to go past ten texts since it would take up more space, and it may lead to memory limitations when trying to process all the data in the program (especially when loading four-grams and five-grams). 

In addition, since I only selected novels from around the 19th century, it doesn't really account for texts outside of the scope of these novels. So, the expected frequencies for things like inspirational quotes or names (such as countries, cities, etc.) may not be reflected well in the processed data. I also had a fear that mixing the scope of data with low sample sizes may lead to unintended side effects (inconsistency), and may not generate a consistent representation of English text.
# Part Two: Simple Substitution Cipher Tools
This part was straightforward, since all I needed to do was encrypt the plaintext and revert that substitution for the decryption part. 
- For each of the encryption and decryption functions, I provided the alphabet, its permutation (key), and the plaintext to be passed in.
- To map the plaintext, I simply iterated through each character in the string, matched the indexes to where that character is found in the alphabet or key (dependent on function), then replaced that character from one to the other.
	- Encryption: Mapped characters by index from `alphabet -> key`.
	- Decryption: Mapped characters by index from `key -> alphabet`.
	- All alphabetic characters were switched to uppercase for simplicity.
	- All non-alphabetic characters were ignored and not disturbed.
- The key used for both functions was `QWERTYUIOPASDFGHJKLZXCVBNM` for simplicity.
# The Program
I separated the processing and main program into two files: `process.cpp` and `main.cpp`, as I assumed that processing the data was going to unnecessarily take a lot of time (it really only took around 5 seconds). The header file `helper.h` contained functions that would help in both `main.cpp` and `process.cpp`.
### Main Program
The main program (`main.cpp`) runs through the program, and attempted to decipher all of the encrypted code. It utilizes the simulated annealing technique to test and iterate through randomly different keys, and scoring each key to keep the one that provide the best score. Since it generally gets stuck in a local maxima, it occasionally restarts to repeat the iteration to find other local maxima in hopes to reach the most optimal one.
- The program stores the best results of each code in the form `decrypted_[n].txt`, where `n` refers to the code that is tested in the nth run. It shows the initial cipher, and the best keys and decrypted text given the best (highest) score. 
### Cipher Tools (Encryption and Decryption)
The program `ciphertools.cpp` showcases the encryption and decryption of the ciphertext provided in `plaintext_code.txt` by doing a simple matching substitution.
- For each entry from the file, the program outputs the original text, encrypted text, and decrypted text in order.
- The alphabet used for both functions was the string `ABCDEFGHIJKLMNOPQRSTUVWXYZ`, for the purpose of index matching with the key.
- The selected key used for encryption is `QWERTYUIOPASDFGHJKLZXCVBNM` (keyboard layout), since it was simple and provided a sufficient permutation of the alphabet. If used for a more practical use, then randomly generated keys would be much better.
### Data Processing
The program `process.cpp` was used to process the counted n-gram frequencies for literature texts in `/data/books/` via hash tables. These tables were then stored individually by n-grams in `/data/frequencies/`.

Files in `/data/frequencies/` are named in format `[n-gram]_freq.txt`, by their common alias.
- Table entries are stored by line, where each `[key]` and `[value]` pair are separated by a space. The contents are handled by custom functions within my helper functions and main program.
# Resources
### Text Files
The English dictionary used for lookups was stored in the file `words_alpha.txt`, which was imported from https://github.com/dwyl/english-words.
- This file contains a list of english words that contained only alphabetic characters.
- In the end, this file wasn't really used in either of my solutions.

All plaintext book files used for processing in `/data/books/` were retrieved from [Project Gutenberg](https://www.gutenberg.org/). My method of selection was primarily preference, targeting popular and more modern (19th century or later) books and texts. 
- The table of contents and credits were stripped from each plaintext file for better n-gram accuracy.

List of texts used:

1. [Moby Dick; Or, The Whale](https://www.gutenberg.org/ebooks/2701) by Herman Melville
2. [The Great Gatsby](https://www.gutenberg.org/ebooks/64317) by F. Scott Fitzgerald 
3. [Pride and Prejudice](https://www.gutenberg.org/ebooks/1342) by Jane Austen
4. [Alice's Adventures in Wonderland](https://www.gutenberg.org/ebooks/11) by Lewis Carroll
5. [Crime and Punishment](https://www.gutenberg.org/ebooks/2554) by Fyodor Dostoyevsky
6. [Great Expectations](https://www.gutenberg.org/ebooks/1400) by Charles Dickens
7. [Metamorphosis](https://www.gutenberg.org/ebooks/5200) by Franz Kafka
8. [Wuthering Heights](https://www.gutenberg.org/ebooks/768) by Emily Brontë
9. [The Wonderful Wizard of Oz](https://www.gutenberg.org/ebooks/55) by L. Frank Baum
10. [Jane Eyre: An Autobiography](https://www.gutenberg.org/ebooks/1260) by Charlotte Brontë
11. ~~[Adventures of Huckleberry Finn](https://www.gutenberg.org/ebooks/76) by Mark Twain~~ (Removed since there were plenty of uncommon words).
# References

[^1]: Jakobsen, T. (1995). A fast method for cryptanalysis of substitution ciphers. *Cryptologia, 19*(3), 265-274. https://doi.org/10.1080/0161-119591883944.

[^2]: Dimitris Bertsimas. John Tsitsiklis (1993). "Simulated Annealing." Statist. Sci. 8 (1) 10-15. https://doi.org/10.1214/ss/1177011077.

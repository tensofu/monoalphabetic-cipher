#include <iostream>
#include <iomanip>
#include <numeric>

#include "helper.h"

// temp addition from helper.h
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// Gathers file contents from directory
std::string get_directory_contents(const std::string directoryname) {
  std::cout << "Accessing directory " << directoryname << std::endl;
  std::string string = "";

  for (const auto& entry : fs::directory_iterator(directoryname)) {
    if (fs::is_regular_file(entry.status())) {
      string += get_file_contents(entry.path());
    }
  }
  return string;
}


// Stores as hashtable with specified name
void store_frequency_table(std::unordered_map<std::string, int> &map, std::string name) {
  std::string filename = "data/frequencies/" + name;
  std::ofstream file(filename);

  if (file.is_open()) {
    for (const auto &pair : map) {
      file << pair.first << " " << pair.second << std::endl;
    }
    file.close();
  } else {
    std::cerr << "Unable to open file for writing: " << filename << std::endl;
  }
}

void count_frequencies_words(std::unordered_map<std::string, int> &map, std::string &text) {
  std::string word;
  for (const char c : text) {
    if (std::isalpha(c)) {
      word += std::toupper(c);
    } else {
      if (word.size() != 0) {
        map[word] ++;
        word = "";
      }
    }
  }
}

int main() {
  // Starts timer
  auto start = std::chrono::high_resolution_clock::now();
  std::cout << "Processing the data files..." << std::endl;

  // Concatenates all plaintext to be processed into a single string
  std::string text = get_directory_contents("data/books/");

  // Hash maps for n-gram frequencies
  std::unordered_map<std::string, int> letter_freq;
  std::unordered_map<std::string, int> bigram_freq;
  std::unordered_map<std::string, int> trigram_freq;
  std::unordered_map<std::string, int> fourgram_freq;
  std::unordered_map<std::string, int> fivegram_freq;
  std::unordered_map<std::string, int> words_freq;

  // Prepares the text to be encrypted
  std::cout << "Normalizing text..." << std::endl;
  std::string cipher_text = normalize_text(text);

  // Processes the frequencies
  std::cout << "Processing frequency tables..." << std::endl;
  count_frequencies(letter_freq, cipher_text, 1);
  count_frequencies(bigram_freq, cipher_text, 2);
  count_frequencies(trigram_freq, cipher_text, 3);
  count_frequencies(fourgram_freq, cipher_text, 4);
  count_frequencies(fivegram_freq, cipher_text, 5);
  count_frequencies_words(words_freq, text);

  // Normalizes the frequencies
  // std::cout << "Normalizing frequency tables..." << std::endl;
  // std::unordered_map<std::string, float> normalized_letter_freq = normalize_frequencies(letter_freq);
  // std::unordered_map<std::string, float> normalized_bigram_freq = normalize_frequencies(bigram_freq);
  // std::unordered_map<std::string, float> normalized_trigram_freq = normalize_frequencies(trigram_freq);
  // std::unordered_map<std::string, float> normalized_fourgram_freq = normalize_frequencies(fourgram_freq);
  // std::unordered_map<std::string, float> normalized_fivegram_freq= normalize_frequencies(fivegram_freq);

  // Stores the frequencies
  std::cout << "Storing frequency tables..." << std::endl;
  store_frequency_table(letter_freq, "letter_freq.txt");
  store_frequency_table(bigram_freq, "bigram_freq.txt");
  store_frequency_table(trigram_freq, "trigram_freq.txt");
  store_frequency_table(fourgram_freq, "fourgram_freq.txt");
  store_frequency_table(fivegram_freq, "fivegram_freq.txt");
  store_frequency_table(words_freq, "words_freq.txt");


  std::cout << "Done!" << std::endl;
  
  // Ends time and prints elapsed time
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << std::fixed << std::setprecision(6) <<  "Elapsed Time: " << elapsed_seconds.count()  << " seconds" << std::endl;

  return 0;
}
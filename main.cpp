#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

#include "helper.h"


// Decrypts the plaintext with a given key and frequences
std::string decipher(std::string &encrypted_text, const std::string &key, const std::string &freq) {
  std::string decrypted_text = "";
  for (char c : encrypted_text) {
    char upper_c = std::toupper(c);
    if (std::isalpha(upper_c)) {
      size_t index = key.find(upper_c);
      if (index != std::string::npos) {
        decrypted_text += freq[index];
      } else {
        decrypted_text += c;
      }
    } else {
      decrypted_text += c;
    }
  }
  return decrypted_text;
}

// Mutates alphabet
std::string mutate_alphabet(std::vector<std::pair<std::string, int>> &vector) {
  const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::string mutated_key;
  for (const auto& element : vector) {
    if (element.first.size() != 1) {
      std::cerr << "The vector passed in must contain pairs of letter frequencies." << std::endl;
      return alphabet;
    }
    mutated_key += element.first;
  }

  std::set<char> mutated_key_letters;
  for (char c : mutated_key) {
    mutated_key_letters.insert(c);
  }

  std::string missing_letters = "";
  for (char c : alphabet) {
    if (mutated_key_letters.find(c) == mutated_key_letters.end()) {
      missing_letters += c;
    }
  }
  mutated_key += missing_letters;
  return mutated_key;
}

// Reads file into hash table
std::unordered_map<std::string, int> convert_file_to_table(std::string filename) {
  std::unordered_map<std::string, int> map;
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string key;
      std::string value_str;

      if (iss >> key >> value_str) {
        int value = std::stoi(value_str);
        map[key] = value;
      }
    }
  }
  return map;
}


// Reads hash table into vector pair
std::vector<std::pair<std::string, int>> convert_table_to_vector(std::unordered_map<std::string, int> &map) {
  std::vector<std::pair<std::string, int>> vector;
  for (const auto &pair : map) {
    vector.push_back({pair.first, pair.second});
  }

  std::sort(vector.begin(), vector.end(), [](const auto &a, const auto &b) {
    return a.second > b.second;
  });

  return vector;
}

// Reads file into vector pair
std::vector<std::pair<std::string, int>> convert_file_to_vector(std::string filename) {
  std::vector<std::pair<std::string, int>> vector;
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string key;
      std::string value_str;

      if (iss >> key >> value_str) {
        int value = std::stoi(value_str);
        vector.push_back({key, value});
      }
    }
  }
  std::sort(vector.begin(), vector.end(), [](const auto &a, const auto &b) {
    return a.second > b.second;
  });

  return vector;
}

void normalize_vector_cipher(std::vector<std::string> &vector) {
  for (std::string &element : vector) {
    element = normalize_text(element);
  }
}

// Outputs the vector of strings
void display_vector(std::vector<std::string> &vector) {
  for (std::string element : vector) {
    std::cout << element << std::endl;
  }
}

// Outputs the vectors of pairs
void display_vector_pair(std::vector<std::pair<std::string, int>> vector) {
  for (const auto& pair : vector) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
}

int main() {
  // Starts timer
  auto start = std::chrono::high_resolution_clock::now();

  std::vector<std::string> codes;
  
  append_file_to_vector(codes, "data/crypto_code.txt");
  
  std::cout << "Reading frequency tables from files..." << std::endl;
  std::vector<std::pair<std::string, int>> letter_freq = convert_file_to_vector("data/frequencies/letter_freq.txt");
  std::vector<std::pair<std::string, int>> bigram_freq = convert_file_to_vector("data/frequencies/bigram_freq.txt");
  std::vector<std::pair<std::string, int>> trigram_freq = convert_file_to_vector("data/frequencies/trigram_freq.txt");
  std::vector<std::pair<std::string, int>> fourgram_freq = convert_file_to_vector("data/frequencies/fourgram_freq.txt");
  std::vector<std::pair<std::string, int>> fivegram_freq = convert_file_to_vector("data/frequencies/fivegram_freq.txt");
  
  display_vector_pair(letter_freq);
  
  // Attempting decryption...
  normalize_vector_cipher(codes);
  std::cout << "Attempting decryption..." << std::endl;
  

  std::string common_freq_english = mutate_alphabet(letter_freq);
  std::cout << common_freq_english << std::endl;

  
  // Simple substitution
  for (std::string &code : codes) {
    std::unordered_map<std::string, int> letter_freq_key;
    count_frequencies(letter_freq_key, code, 1);
    std::vector<std::pair<std::string, int>> letter_freq_key_vector = convert_table_to_vector(letter_freq_key);

    std::string common_freq_key = mutate_alphabet(letter_freq_key_vector);
    std::cout << "Key:" << common_freq_key << std::endl;  

    code = decipher(code, common_freq_key, common_freq_english);
    std::cout << code << std::endl;
  }

  std::cout << "Done!" << std::endl;

  // Ends time and prints elapsed time
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << std::fixed << std::setprecision(6) <<  "Elapsed Time: " << elapsed_seconds.count()  << " seconds" << std::endl;

  return 0;
}
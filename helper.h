#ifndef HELPER_H
#define HELPER_H

#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <sstream>
#include <set>

namespace fs = std::filesystem;

// Decrypts the plaintext with a given key and frequencies
std::string decipher(const std::string &cipher_text, const std::string &cipher_key, const std::string &expected_key) {
  std::string decipher;
  for (char c : cipher_text) {
    if (std::isalpha(c)) {
      size_t index = cipher_key.find(std::toupper(c));
      if (index != std::string::npos) {
        decipher += expected_key[index];
      } else {
        decipher += c;
      }
    } else {
      decipher += c;
    }
  }
  return decipher;
}

// Stores file contents into a string
std::string get_file_contents(const std::string filename) {
  std::cout << "Accessing contents of '" << filename << "'... ";
  std::ifstream in(filename, std::ios::in | std::ios::binary);

  std::string contents;
  in.seekg(0, std::ios::end);
  contents.resize(in.tellg());
  in.seekg(0, std::ios::beg);
  in.read(&contents[0], contents.size());
  in.close();

  std::cout << "Success!" << std::endl;
  return(contents);
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


// Mutates by letter frequency
std::string generate_key(std::vector<std::pair<std::string, int>> &vector) {
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

// Appends strings from file to existing vector
void append_file_to_vector(std::vector<std::string>& vector, std::string filename) {
  std::ifstream file(filename);

  if (file.is_open()) {
    std::string text;
    while (std::getline(file, text)) {
      if (!text.empty()) {
        vector.push_back(text);
      }
    }
  } else {
    std::cerr << "File cannot be opened: " << filename << std::endl;
  }

  file.close();
}

// Removes all non-alphabetic characters and sets to uppercase to fit cipher format
std::string normalize_text (std::string text) {
  std::string result;
  result.reserve(text.length());
  for (char c : text) {
    if (std::isalpha(c)) {
      result += std::toupper(c);
    }
  }
  return result;
}

// Processes the maps and obtains n-gram frequencies from a text using a sliding window
void count_frequencies(std::unordered_map<std::string, int> &map, const std::string &text, int n) {
  std::string window;
  for (int i = 0; i < text.length() - n; i++) {
    map[text.substr(i, n)] ++;
  }
}

// Generates a vector that normalizes the frequencies from 0-1
std::unordered_map<std::string, double> normalize_frequencies(std::unordered_map<std::string, int> &map) {
  std::unordered_map<std::string, double> normalized_map;
  int sum = std::accumulate(map.begin(), map.end(), 0, [](int s, const auto &pair){return s + pair.second;});

  for (const auto &pair : map) {
    normalized_map[pair.first] = static_cast<double>(pair.second) / sum;
  }

  return normalized_map;
}

void normalize_vector_cipher(std::vector<std::string> &vector) {
  for (std::string &element : vector) {
    element = normalize_text(element);
  }
}


// Displays map of any type
template <typename K, typename V>
void display_map(std::unordered_map<K, V> &map) {
  for (const auto &pair : map) {
    std::cout << pair.first << ": " << pair.second << '\n';
  }
}

#endif
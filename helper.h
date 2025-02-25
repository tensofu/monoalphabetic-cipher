#ifndef HELPER_H
#define HELPER_H

#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <numeric>

namespace fs = std::filesystem;

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

// Displays map of any type
template <typename K, typename V>
void display_map(std::unordered_map<K, V> &map) {
  for (const auto &pair : map) {
    std::cout << pair.first << ": " << pair.second << '\n';
  }
}

// Processes the maps and obtains n-gram frequencies from a text using a sliding window
void count_frequencies(std::unordered_map<std::string, int> &map, const std::string &text, int n) {
  std::string window;
  for (int i = 0; i < text.length(); i++) {
    window.push_back(text[i]);
    if (window.size() == n) {
      map[window]++;
      window.erase(window.begin());
    }
  }
}

// Generates a vector that normalizes the frequencies from 0-1
std::unordered_map<std::string, float> normalize_frequencies(std::unordered_map<std::string, int> &map) {
  std::unordered_map<std::string, float> normalized_map;
  int sum = std::accumulate(map.begin(), map.end(), 0, [](int s, const auto &pair){return s + pair.second;});

  for (const auto &pair : map) {
    normalized_map[pair.first] = static_cast<float>(pair.second) / sum;
  }

  return normalized_map;
}

#endif
#include <iostream>
#include <cmath>
#include <algorithm>

#include "helper.h"

// I will need to import letter_freq.txt and bigram_freq.txt, and normalize the bigram matrix.
// Then, build those into a bigram matrix.
// Rows should be bigram[0], columns bigram[1]. (first and second)
// Add them in alphabetical order. A->Z for rows and columns

// Creates a bigram frequency matrix, where the rows represents the first letter, and the columns the second.
// Utilizes hash tables of normalized frequencies <string, double>
std::vector<std::vector<double>> create_bigram_matrix(const std::unordered_map<std::string, double> &freq_map) {
  std::vector<std::vector<double>> matrix(26, std::vector<double>(26, 0));

  for (const auto &[key, value] : freq_map) {
    matrix[key[0] - 'A'][key[1] - 'A'] = value;
  }

  return matrix;
}

// Scores the value based on a given key, based on the ciphertext bigram frequency matrix and the expected bigram frequency matrix
double score(std::vector<std::vector<double>> &bigram_ciphertext, const std::vector<std::vector<double>> &bigram_expected) {
  if (bigram_ciphertext.size() != bigram_expected.size() || bigram_ciphertext[0].size() != bigram_expected[0].size()) {
    std::cerr << "Error: Matrices are not the same shape." << std::endl;
    return 0.0;
  }

  double sum = 0.0;
  int rows = bigram_ciphertext.size();
  int cols = bigram_ciphertext[0].size();

  for (int i = 0; i < rows ; i++) {
    for (int j = 0; j < cols; j++) {
      sum += std::abs(bigram_ciphertext[i][j] - bigram_expected[i][j]);
    }
  }

  return sum;
}

// swaps matrix rows
void swapMatrixRows(std::vector<std::vector<double>>& matrix, int row1, int row2) {
  // Check if row indices are within the matrix bounds
  if (row1 >= 0 && row1 < matrix.size() && row2 >= 0 && row2 < matrix.size()) {
      std::swap(matrix[row1], matrix[row2]);
  } else {
      std::cerr << "Error: Invalid row indices for swapping." << std::endl;
  }
}

// swaps matrix columns
void swapMatrixColumns(std::vector<std::vector<double>>& matrix, int col1, int col2) {
  // Check if column indices are within the matrix bounds
  if (matrix.empty() || col1 < 0 || col1 >= matrix[0].size() || col2 < 0 || col2 >= matrix[0].size()) {
      std::cerr << "Error: Invalid column indices for swapping." << std::endl;
      return;
  }

  for (size_t i = 0; i < matrix.size(); ++i) {
      std::swap(matrix[i][col1], matrix[i][col2]);
  }
}

// Swap two characters in string by indexes
void swap(std::string &str, int a, int b) {
  if (a >= 0 && b < str.length() && b >= 0 && a < str.length()) {
    std::swap(str[a], str[b]);
  } else {
    std::cerr << "Error: Invalid indices for swapping." << std::endl;
  }
}


int main() {
  // Starts the program timer
  auto start = std::chrono::high_resolution_clock::now();
  int a, b = 1; // variables for swap later

  // Loads the letter frequencies and bigram frequencies from files
  // Processes the data to create a key based on descending letter frequency, and matrix based on bigram frequencies.
  // They both should be normalized.
  std::cout << "Loading expected bigram matrix and letter frequencies..." << std::endl;

  // Letter Frequency Key as 'expected_freq_key'
  std::vector<std::pair<std::string, int>> letter_freq = convert_file_to_vector("data/frequencies/letter_freq.txt");
  std::string expected_freq_key = generate_key(letter_freq);

  // Bigram Frequency Matrix as 'expected_bigram_matrix'
  std::unordered_map<std::string, int> temp_bigram_freq = convert_file_to_table("data/frequencies/bigram_freq.txt");
  std::unordered_map<std::string, double> bigram_freq = normalize_frequencies(temp_bigram_freq);
  std::vector<std::vector<double>> expected_bigram_matrix = create_bigram_matrix(bigram_freq);


  // Reads the ciphertext from file into the vector 'codes'
  std::cout << "Reading code into file..." << std::endl;
  std::vector<std::string> codes;
  append_file_to_vector(codes, "data/crypto_code.txt");
  normalize_vector_cipher(codes);


  // Attempts decryption through all codes
  std::cout << "Attempting decryption..." << std::endl;
  
  for (std::string &code : codes) {
    // Variable to keep highest score with best cipher key
    double best_value = 100.0;
    std::string best_ciphertext_freq_key;
    std::string best_decrypted_text;
    std::string temp_code;

    // Starts the analysis for selected code
    std::cout << "Testing ciphertext: " << code << std::endl;
    std::unordered_map<std::string, int> temp_code_freq;

    // Generates an initial key based on the common frequencies as 'ciphertext_freq_key'
    count_frequencies(temp_code_freq, code, 1);
    std::vector<std::pair<std::string, int>> code_freq = convert_table_to_vector(temp_code_freq);
    std::string ciphertext_freq_key = generate_key(code_freq);


    std::cout << "New key: " << ciphertext_freq_key << std::endl;

    
    // Maps the code using the key (transforms code to different string) // might reuse
    temp_code = decipher(code, ciphertext_freq_key, expected_freq_key);
    std::cout << "New ciphertext: " << temp_code << std::endl;

    // Make the cipher bigram matrix
    std::unordered_map<std::string, int> temp_code_bigram_freq;
    count_frequencies(temp_code_bigram_freq, temp_code, 2);
    std::unordered_map<std::string, double> normalized_code_bigram_freq = normalize_frequencies(temp_code_bigram_freq);
    std::vector<std::vector<double>> code_bigram_matrix = create_bigram_matrix(normalized_code_bigram_freq); //reuse this one

    // FOR LOOP - modify ciphertext_freq_key at end based on value
    // 1. swap letters in key based on algorithm
    // 2. transform code again and score it
    // 3. compare best and current values, and keep the lowest

    while (b != 26) {
    // Scores the matrix then compares value with next value  (lower one stays)
      double current_value = score(code_bigram_matrix, expected_bigram_matrix);
      if (current_value < best_value) {
        best_value = current_value;
        best_ciphertext_freq_key = ciphertext_freq_key;
        best_decrypted_text = temp_code;
        std::cout << "New Lowest Score: " << best_value << std::endl;
      }

      // swaps should happen here
      swap(ciphertext_freq_key, a - 1, b + a - 1);
      a++;

      std::cout << "New Key: " << ciphertext_freq_key << std::endl;
      if (a + b > 26) {
        a = 1;
        b++ ;
        if (b == 25) {
          //terminate
          break;
        }
      }

      // // updates the bigram matrix
      // swapMatrixRows(code_bigram_matrix, a, b);
      // swapMatrixColumns(code_bigram_matrix, a, b);

      // Make the cipher bigram matrix
      std::unordered_map<std::string, int> temp_code_bigram_freq;
      count_frequencies(temp_code_bigram_freq, temp_code, 2);
      std::unordered_map<std::string, double> normalized_code_bigram_freq = normalize_frequencies(temp_code_bigram_freq);
      code_bigram_matrix = create_bigram_matrix(normalized_code_bigram_freq); //reuse this one

      // Maps the code using the key (transforms code to different string) // might reuse
      temp_code = decipher(code, ciphertext_freq_key, expected_freq_key);
    }

    std::cout << "Best Score: " << best_value << std::endl;
    std::cout << "Best Key: " << best_ciphertext_freq_key << std::endl;
    std::cout << "Best Decrypted Text: " << best_decrypted_text << std::endl;

    a = 1;
    b = 1;
  }



  std::cout << "Done!" << std::endl;

  // Ends time and prints elapsed time
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << std::fixed << std::setprecision(6) <<  "Elapsed Time: " << elapsed_seconds.count()  << " seconds" << std::endl;

  return 0;
}
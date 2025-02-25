#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

#include "helper.h"

// Applies monoalphabetic subsitution encryption to text with a given key
std::string encrypt_monoalphabetic(std::string& plaintext, const std::string& key) {
  const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::string cipher_text = "";
  for (char c : plaintext) {
    char upper_c = std::toupper(c);
    if (std::isalpha(upper_c)) {
      size_t index = alphabet.find(upper_c);
      if (index != std::string::npos) {
        cipher_text += key[index];
      } else {
        cipher_text += c;
      }
    } else {
      cipher_text += c;
    }
  }

  return cipher_text;
}

// Decrypts the plaintext with a given key
std::string decrypt_monoalphabetic(std::string& encrypted_text, const std::string& key) {
  const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::string decrypted_text = "";

  for (char c : encrypted_text) {
    char upper_c = std::toupper(c);
    if (std::isalpha(upper_c)) {
      size_t index = key.find(upper_c);
      if (index != std::string::npos) {
        decrypted_text += alphabet[index];
      } else {
        decrypted_text += c;
      }
    } else {
      decrypted_text += c;
    }
  }

  return decrypted_text;
}

int main() {
  // Starts timer
  auto start = std::chrono::high_resolution_clock::now();

  std::cout << "Starting the cipher tools..." << std::endl;
  std::cout << std::endl;

  // Key used for testing
  const std::string key = "QWERTYUIOPASDFGHJKLZXCVBNM";

  // Vector of constant strings (text to be encrypted)
  std::vector<std::string> texts;

  // Appends text from file to vector
  append_file_to_vector(texts, "data/plaintext_code.txt");

  // Loops through texts in vector and outputs original, encrypted, and decrypted text
  for (std::string text : texts) {
    std::cout << "Original Text:" << std::endl;
    std::cout << text << std::endl;

    // Encrypts text
    std::cout << "Encrypted Text:" << std::endl;
    std::string encrypted_text = encrypt_monoalphabetic(text, key);
    std::cout << encrypted_text << std::endl; 

    // Decrypts text
    std::cout << "Decrypted Text:" << std::endl;
    std::string decrypted_text = decrypt_monoalphabetic(encrypted_text, key);
    std::cout << decrypted_text << std::endl; 

    std::cout << std::endl;
  }  

  std::cout << "Done!" << std::endl;

  // Ends time and prints elapsed time
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << std::fixed << std::setprecision(6) <<  "Elapsed Time: " << elapsed_seconds.count()  << " seconds" << std::endl;

  return 0;
}
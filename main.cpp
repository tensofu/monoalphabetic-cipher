#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>

#include "helper.h"

/*
Normalizes a counted frequency table to a value between 0-1, by taking the sum of the counts and 
dividing the count of each entry by the sum, so the new value is the function 'value = count/sum'
*/
std::unordered_map<std::string, double> normalize_frequencies(const std::unordered_map<std::string, int> &freq_map) {
  std::unordered_map<std::string, double> normalized;
  double total = 0.0;
  
  for (const auto &[key, value] : freq_map) {
    total += value;
  }
  
  for (const auto &[key, value] : freq_map) {
    normalized[key] = static_cast<double>(value) / total;
  }
  
  return normalized;
}

// Apply a substitution key to generate decrypted text
std::string apply_key(const std::string &ciphertext, const std::string &key_from, const std::string &key_to) {
  std::string result = ciphertext;
  std::unordered_map<char, char> key_map;
  
  // Create mapping between the two keys
  for (int i = 0; i < key_from.size() && i < key_to.size(); i++) {
    key_map[key_from[i]] = key_to[i];
  }
  
  // Apply mapping to ciphertext
  for (char &c : result) {
    if (key_map.find(c) != key_map.end()) {
      c = key_map[c];
    }
  }
  
  return result;
}

// Score text based on normalized n-gram frequencies
double score_text(const std::string &text, 
                  const std::unordered_map<std::string, double> &bigram_freq, const std::unordered_map<std::string, double> &trigram_freq,
                  const std::unordered_map<std::string, double> &fourgram_freq, const std::unordered_map<std::string, double> &fivegram_freq) {

  /*
  This section scores n-grams based on log likelihood, based on the probabilities (frequencies)
  of the bigram showing up in English texts.

  Each n-gram is assigned a base weight depending on their length. As we check through each
  n-gram of length 2-5, we accumulate the score by taking the log of the probability/frequency
  multipled by its base weight, so we have the function 'score += weight * log(p)'. Each log
  evaluation added a very small number (1e-10) to avoid the calculation of log(0).

  The higher (less negative) the score, the more it lines up with my processed n-gram frequencies, 
  which should be a decent evaluation of how close it is to English text (in context of 19th century novels).
  */
 
 double score = 0.0;
 double bigram_weight = 1.0;
 double trigram_weight = 2.0;
 double fourgram_weight = 3.0;
 double fivegram_weight = 4.0;
 int n = 2;
  
  // Score bigrams
  for (int i = 0; i < text.length() - n; i++) {
    std::string bigram = text.substr(i, n);
    if (bigram_freq.find(bigram) != bigram_freq.end()) {
      score += bigram_weight * log(bigram_freq.at(bigram) + 1e-10);
    } else {
      score += bigram_weight * log(1e-10);
    }
  } n++;
  
  // Score trigrams
  for (int i = 0; i < text.length() - n; i++) {
    std::string trigram = text.substr(i, n); 
    if (trigram_freq.find(trigram) != trigram_freq.end()) {
        score += trigram_weight * log(trigram_freq.at(trigram) + 1e-10); 
    } else {
        score += trigram_weight * log(1e-10);
    }
  } n++;
  
  // Score fourgrams
  for (int i = 0; i < text.length() - n; i++) {
    std::string fourgram = text.substr(i, n);
    if (fourgram_freq.find(fourgram) != fourgram_freq.end()) {
      score += fourgram_weight * log(fourgram_freq.at(fourgram) + 1e-10); 
    } else {
      score += fourgram_weight * log(1e-10);
    }
  } n++;
  
  // Score fivegrams
  for (int i = 0; i < text.length() - n; i++) {
    std::string fivegram = text.substr(i, n);
    if (fivegram_freq.find(fivegram) != fivegram_freq.end()) {
      score += fivegram_weight * log(fivegram_freq.at(fivegram) + 1e-10); 
    } else {
      score += fivegram_weight * log(1e-10);
    }
  }
  return score; 
}

// Swap two characters in a string
void swap_chars(std::string &str, int a, int b) {
  if (a >= 0 && a < str.length() && b >= 0 && b < str.length()) {
    std::swap(str[a], str[b]);
  } else {
    std::cerr << "Error: Invalid indices for swapping: " << a << ", " << b << std::endl;
  }
}

// Simulated annealing algorithm utilizing multiple n-grams and cooling 
// (most of this part was assisted by AI, i added the comments)
std::string simulated_annealing(const std::string &ciphertext, const std::string &initial_key, const std::string &target_key,
                                const std::unordered_map<std::string, double> &bigram_freq,
                                const std::unordered_map<std::string, double> &trigram_freq,
                                const std::unordered_map<std::string, double> &fourgram_freq,
                                const std::unordered_map<std::string, double> &fivegram_freq,
                                int iterations = 50000) {

  /* Generates random numbers for simulation
  std::random_device rd:      Source of non-deterministic random numbers.
  std::mt19937 gen(rd()):     Generates a random number based on the Marsenne Twister algorithm, taking a seed from rd().
                              The seed essentially sets the starting point of a long sequence of random numbers.
  uniform_int_distribution:   Generates random integers between 0-25 (chars)
  uniform_real_distribution:  Generates random real numbers between 0.0-1.0 (probability)
  */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> char_dist(0, 25);
  std::uniform_real_distribution<> prob_dist(0.0, 1.0);
  
  /* Sets the initial temperature and defines the cooling rate
  temperature:  Signifies how wide the simulation will search for candidates (more likely to pick immediately worse solutions)
  cooling_rate: Signifies the rate at which the temperature decreases over time (more likely to converge to local optimum)
  */
  double temperature = 30.0;
  double cooling_rate = 0.9995;
    
  // Starts with the initial solution (key, score)
  std::string current_key = initial_key;
  std::string best_key = current_key;

  std::string current_decryption = apply_key(ciphertext, current_key, target_key);
  double current_score = score_text(current_decryption, bigram_freq, trigram_freq, fourgram_freq, fivegram_freq);
  double best_score = current_score;
  
  // Variables to track improvement. Resets the starting point if we hit max_no_improvement.
  int no_improvement_count = 0;
  const int max_no_improvement = 5000;
  double last_best_score = best_score;
    
  std::cout << "Starting simulated annealing with score: " << current_score << std::endl;
    
  // Main simulated annealing loop
  for (int iter = 0; iter < iterations; iter++) {
    // Modified neighbor generation strategy
    std::string neighbor_key = current_key;
    
    /* Occasionally tries more swaps at a time.
    80% of the time it will do a single swap between two letters
    10% of the time it will do two swaps between two pairs of letters
    10% of the time it will swap three characters in a cyclic rotation
    */ 
    double r = prob_dist(gen);
    if (r < 0.8) {  // 80% simple swap
      int pos1 = char_dist(gen);
      int pos2 = char_dist(gen);
      while (pos1 == pos2) {
        pos2 = char_dist(gen);
      }
      swap_chars(neighbor_key, pos1, pos2);
    } else if (r < 0.9) {  // 10% double swap
      for (int i = 0; i < 2; i++) {
        int pos1 = char_dist(gen);
        int pos2 = char_dist(gen);
        while (pos1 == pos2) {
          pos2 = char_dist(gen);
        }
        swap_chars(neighbor_key, pos1, pos2);
      }
    } else {  // 10% rotation of 3 characters
      int pos1 = char_dist(gen);
      int pos2 = char_dist(gen);
      int pos3 = char_dist(gen);
      
      char temp = neighbor_key[pos1];
      neighbor_key[pos1] = neighbor_key[pos2];
      neighbor_key[pos2] = neighbor_key[pos3];
      neighbor_key[pos3] = temp;
    }
        
    // Generates the decryption using the neighbor key, and evaluates the neighbor with a new score
    std::string neighbor_decryption = apply_key(ciphertext, neighbor_key, target_key);
    double neighbor_score = score_text(neighbor_decryption, bigram_freq, trigram_freq, fourgram_freq, fivegram_freq);
    
    // Decide whether to accept the neighbor
    bool accept = false;
    if (neighbor_score > current_score) {  
      accept = true;
    } else {
      // Accept worse solutions with decreasing probability as temp decreases
      double acceptance_probability = std::exp((neighbor_score - current_score) / temperature);
      accept = (prob_dist(gen) < acceptance_probability);
    }
    
    if (accept) {
      // Sets the better key as the current key
      current_key = neighbor_key;
      current_score = neighbor_score;
            
      // Update best solution if needed, then resets the no_improvement_count
      if (current_score > best_score) {
        best_score = current_score;
        best_key = current_key;
        no_improvement_count = 0;
                
        // Print progress for significant improvements
        if (best_score > last_best_score + 10.0 || iter % 1000 == 0) {
          std::cout << "Iteration "      << iter 
                    << ", Temperature: " << temperature 
                    << ", Score: "       << best_score << std::endl;
          std::string sample = apply_key(ciphertext.substr(0, 50), best_key, target_key);
          std::cout << "Sample decryption: " << sample << "..." << std::endl;
          last_best_score = best_score;
        }
      } else {
        no_improvement_count++;
      }
    } else {
      no_improvement_count++;
    }
        
    // Restart if stuck in a local optimum
    if (no_improvement_count > max_no_improvement) {
      std::cout << "No improvement for " << max_no_improvement << " iterations. Restarting..." << std::endl;
            
      // Mutate the current solution significantly by repeatedly swapping multiple times the best key before the next test
      std::string mutated_key = best_key;  // Restart from the best key found so far
      for (int i = 0; i < 16; i++) {  // More aggressive mutation
        int pos1 = char_dist(gen);
        int pos2 = char_dist(gen);
        swap_chars(mutated_key, pos1, pos2);
      }
            
      current_key = mutated_key;
      current_decryption = apply_key(ciphertext, current_key, target_key);
      current_score = score_text(current_decryption, bigram_freq, trigram_freq, fourgram_freq, fivegram_freq);
      no_improvement_count = 0;
      
      // Reheat the system
      temperature = temperature * 3.0;
      if (temperature > 30.0) temperature = 30.0;  // Caps the temperature at 30.0
    }
        
    // Cools down the temperature based on cooling rate (stabilizes)
    temperature *= cooling_rate;
        
    // Dynamically adjust cooling rate based on progress
    if (iter % 5000 == 0 && iter > 0) {
      if (best_score > last_best_score + 100.0) {
        // Slow down cooling if making good progress
        cooling_rate = std::min(cooling_rate * 1.001, 0.9999);
      } else {
        // Speed up cooling if progress is slow
        cooling_rate = std::max(cooling_rate * 0.999, 0.995);
      }
    }
  }
    
  std::cout << "Final best score: " << best_score << std::endl;
  std::string final_decryption = apply_key(ciphertext, best_key, target_key);
  std::cout << "Final decryption: " << final_decryption.substr(0, 100) << "..." << std::endl;
  
  return best_key;
}

// Function to run multiple independent annealing attempts
std::string multi_start_annealing(const std::string &ciphertext, const std::string &initial_key, const std::string &target_key,
                                  const std::unordered_map<std::string, double> &bigram_freq,
                                  const std::unordered_map<std::string, double> &trigram_freq,
                                  const std::unordered_map<std::string, double> &fourgram_freq,
                                  const std::unordered_map<std::string, double> &fivegram_freq,
                                  int num_starts = 3, int iterations_per_start = 20000) {
    
  // Stores the best keys and scores by run index
  std::vector<std::string> best_keys;
  std::vector<double> best_scores;
    
  // Mutex added to lock 
  std::mutex mtx;
  std::vector<std::thread> threads;
  
  for (int i = 0; i < num_starts; i++) {
    threads.push_back(std::thread([&, i]() {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> char_dist(0, 25);
      
      // Generate a different starting key for each run
      std::string start_key = initial_key;
      // Mutate the initial key
      for (int j = 0; j < 5; j++) {
        int pos1 = char_dist(gen);
        int pos2 = char_dist(gen);
        swap_chars(start_key, pos1, pos2);
      }
      
      std::cout << "Starting annealing run " << i+1 << " with mutated key" << std::endl;
      
      std::string result_key = simulated_annealing(
        ciphertext, 
        start_key, 
        target_key, 
        bigram_freq, 
        trigram_freq,
        fourgram_freq,
        fivegram_freq,
        iterations_per_start
      );
      
      std::string decrypted = apply_key(ciphertext, result_key, target_key);
      double score = score_text(decrypted, bigram_freq, trigram_freq, fourgram_freq, fivegram_freq);
      
      // Store result safely
      std::lock_guard<std::mutex> lock(mtx);
      best_keys.push_back(result_key);
      best_scores.push_back(score);
      
      std::cout << "Run " << i+1 << " completed with score: " << score << std::endl;
    }));
  }
  
  // Wait for all threads to complete
  for (auto &t : threads) {
    t.join();
  }
  
  // Find the best result
  int best_idx = 0;
  for (size_t i = 1; i < best_scores.size(); i++) {
    if (best_scores[i] > best_scores[best_idx]) {
      best_idx = i;
    }
  }
  
  return best_keys[best_idx];
}

// Function to convert n-gram frequencies from vector
std::unordered_map<std::string, double> load_ngram_frequencies(const std::vector<std::pair<std::string, int>> &ngram_vec) {
  std::unordered_map<std::string, int> ngram_map;
  
  for (const auto &[ngram, count] : ngram_vec) {
    ngram_map[ngram] = count;
  }
  
  return normalize_frequencies(ngram_map);
}

int main() {
  // Start timer
  auto start = std::chrono::high_resolution_clock::now();

  // Load n-gram frequency data
  std::vector<std::pair<std::string, int>> bigram_freq_vec = convert_file_to_vector("data/frequencies/bigram_freq.txt");
  std::vector<std::pair<std::string, int>> trigram_freq_vec = convert_file_to_vector("data/frequencies/trigram_freq.txt");
  std::vector<std::pair<std::string, int>> fourgram_freq_vec = convert_file_to_vector("data/frequencies/fourgram_freq.txt");
  std::vector<std::pair<std::string, int>> fivegram_freq_vec = convert_file_to_vector("data/frequencies/fivegram_freq.txt");
  
  // Normalize frequencies
  std::unordered_map<std::string, double> bigram_freq = load_ngram_frequencies(bigram_freq_vec);
  std::unordered_map<std::string, double> trigram_freq = load_ngram_frequencies(trigram_freq_vec);
  std::unordered_map<std::string, double> fourgram_freq = load_ngram_frequencies(fourgram_freq_vec);
  std::unordered_map<std::string, double> fivegram_freq = load_ngram_frequencies(fivegram_freq_vec);
  
  // Letter Frequency Key as 'expected_freq_key'
  std::vector<std::pair<std::string, int>> letter_freq = convert_file_to_vector("data/frequencies/letter_freq.txt");
  std::string expected_freq_key = generate_key(letter_freq);
  std::cout << "Expected frequency key: " << expected_freq_key << std::endl;
  
  // Read ciphertexts from file
  std::cout << "Reading ciphertexts from file..." << std::endl;
  std::vector<std::string> codes;
  append_file_to_vector(codes, "data/crypto_code.txt");
  normalize_vector_cipher(codes);
  
  // Process each ciphertext
  std::cout << "Attempting decryption on " << codes.size() << " ciphertexts..." << std::endl;
  
  int code_num = 1;
  for (const std::string &code : codes) {
    std::cout << "\n======= Processing Ciphertext #" << code_num++ << " =======" << std::endl;
    std::cout << "Length: " << code.length() << " characters" << std::endl;
    std::cout << "Sample: " << code.substr(0, 50) << "..." << std::endl;
    
    // Generate initial key based on letter frequencies
    std::unordered_map<std::string, int> initial_freq;
    count_frequencies(initial_freq, code, 1);
    std::vector<std::pair<std::string, int>> code_freq = convert_table_to_vector(initial_freq);
    std::string initial_key = generate_key(code_freq);
    
    std::cout << "Initial frequency-based key: " << initial_key << std::endl;
    
    // Use multi-start simulated annealing
    std::string best_key = multi_start_annealing(code, initial_key, expected_freq_key, 
      bigram_freq, trigram_freq, fourgram_freq, fivegram_freq, 3, 20000
    );
    
    // Apply the best key to get the final decryption
    std::string decrypted_text = apply_key(code, best_key, expected_freq_key);
    
    // Output results
    std::cout << "\n----- Final Results -----" << std::endl;
    std::cout << "Best key: " << best_key << std::endl;
    std::cout << "Decrypted text: " << decrypted_text << std::endl;
    
    // Calculate n-gram statistics for the decrypted text
    double final_score = score_text(decrypted_text, bigram_freq, trigram_freq, fourgram_freq, fivegram_freq);
    std::cout << "Final quality score: " << final_score << std::endl;
    
    // Write results to file
    std::string output_filename = "decrypted_" + std::to_string(code_num-1) + ".txt";
    std::ofstream output_file(output_filename);
    if (output_file.is_open()) {
      output_file << "Original ciphertext:\n" << code << "\n\n";
      output_file << "Decryption key: " << best_key << " -> " << expected_freq_key << "\n\n";
      output_file << "Decrypted text:\n" << decrypted_text << std::endl;
      output_file << "\nQuality score: " << final_score << std::endl;
      output_file.close();
      std::cout << "Results saved to " << output_filename << std::endl;
    } else {
      std::cerr << "Unable to write results to file." << std::endl;
    }
  }
  
  // End timer and print elapsed time
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << std::fixed << std::setprecision(6) << elapsed_seconds << std::endl;
}
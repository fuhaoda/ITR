#ifndef __COMP_SEARCH_H__
#define __COMP_SEARCH_H__

#include <vector>
#include "Data.h"
#include "Types.h"

//
// Comprehensive search
//
class CompSearch {
public:
  // Constructor.
  CompSearch(const Data *data, unsigned depth, unsigned nthreads);

  // Run the comprehensive search.
  void run();

  // Report the best scores.
  rVector score(size_t nTop);

  // Report the variables associated with the best scores.
  uMaitrx var(size_t nTop);

  // Report the cut value associated with the i-th best score. 
  sVector cut(size_t i);

  // Report the cut direction associated with the i-th best score.
  sVector dir(size_t i); 

private:
  const Data *data_; 
  unsigned depth_;
  unsigned nthreads_;
  size_t total_choices_{0};

  struct Meta {
    // Choices of variables used in the search.
    size_t vidx[3];
    // Choices of cuts used in the search. 
    size_t cidx[3];
  };

  // Choices of all the variables and cuts combination used in the search.
  std::vector<Meta> choices_;

  // Values of the search scores.
  std::vector<double> scores_;

  // Position to store the next search choices.
  size_t iter_;

  // Number of scores that have been sorted.
  size_t ntop_;

  // Order of the sorted scores.
  std::vector<size_t> index_;

  // This function loops through all combinations formed by choosing `depth_`
  // number of variables out of all available variables. For each combination
  // found, depending on the mode, it will either call `count` or `set` method.
  void combination(std::vector<size_t> curr, std::vector<size_t> option,
                   char mode);

  // Counts the number of search options formed by the specified choice of
  // variables. 
  void count_choices(const std::vector<size_t> &choices);

  // Sets the number of search options formed by the specified choice of
  // variables.
  void set_choices(const std::vector<size_t> &choices);

  // Helper function for `set_choices`.
  void set_choices_helper(const std::vector<size_t> &vidx,
                          std::vector<size_t> cidx,
                          std::vector<size_t> &cbound,
                          size_t curr);

  // Worker function of the search
  void worker(size_t tid);   
}; 

#endif 

#ifndef __COMP_SEARCH_H__
#define __COMP_SEARCH_H__

#include <cstdint>
#include <cstring>
#include <set>
#include "data.h"
#include "types.h"

// Comprehensive search when the action choice is binary. 
class CompSearch {
public:
  // Constructor.
  CompSearch(unsigned depth, unsigned nthreads); 

  // Preprocess raw data set i.
  void preprocess(unsigned i); 
   
  // Run the comprehensive search.
  void run();

  // Report the best scores.
  rVector score(size_t ntop);

  // Report the variables associated with the best scores.
  uMatrix var(size_t ntop);

  // Report the cut value associated with the i-th best score. 
  sVector cut(size_t i);

  // Report the cut direction associated with the i-th best score.
  sVector dir(size_t i); 

private:
  unsigned depth_;
  unsigned nthreads_;
  size_t total_choices_{0};
  size_t nsample_;
  size_t ncont_;
  size_t nord_;
  size_t nnom_; 

  // Use 4 bits to store each action value. Type uint32_t is used such that 8
  // actions can be packed/unpacked at one time. 
  std::vector<std::uint32_t> act_;

  // Scaled response values.
  std::vector<double> resp_;

  // Summation of the responses where the corresponding action values are 0.
  double T0_; 
  
  struct Cvar {
    // Values of the cut for this variable.
    std::vector<int> value;

    // Masks of the components that belong to each cut.
    std::vector<std::vector<std::uint32_t>> mask;
  };

  // Cut and mask information for each variable.
  std::vector<Cvar> cvar_;

  // Return the value of cut `cidx` for variable `vidx`
  std::string cut_val(size_t vidx, size_t cidx) const;

  // Return the string representation for the cut direction of variable vidx
  std::string cut_dir(size_t vidx, size_t m) const; 
  
  // Decile values of each continuous variable.
  std::vector<std::vector<double>> decile_;

  // Unique values of each ordinal variable. 
  std::vector<std::set<int>> uniq_ord_;

  // Unique values of each nominal variable.
  std::vector<std::set<int>> uniq_nom_; 

  // Compress the raw action values.
  void pack_actions(const std::vector<int> &act);

  // Compute the scaled responses.
  void scale_response(const std::vector<double> &resp,
                      const std::vector<double> &prob); 
  
  // Set up the cut masks for continuous variable. 
  void set_cont_masks(const Data *data);

  // Set up the cut masks for ordinal variable. 
  void set_ord_masks(const Data *data);

  // Set up the cut masks for nominal variable. 
  void set_nom_masks(const Data *data);  

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

  // Helper function for various report functions
  void report_helper(size_t &ntop);  
}; 

#endif 

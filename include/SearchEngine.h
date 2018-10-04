#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include <vector>
#include "Data.h"

namespace ITR {

class SearchEngine {
public:
  // Constructor
  SearchEngine(const Data *data, unsigned depth, unsigned nThreads);
  
  // Return the number of search choices to examine
  size_t nChoices() const { return choices_.size() << depth_; }
  
  // Run the comprehensive search
  void run();
  
  // Report
  void report(size_t nTop) const;
  
private:
  const Data *data_;
  unsigned depth_;
  unsigned nThreads_;
  size_t totalChoices_{0};
  
  struct Meta {
    // Choices of vriables used in the search
    size_t vIdx[3];  
    // Choices of cuts used in the search
    size_t cIdx[3]; 
  };
  
  // Choices of all the variables and cuts combination used in the search
  std::vector<Meta> choices_;
  
  // Values of the search scores_
  std::vector<double> scores_;

  // Position to store the next search choices
  size_t iter_;   
  
  // This function will loop through all combinations formed by choosing 'depth'
  // number of variables out of all available variables. For each combination
  // found, depending on the mode, it will either call 'count' or 'set' method.
  void combination(std::vector<size_t> curr, std::vector<size_t> option,
                   char mode);
  
  // This function counts the number of search options formed by the specified
  // choices of variables
  void countChoices(const std::vector<size_t> &choices);
  
  // This function sets the number of search options formed by the specified
  // choices of variables
  void setChoices(const std::vector<size_t> &choices); //, size_t &iter);
  
  // This function is the helper function for setChoices
  void setChoicesHelper(const std::vector<size_t> &vIdx,
                        std::vector<size_t> cIdx,
                        std::vector<size_t> &cBound,
                        size_t curr); 
  
  // This function is the worker of the search
  void worker(size_t tid);
};

} // namespace ITR

#endif 

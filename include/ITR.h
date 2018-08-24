#ifndef __ITR_H__
#define __ITR_H__

#include <string>
//#include <memory> 
//#include "Data.h"
//#include "SearchEngine.h"

namespace ITR {

class ITR { // Individual Treatment Recommender 
public:
  // Constructor
  // input_file: path to load the input data
  // search_depth: depth of the search 
  ITR(std::string input_file, unsigned search_depth);

  // Run the comprehensive search
  void run();

  // Report the result of the comprehensive search
  void report();

  //private:
  //std::unique_ptr<Data> data_;
  //std::unique_ptr<SearchEngine> engine_; 
};

} // namespace ITR
  

#endif 


// #ifndef ITR_H
// #define ITR_H

// #include "Data.h"
// #include "DataInfo.h"
// #include "Res.h"

// class ITR
// {
// public:
//     ITR();
//     virtual ~ITR();
//     virtual void process() = 0;

// 	static void setData(Data* data);                         /**< Set data point */
// 	static void setVarType(vector<unsigned int> X);          /**< Set 1D data type vector */
// 	static void setX(vector<vector<unsigned int>> X);        /**< Set 2D X matrix */

//     static Data* getData();                      /**< Return data point */
//     static vector<vector<unsigned int>>& getX(); /**< Return processed 2D X matrix (int type) */
//     static double getT0();                       /**< Return sum (Y | A = 0) */
//     static vector<unsigned int> getVarType();    /**< Return 1D vector for variable type */
// 	static vector<Res *> getSearchResults();     /**< Return 1D vector for search results */

// 	// Functions defined for test purpose
// 	static unsigned int** get_var_A() { return var_A; };
// 	static bool*** get_Table_X() { return table_X; };
// 	static double** get_var_Y() { return var_Y; };
// 	static vector<DataInfo *> getDataInfo();     /**< Return 1D vector for data info */
// 	static unsigned int* getCutSize() { return cutSize; };

// protected:
//     static double T0;                      /**< Update by class "CreateLookUpTable", Total sum of Non Treatment*/
//     static unsigned int** var_A;           /**< Update by class "CreateLookUpTable". Action, 2d matrix  (sample_Size x action_Size)*/
//     static double** var_Y;                 /**< Update by class "CreateLookUpTable". Y, 2d matrix  (sample_Size x y_Size)*/
//     static bool*** table_X;                /**< Update by class "CreateLookUpTable". Lookup table, 3d matrix (var_Size x cut_Size[i] x sample_Size)*/
//     static vector<DataInfo *> info;        /**< Update by class "CreateLookUpTable". Variable info vector, (var_Size)*/
//     static unsigned int* cutSize;          /**< Update by class "CreateLookUpTable". Cut size vector, (var_Size) */
//     static vector<Res *> searchResults;    /**< Update by search classes */

// private:
// 	static Data* mydata;                         /**< Update by Class "LoadData" */
// 	static vector<unsigned int> var_Type;        /**< Update by class "CreateX", 1D vector for variable type (0,1,2)  */
// 	static vector<vector<unsigned int>> var_X;   /**< Update by class "CreateX", combined X  */
// };

// #endif // ITR_H

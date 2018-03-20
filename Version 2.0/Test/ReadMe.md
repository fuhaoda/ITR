### Unit Test
* Implement googletest for unit test (build-in in Visual Studio)
* Apply python for [result validation](https://github.com/JieXue328/ITR-Develop/tree/master/Version%202.0/Python%20Verification)

3 test datasets:  
* [Simple test data](https://github.com/JieXue328/ITR-Develop/blob/master/Version%202.0/Python%20Verification/TestBuildInSimpleData.csv) (1 ord)
* [CSV test data](https://github.com/JieXue328/ITR-Develop/blob/master/Version%202.0/Python%20Verification/TestSample_100_XTable.csv) (3 cont, 3 ord, 3 nom)
* [Simulated test data](https://github.com/JieXue328/ITR-Develop/blob/master/Version%202.0/Test/CreateSimulationData.h) (2 cont, 3 ord, 4 nom)
 
[30 tests for 8 test cases](test.cpp):       
1. [**DataInput**](LoadData.h)**Test**       
  1.1 **TestSimpleInputData**: validate input data for simple test data      
  1.2 **TestReadCSV_Size**: validate data size for CSV test data     
  1.3 **TestReadCSV_Value**: validate input data for CSV test data     
  1.4 **TestSimulationData_Size**: validate data size for simulated data   
2. [**LoadData**](LoadData.h)**Test**      
  2.1 **SimpleTestData**:  validate "class LoadData" for simple test data    
  2.2 **CSVData**:  validate "class LoadData" for CSV test data     
  2.3 **SimulationData**:  validate "class LoadData" for simulated data     
3. [**CreateX**](CreateX.h)**Test**                   
  3.1 **SimpleTestData**: validate "class CreateX" for simple test data     
  3.2 **CSVTestData**: validate "class CreateX" for CSV test data     
  3.3 **SimulationData**: validate "class CreateX" for simulated data    
4. [**CreateLookUpTableTest**](../Python%20Verification/CreateLookUpTable.h)**Test**      
  4.1 **SimpleTestData_Part_I**: validate _DataInfo_ for "class CreateLookUpTable" with simple test data     
  4.2 **SimpleTestData_Part_II**: validate lookup table for "class CreateLookUpTable" with simple test data    
  4.3 **CSVData_Part_I**: validate _DataInfo_ for "class CreateLookUpTable" with CSV test data    
  4.4 **CSVData_Part_II**: validate lookup table for "class CreateLookUpTable" with CSV test data    
  4.5 **SimulatedData**: validate _DataInfo_ for "class CreateLookUpTable" with simulated data    
5. [**OneDepthSearch**](OneDepthSearch.h)**Test**          
  5.1 **SimpleTestData_Part_I**: validate "class OneDepthSearch" for _print_ with simple test data     
  5.2 **SimpleTestData_Part_II**:  validate "class OneDepthSearch" for _output_ with simple test data    
  5.3 **CSVData_Part_I**: validate "class OneDepthSearch" for _print_ with CSV test data    
  5.4 **CSVData_Part_II**: validate "class OneDepthSearch" for _output_ with CSV test data    
  5.5 **SimulationTestData**:  validate "class OneDepthSearch" for _output_ with simulated data  
  [Python validation](../Python%20Verification/OneDepthSearch.ipynb)          
6. [**TwoDepthSearch**](TwoDepthSearch.h)**Test**      
  6.1 **CSVTestData**: validate "class TwoDepthSearch" for _output_ with CSV test data    
  6.2 **SimulationTestData**: validate "class TwoDepthSearch" for _output_ with simulated data  
  [Python validation](../Python%20Verification/TwoDepthSearch.ipynb)       
7. [**ThreeDepthSearch**](ThreeDepthSearch.h)**Test**         
  7.1 **CSVTestData**: validate "class ThreeDepthSearch" for _output_ with CSV test data    
  7.2 **SimulationTestData**: validate "class ThreeDepthSearch" for _output_ with simulated data   
  [Python validation](../Python%20Verification/ThreeDepthSearch.ipynb)     
8. [**FlexDepthSearch**](FlexDepthSearch.h)**Test**        
  8.1 **CSVTestData_1D**: validate "class FlexDepthSearch" for depth 1 with CSV data   
  8.2 **CSVTestData_2D**: validate "class FlexDepthSearch" for depth 2 with CSV data   
  8.2 **CSVTestData_3D**: validate "class FlexDepthSearch" for depth 3 with CSV data     
  8.4 **SimulatedTestData_1D**: validate "class FlexDepthSearch" for depth 1 with simulated data   
  8.5 **SimulatedTestData_2D**: validate "class FlexDepthSearch" for depth 2 with simulated data     
  8.6 **SimulatedTestData_3D**: validate "class FlexDepthSearch" for depth 3 with simulated data   
  [Python validation 1D](../Python%20Verification/FlexSearch_1D.ipynb)    
  [Python validation 2D](../Python%20Verification/FlexSearch_2D.ipynb)    
  [Python validation 3D](../Python%20Verification/FlexSearch_3D.ipynb)     
  
  
Reference for [unit testing](https://github.com/google/googletest)    
Reference for [google test](https://en.wikipedia.org/wiki/Unit_testing)

Please contact "Jie Xue" for more information.   
Email: <xue_jie@lilly.com>

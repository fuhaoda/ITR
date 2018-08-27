// #include "pch.h"
// #include "LoadCSVData.h"

// typedef vector <double> record_t;
// typedef vector <record_t> data_t;

// //-----------------------------------------------------------------------------
// // Let's overload the stream input operator to read a list of CSV fields (which a CSV record).
// // Remember, a record is a list of doubles separated by commas ','.
// istream& operator >> ( istream& ins, record_t& record )
// {
//     // make sure that the returned record contains only the stuff we read now
//     record.clear();

//     // read the entire line into a string (a CSV record is terminated by a newline)
//     string line;
//     getline( ins, line );

//     // now we'll use a stringstream to separate the fields out of the line
//     stringstream ss( line );
//     string field;
//     while (getline( ss, field, ',' ))
//     {
//         // for each field we wish to convert it to a double
//         // (since we require that the CSV contains nothing but floating-point values)
//         stringstream fs( field );
//         double f = 0.0;  // (default value is 0.0)
//         fs >> f;

//         // add the newly-converted field to the end of the record
//         record.push_back( f );
//     }

//     // Now we have read a single line, converted into a list of fields, converted the fields
//     // from strings to doubles, and stored the results in the argument record, so
//     // we just return the argument stream as required for this kind of input overload function.
//     return ins;
// }

// //-----------------------------------------------------------------------------
// // Let's likewise overload the stream input operator to read a list of CSV records.
// // This time it is a little easier, just because we only need to worry about reading
// // records, and not fields.
// istream& operator >> ( istream& ins, data_t& data )
// {
//     // make sure that the returned data only contains the CSV data we read here
//     data.clear();

//     // For every record we can read from the file, append it to our resulting data
//     record_t record;
//     while (ins >> record)
//     {
//         data.push_back( record );
//     }

//     // Again, return the argument stream as required for this kind of input stream overload.
//     return ins;
// }



// LoadCSVData::LoadCSVData(string fInput,bool header)
// {
//     //ctor
//     LoadCSVData::fileName = fInput;
//     LoadCSVData::isHeader = header;
// }

// LoadCSVData::~LoadCSVData()
// {
//     //dtor
// }

// void LoadCSVData::loadData()
// {
//     data_t data;

//     // Here is the file containing the data. Read it into data.
//     ifstream infile( fileName );
//     infile >> data;

//     // Complain if something went wrong.
//     if (!infile.eof())
//     {
//         cout << "Something went wrong!\n";
//     }
//     infile.close();

//     unsigned int rowN = data.size();
//     unsigned int i = isHeader?1:0;

//     vector<unsigned int> test_DataType{0,0,0,1,1,1,2,2,2};
//     vector<unsigned int> tmpAction;
//     vector<double> tmpY;
//     vector<double> tmpCont1;
//     vector<double> tmpCont2;
//     vector<double> tmpCont3;
//     vector<unsigned int> tmpOrd1;
//     vector<unsigned int> tmpOrd2;
//     vector<unsigned int> tmpOrd3;
//     vector<unsigned int> tmpNom1;
//     vector<unsigned int> tmpNom2;
//     vector<unsigned int> tmpNom3;

//     for(; i<rowN; ++i){
//         Data::id.push_back(data[i][0]);
//         tmpAction.push_back(data[i][10]);
//         tmpY.push_back(data[i][11]);
//         tmpCont1.push_back(data[i][1]);
//         tmpCont2.push_back(data[i][2]);
//         tmpCont3.push_back(data[i][3]);
//         tmpOrd1.push_back(data[i][4]);
//         tmpOrd2.push_back(data[i][5]);
//         tmpOrd3.push_back(data[i][6]);
//         tmpNom1.push_back(data[i][7]);
//         tmpNom2.push_back(data[i][8]);
//         tmpNom3.push_back(data[i][9]);
//     }

//     Data::sampleSize = isHeader?rowN-1:rowN;
//     Data::dataType = test_DataType;
//     Data::actions.push_back(tmpAction);
//     Data::y.push_back(tmpY);
//     Data::x_Cont.push_back(tmpCont1);
//     Data::x_Cont.push_back(tmpCont2);
//     Data::x_Cont.push_back(tmpCont3);
//     Data::x_Ord.push_back(tmpOrd1);
//     Data::x_Ord.push_back(tmpOrd2);
//     Data::x_Ord.push_back(tmpOrd3);
//     Data::x_Nom.push_back(tmpNom1);
//     Data::x_Nom.push_back(tmpNom2);
//     Data::x_Nom.push_back(tmpNom3);
// }

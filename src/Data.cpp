#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <bitset>
#include <sys/stat.h>
#include "Data.h"
#include "Covariate.h"

namespace ITR {

    Data::Data(const std::string &input) {
        struct stat buffer{};
        if (stat(input.c_str(), &buffer) != 0)
            throw "Input file does not exist!";

        // TODO:
        // Switch different load function based on the file format
        loadCSV(input);
    }

    void Data::loadCSV(const std::string &input) {
        // Open the input CSV file
        std::ifstream infile(input);

        // Parse the header of the input file
        parseCSVHeader(infile);

        // Allocate temporary buffer to load the raw data
        std::vector<std::vector<double>> cont(nCont_);
        std::vector<std::vector<int>> ord(nOrd_);
        std::vector<std::vector<int>> nom(nNom_);

        decile_.resize(nCont_);
        uniqOrd_.resize(nOrd_);
        uniqNom_.resize(nNom_);

        // Load raw data of the input file
        loadRawData(infile, cont, ord, nom);

        // Close the input file
        infile.close();

        // Parse raw data of the input file
        parseRawData(cont, ord, nom);
    }

    void Data::parseCSVHeader(std::ifstream &infile) {
        // This function counts the number of continuous, ordinal, nominal variables,
        // and the number of actions and responses. It requires the input file has
        // aligned format.
        std::string line;
        std::istringstream ss;
        std::string field;

        getline(infile, line);
        ss.str(line);

        nCont_ = nOrd_ = nNom_ = nResp_ = 0; //reset to zeros in case calling it again from other function.

        while (ss.good()) {
            getline(ss, field, ',');

            // Convert field to all CAPS
            std::transform(field.begin(), field.end(), field.begin(), ::toupper);
            if (field.find("CONT") != std::string::npos) {
                nCont_++;
            } else if (field.find("ORD") != std::string::npos) {
                nOrd_++;
            } else if (field.find("NOM") != std::string::npos) {
                nNom_++;
            } else if (field.find('Y') != std::string::npos) {
                nResp_++;
            }
        }

        nVar_ = nCont_ + nOrd_ + nNom_;
    }

    void Data::loadRawData(std::ifstream &infile,
                           std::vector<std::vector<double>> &cont,
                           std::vector<std::vector<int>> &ord,
                           std::vector<std::vector<int>> &nom) {
        std::string line;
        std::istringstream ss;
        std::string field;
        std::uint32_t val = 0;
        nSample_ = 0; //reset sample size equal to 0 in case calling this function multiple times

        //this function required data in a special format, starting with ID, continuous variables, ordinal, nominal categorical variables etc..
        while (getline(infile, line)) {

            ss.clear();
            ss.str(line);

            // Read subject ID
            getline(ss, field, ',');
            id_.push_back(stoi(field));

            // Read continuous variables
            for (size_t i = 0; i < nCont_; ++i) {
                getline(ss, field, ',');
                cont[i].push_back(stod(field));
            }

            // Read ordinal variables
            for (size_t i = 0; i < nOrd_; ++i) {
                getline(ss, field, ',');
                auto tempv = stoi(field);
                ord[i].push_back(tempv);
                uniqOrd_[i].insert(tempv);
            }

            // Read nominal variables
            for (size_t i = 0; i < nNom_; ++i) {
                getline(ss, field, ',');
                auto tempv = stoi(field);
                nom[i].push_back(tempv);
                uniqNom_[i].insert(tempv);
            }

            // Read action
            getline(ss, field, ',');
            int act=stoi(field);
            if(act!=0 && act!=1){
                std::cout<<"Action value should be either 0 or 1"<<std::endl;
                exit(-1);
            }
            // Each action is saved using 4 bits. The values are accumulated inside
            // 'val' and write to the container 8 at a time.
            val |= act << (28 - 4 * (nSample_ & 0x7));
            if ((nSample_ & 0x7) == 0x7) {
                act_.push_back(val);
                val = 0;
            }

            // Read responses
            for (size_t i = 0; i < nResp_; ++i) {
                getline(ss, field, ',');
                resp_.push_back(stod(field));
            }

            // Read P(A = 1| X)
            getline(ss, field, ',');
            double prAX = stod(field);
            if(prAX<0.0 ||prAX>1.0){
                std::cout << "The propensity score Pr(A=1|X) should be in between 0.0 and 1.0!" << prAX << std::endl;
                exit(-1);
            }
            prob_.push_back(prAX);

            ++nSample_;

        }

        // Write the final bits if the number of samples is not a multiple of 8
        if (nSample_ & 0x7)
            act_.push_back(val);
    }

    void Data::parseRawData(std::vector<std::vector<double>> &cont,
                            std::vector<std::vector<int>> &ord,
                            std::vector<std::vector<int>> &nom) {
        // Compute "scaled" response: Y / P(A | X) and T0
        // Assuming Y is a vector
        int r = static_cast<int>(nSample_ % 8);
        size_t nBatches = nSample_ >> 3;

        for (size_t i = 0; i < nBatches; ++i) {
            size_t i8 = i << 3;
            auto mask = act_[i];
            for (int k = 7; k >= 0; --k) {
                size_t idx = (i8 + k) * nResp_;
                double p = prob_[i8 + k];
                resp_[idx] /= (mask & 0xF ? p : 1 - p);
                T0_ += resp_[idx] * (1 - (mask & 0xF));
                mask >>= 4;
            }
        }

        if (r) {
            size_t i8 = nBatches << 3;
            auto mask = act_[nBatches];

            // Drop the bottom 32 - 4 * r bits that are all zero
            mask >>= (32 - 4 * r);

            for (int k = r - 1; k >= 0; --k) {
                size_t idx = (i8 + k) * nResp_;
                double p = prob_[i8 + k];
                resp_[idx] /= (mask & 0xF ? p : 1 - p);
                T0_ += resp_[idx] * (1 - (mask & 0xF));
                mask >>= 4;
            }
        }

        cvar_.resize(nVar_);

        // Parse continuous variables and set up cut masks
        // The parse variable order can't be changed.
        for (size_t i = 0; i < nCont_; ++i) {
            convertContToDeciles(cont[i], decile_[i]);
            setContCutMasks(i, cont[i]);
        }

        // Parse ordinal variables
        for (size_t i = 0; i < nOrd_; ++i) {
            convertOrdToRanks(ord[i], uniqOrd_[i]);
            setOrdCutMasks(i, ord[i]);
        }

        // Parse nominal variables
        for (size_t i = 0; i < nNom_; ++i) {
            if(uniqNom_[i].size()>(sizeof(int)*8-1)){
                std::cout << "Nominal Categorical Variable "<<i<<" has more than 31 categories!" << std::endl;
                exit(-1);
            }
            convertNomToBitMasks(nom[i], uniqNom_[i]);
            setNomCutMasks(i, nom[i]);
        }
    }

    void Data::setContCutMasks(size_t vIdx, const std::vector<double> &data) {
        // vIdx is the overall variable index, since we parse continuous variable first, vIdx can be used directly
       // Continuous variable has 10 cuts
        for (int value = 1; value <= 10; ++value) {
            size_t r = nSample_ % 8;
            size_t nBatches = nSample_ >> 3;
            std::vector<std::uint32_t> mask(nBatches + (r > 0));

            for (size_t j = 0; j < nBatches; ++j) {
                size_t j8 = j << 3;
                std::uint32_t val{0};
                // Mask for sample j8 is stored in bits 28-31
                // Mask for sample j8+1 is stored in bits 24-27
                // Mask for sample j8+2 is stored in bits 20-23
                // Mask for sample j8+3 is stored in bits 16-19
                // Mask for sample j8+4 is stored in bits 12-15
                // Mask for sample j8+5 is stored in bits 8-11
                // Mask for sample j8+6 is stored in bits 4-7
                // Mask for sample j8+7 is stored in bits 0-3
                for (size_t k = 0; k < 8; ++k)
                    val |= (data[j8 + k] < value) << (28 - 4 * k);

                mask[j] = val;
            }

            if (r) {
                size_t j8 = nBatches << 3;
                std::uint32_t val{0};
                for (size_t k = 0; k < r; ++k)
                    val |= (data[j8 + k] < value) << (28 - 4 * k);
                mask[nBatches] = val;
            }

            cvar_[vIdx].mask.push_back(mask);
            cvar_[vIdx].value.push_back(value - 1);
        }
    }

    void Data::setOrdCutMasks(size_t i, const std::vector<int> & data) {
        // Compute the overall variable index
        size_t vIdx = i + nCont_;

        // The number of cuts for ordinal variable vIdx is the number of the unique
        // values.
        for (const auto &value : uniqOrd_[vIdx - nCont_]) {
            size_t r = nSample_ % 8;
            size_t nBatches = nSample_ >> 3;
            std::vector<std::uint32_t> mask(nSample_ + (r > 0));

            for (size_t j = 0; j < nBatches; ++j) {
                size_t j8 = j << 3;
                std::uint32_t val{0};
                for (size_t k = 0; k < 8; ++k)
                    val |= (data[j8 + k] <= value) << (28 - 4 * k);

                mask[j] = val;
            }

            if (r) {
                size_t j8 = nBatches << 3;
                std::uint32_t val{0};
                for (size_t k = 0; k < r; ++k)
                    val |= (data[j8 + k] <= value) << (28 - 4 * k);
                mask[nBatches] = val;
            }
            cvar_[vIdx].mask.push_back(mask);
            cvar_[vIdx].value.push_back(value);
        }
    }

    void Data::setNomCutMasks(size_t i, const std::vector<int> & data) {
        // Compute the overall variable index
        size_t vIdx = i + nCont_ + nOrd_;


        // The number of cuts for nominal variable vIdx is the number of subsets
        // that contain no more than half of the unique values.

        // Denote the number of unique values by p. Here, we loop from 0 to 2^p. Each
        // iterator is interpreted as a bitmask, where bit j means the jth element in
        // the unique set. For each integer, if the number of 1 bits is no more than
        // half of p, it then represents a valid cut, and the subset consists of the
        // elements corresponding to the 1 bits in the integer.
        size_t p = uniqNom_[i].size();
        size_t max = 1u << p;
        size_t half = p / 2;

        for (size_t value = 0; value < max; ++value) {
            // The following works only if there are no more than 64 unique
            // values.
            std::bitset<64> subset(value);
            if (subset.count() <= half) {
                size_t r = nSample_ % 8;
                size_t nBatches = nSample_ >> 3;
                std::vector<std::uint32_t> mask(nBatches + (r > 0));

                for (size_t j = 0; j < nBatches; ++j) {
                    size_t j8 = j << 3;
                    std::uint32_t val{0};
                    for (size_t k = 0; k < 8; ++k)
                        val |= ((data[j8 + k] & value) > 0) << (28 - 4 * k);

                    mask[j] = val;
                }

                if (r) {
                    size_t j8 = nBatches << 3;
                    std::uint32_t val{0};
                    for (size_t k = 0; k < r; ++k)
                        val |= ((data[j8 + k] & value) > 0) << (28 - 4 * k);
                    mask[nBatches] = val;
                }
                cvar_[vIdx].mask.push_back(mask);
                cvar_[vIdx].value.push_back(static_cast<int>(value));
            }
        }
    }

    std::string Data::cutInfo(size_t vIdx, size_t cIdx, bool m) const {
        std::stringstream info;
        if (vIdx < nCont_) {
            info << " X" << vIdx << (m ? " < " : " >= ")
                 << decile_[vIdx][cIdx] << ", ";
        } else if (vIdx < nCont_ + nOrd_) {
            info << " X" << vIdx << (m ? " < " : " >= ")
                 << cvar_[vIdx].value[cIdx] << ", ";
        } else {
            info << " X" << vIdx << (m ? " in " : " not in ") << "{";
            auto subset = cvar_[vIdx].value[cIdx];

            if (subset == 0) {
                // This represents an empty set
                info << " }\n";
            } else {
                unsigned iter = 0;
                for (auto const &v : uniqOrd_[vIdx - nCont_ - nOrd_]) {
                    if (subset & (1 << (iter++)))
                        info << v << ", ";
                }
                info << "\b\b}, ";
            }
        }

        return info.str();
    }

} // namespace ITR

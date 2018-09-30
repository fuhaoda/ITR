#ifndef __ITR_H__
#define __ITR_H__

#include <string>
#include <vector>
#include <memory>
#include "Data.h"
#include "SearchEngine.h"

namespace ITR {

    class ITR {
    public:
        // Constructor
        explicit ITR(std::string input, unsigned depth = 3, unsigned nThreads = 1);

        // Run the comprehensive search
        void run() const;

        // Report the top n search results
        void report(size_t nTop = 1) const;

    private:
        std::unique_ptr<Data> data_;
        std::unique_ptr<SearchEngine> engine_;
    };

} // namespace ITR

#endif 



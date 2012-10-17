#ifndef __HISTMANAGER__
#define __HISTMANAGER__

#include <map>
#include <vector>
#include <string>

class TH1;
class TH1F;
class TH2F;

class HistManager
{
    public:
        HistManager();
        ~HistManager();

    public:
        /**
         * Looks up the histogram by name and if present
         * returns the histogram
         */
        TH1* getHistogram(std::string histName);

        /**
         * Return a vector of 1d histograms
         */
        std::vector<TH1F*> geth1List();

        /**
         * Return vector of 2d histograms
         */
        std::vector<TH2F*> geth2List();

        /**
         * Add 1d histogram
         */
        void Addh1(std::string &name, float xmin, float xmax, int nBins );

        /**
         * Add 2d histogram 
         */
        void Addh2(std::string &name, float xmin, float xmax, int nxBins, float ymin, float ymax, int nyBins);

    private:
        std::map<std::string, TH1F*> fh1_Collection;
        std::map<std::string, TH2F*> fh2_Collection;
};

#endif

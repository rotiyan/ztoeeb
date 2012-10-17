#include "HistManager.h"

#include <cstdlib>

#include "TH1F.h"
#include "TH1F.h"
#include "TH2F.h"
HistManager::HistManager()
{}

HistManager::~HistManager()
{}

void HistManager::Addh1(std::string &name, float xmin,float xmax, int nbins)
{
    TH1F* hist  = new TH1F(name.c_str(),name.c_str(),nbins,xmin,xmax);
    fh1_Collection.insert(std::pair<std::string,TH1F*>(name,hist));
}

void HistManager::Addh2(std::string &name, float xmin, float xmax, int nxBins, float ymin, float ymax, int nyBins)
{
    TH2F* hist  = new TH2F(name.c_str(),name.c_str(),nxBins, xmin, xmax, nyBins, ymin,ymax);
    fh2_Collection.insert(std::pair<std::string,TH2F*>(name,hist));
}

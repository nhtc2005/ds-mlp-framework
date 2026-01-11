#ifndef DSFACTORY_H
#define DSFACTORY_H

#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ann/functions.h"
#include "config/Config.h"
#include "dsaheader.h"
#include "loader/dataloader.h"
#include "loader/dataset.h"

using namespace std;

class DSFactory {
   public:
    DSFactory(string cfg_filename);
    DSFactory(const DSFactory& orig);
    virtual ~DSFactory();

    xmap<string, TensorDataset<double, double>*>* get_datasets_3cc();
    xmap<string, TensorDataset<double, double>*>* get_datasets_2cc();

   protected:
    Config* m_pConfig;
};

#endif  // DSFACTORY_H

#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ann/functions.h"
#include "dsaheader.h"

using namespace std;

class Config {
   public:
    Config(string cfg_filename = "config.txt");
    Config(const Config& orig);
    virtual ~Config();
    string get(string key, string def_value);
    string get_new_checkpoint(string model_name);

   protected:
    virtual void load_default();
    virtual void load_from(string filename);

   protected:
    string m_cfg_filename;
    xmap<string, string>* m_pMap;
};

#endif  // CONFIG_H

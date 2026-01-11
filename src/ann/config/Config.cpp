#include "config/Config.h"

#include <filesystem>

#include "sformat/fmt_lib.h"
namespace fs = std::filesystem;

Config::Config(string cfg_filename) : m_cfg_filename(cfg_filename) {
    m_pMap = new xmap<string, string>(&stringHash);
    load_default();           // Load default config values
    load_from(cfg_filename);  // Override with user config
}

Config::Config(const Config& orig) {}
Config::~Config() { delete m_pMap; }

// Default configuration values
void Config::load_default() {
    m_pMap->put("model_root", "./models");
    m_pMap->put("ckpt_name", "checkpoint");
    m_pMap->put("arch_file", "arch.txt");
    m_pMap->put("dataset_root", "datasets");
}

// Load configuration from file
void Config::load_from(string filename) {
    ifstream datastream(filename);
    if (!datastream.is_open()) {
        cerr << fmt::format("{:s}: cannot open for reading.", filename) << endl;
        return;
    }

    string line;
    while (getline(datastream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        char delimiter = ':';
        istringstream linestream(line);
        string key, value;
        getline(linestream, key, delimiter);
        getline(linestream, value, delimiter);

        key = trim(key);
        value = trim(value);
        if (key.empty() || value.empty())
            continue;

        m_pMap->put(key, value);
    }
    datastream.close();
}

// Get config value, fallback to default
string Config::get(string key, string def_value) {
    try {
        return m_pMap->get(key);
    } catch (KeyNotFound&) {
        return def_value;
    }
}

// Generate path for the next checkpoint folder
string Config::get_new_checkpoint(string model_name) {
    string model_root = get("model_root", "./models");
    string ckpt_name = get("ckpt_name", "checkpoint");
    string model_path = fs::path(model_root) / fs::path(model_name);

    if (!fs::exists(model_path))  // First checkpoint
        return fs::path(model_path) / fs::path(ckpt_name + "-1");

    // Find the largest checkpoint index
    int largest = 0;
    for (const auto& entry : fs::directory_iterator(model_path)) {
        if (entry.is_directory()) {
            string path = entry.path().string();
            string ckpt_idx_str = path.substr(path.rfind("-") + 1);
            int ckpt_idx;
            try {
                ckpt_idx = stoi(ckpt_idx_str);
            } catch (std::invalid_argument&) {
                ckpt_idx = -1;
            }
            if (largest < ckpt_idx)
                largest = ckpt_idx;
        }
    }

    int next_idx = largest + 1;
    string ckpt_folder = ckpt_name + "-" + to_string(next_idx);
    return fs::path(model_path) / fs::path(ckpt_folder);
}

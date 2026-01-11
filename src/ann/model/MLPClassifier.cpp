#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>
#include <sstream>

#include "ann/functions.h"
#include "layer/FCLayer.h"
#include "layer/ReLU.h"
#include "layer/Sigmoid.h"
#include "layer/Softmax.h"
#include "layer/Tanh.h"
#include "metrics/ClassMetrics.h"
#include "model/MLPClassifier.h"
#include "optim/IParamGroup.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"

// Constructors and Destructors
MLPClassifier::MLPClassifier(string cfg_filename, string sModelName)
    : IModel(cfg_filename, sModelName) {}

MLPClassifier::MLPClassifier(string cfg_filename, string sModelName, ILayer** seq, int size)
    : IModel(cfg_filename, sModelName) {
    for (int idx = 0; idx < size; idx++)
        m_layers.add(seq[idx]);
}

MLPClassifier::MLPClassifier(const MLPClassifier& orig)
    : IModel(orig.m_cfg_filename, orig.m_sModelName) {
    m_layers = orig.m_layers;
}

MLPClassifier::~MLPClassifier() {
    for (auto ptr_layer : m_layers)
        delete ptr_layer;
}

// Inference mode
double_tensor MLPClassifier::predict(double_tensor X, bool make_decision) {
    bool old_mode = this->m_trainable;
    this->set_working_mode(false);

    double_tensor Y = forward(X);

    this->set_working_mode(old_mode);

    if (make_decision)
        return Y;
    else
        return xt::argmax(Y, -1);
}

double_tensor MLPClassifier::predict(DataLoader<double, double>* pLoader, bool make_decision) {
    bool old_mode = this->m_trainable;
    this->set_working_mode(false);

    double_tensor results;
    bool first_batch = true;

    cout << "Prediction: Started" << endl;
    cout << fmt::format("{:<6s}/{:<12s}|{:<50s}\n", "Batch", "Total Batch",
                        "Num. of samples processed");

    int total_batch = pLoader->get_total_batch();
    int batch_idx = 1;
    unsigned long long nsamples = 0;

    for (auto batch : *pLoader) {
        double_tensor X = batch.getData();
        double_tensor Y = forward(X);

        if (first_batch) {
            results = Y;
            first_batch = false;
        } else
            results = xt::concatenate(xt::xtuple(results, Y), 0);

        nsamples += X.shape()[0];
        batch_idx++;

        cout << fmt::format("{:<6d}/{:<12d}|{:<50d}\n", batch_idx, total_batch, nsamples);
    }
    cout << "Prediction: End" << endl;

    this->set_working_mode(old_mode);

    if (make_decision)
        return results;
    else
        return xt::argmax(results, -1);
}

double_tensor MLPClassifier::evaluate(DataLoader<double, double>* pLoader) {
    bool old_mode = this->m_trainable;
    this->set_working_mode(false);

    ClassMetrics meter(this->get_num_classes());
    meter.reset_metrics();
    for (auto batch : *pLoader) {
        double_tensor X = batch.getData();
        double_tensor t = batch.getLabel();
        double_tensor Y = forward(X);
        ulong_tensor y_true = xt::argmax(t, 1);
        ulong_tensor y_pred = xt::argmax(Y, 1);
        meter.accumulate(y_true, y_pred);
    }
    double_tensor metrics = meter.get_metrics();
    this->set_working_mode(old_mode);
    return metrics;
}

// Training mode
void MLPClassifier::compile(IOptimizer* pOptimizer, ILossLayer* pLossLayer,
                            IMetrics* pMetricLayer) {
    this->m_pOptimizer = pOptimizer;
    this->m_pLossLayer = pLossLayer;
    this->m_pMetricLayer = pMetricLayer;

    for (auto pLayer : m_layers) {
        if (pLayer->has_learnable_param()) {
            string name = pLayer->getname();
            IParamGroup* pGroup = pOptimizer->create_group(name);
            pLayer->register_params(pGroup);
        }
    }
}

void MLPClassifier::set_working_mode(bool trainable) {
    m_trainable = trainable;
    for (auto pLayer : m_layers) {
        pLayer->set_working_mode(trainable);
    }
}

// Protected: training mode
double_tensor MLPClassifier::forward(double_tensor X) {
    double_tensor Y = X;
    for (auto& pLayer : m_layers)
        Y = pLayer->forward(Y);
    return Y;
}

void MLPClassifier::backward() {
    double_tensor dY = m_pLossLayer->backward();
    for (auto pLayer = m_layers.bbegin(); pLayer != m_layers.bend(); pLayer++)
        dY = (*pLayer)->backward(dY);
}

// Save and Load model
bool MLPClassifier::save(string model_path) {
    try {
        model_path = trim(model_path);
        if (fs::exists(model_path))
            fs::remove_all(model_path);
        else
            model_path = m_pConfig->get_new_checkpoint(this->m_sModelName);

        cout << model_path << ": creation" << endl;
        fs::create_directories(model_path);

        string arch_file = fs::path(model_path) / fs::path(m_pConfig->get("arch_file", "arch.txt"));
        ofstream datastream(arch_file);
        if (!datastream.is_open())
            throw "Cannot open architecture file for writing.";

        datastream << "model name: " << this->m_sModelName << endl;
        for (auto pLayer : m_layers) {
            datastream << pLayer->get_desc() << endl;
            pLayer->save(model_path);
        }
        datastream.close();
        return true;
    } catch (exception& e) {
        cerr << "MLPClassifier::save failed; model_path=" << model_path << endl;
        cerr << e.what() << endl;
        return false;
    }
}

bool MLPClassifier::load(string model_path, bool use_name_in_file) {
    try {
        if (!fs::exists(model_path)) {
            cerr << model_path << ": not exist." << endl;
            return false;
        }

        string arch_file = model_path + "/" + "arch.txt";
        ifstream datastream(arch_file);
        if (!datastream.is_open()) {
            cerr << arch_file << ": cannot open for reading." << endl;
            return false;
        }

        string line;
        while (getline(datastream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#')
                continue;

            char delimiter = ':';
            istringstream linestream(line);
            string first, second;
            getline(linestream, first, delimiter);
            getline(linestream, second, delimiter);

            delimiter = ',';
            istringstream partstream(first);
            string layer_type, layer_name;
            getline(partstream, layer_type, delimiter);
            getline(partstream, layer_name, delimiter);
            layer_type = trim(layer_type);
            layer_name = trim(layer_name);

            string new_name = use_name_in_file ? layer_name : "";

            if (layer_type == "FC") {
                string w_file = model_path + "/" + layer_name + "_W.npy";
                string b_file = model_path + "/" + layer_name + "_b.npy";
                m_layers.add(new FCLayer(trim(second), w_file, b_file, new_name));
            } else if (layer_type == "ReLU")
                m_layers.add(new ReLU(new_name));
            else if (layer_type == "Sigmoid")
                m_layers.add(new Sigmoid(new_name));
            else if (layer_type == "Tanh")
                m_layers.add(new Tanh(new_name));
            else if (layer_type == "Softmax") {
                int nAxis;
                try {
                    nAxis = stoi(trim(second));
                } catch (std::invalid_argument&) {
                    cerr << "Cannot read axis of Softmax from: " << trim(second) << endl;
                    cout << "Use 'axis=-1' instead." << endl;
                    nAxis = -1;
                }
                m_layers.add(new Softmax(nAxis, new_name));
            }
        }
        datastream.close();
        return true;
    } catch (exception& e) {
        cerr << "In MLPClassifier::load:" << endl;
        cout << e.what() << endl;
        return false;
    }
}

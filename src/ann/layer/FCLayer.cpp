#include "layer/FCLayer.h"

#include <exception>
#include <filesystem>
namespace fs = std::filesystem;
#include <sstream>

#include "ann/functions.h"
#include "sformat/fmt_lib.h"

using namespace std;

// Constructor: initialize randomly
FCLayer::FCLayer(int Nin, int Nout, bool use_bias) {
    m_nNin = Nin;
    m_nNout = Nout;
    m_bUse_Bias = use_bias;
    m_sName = "FC_" + to_string(++m_unLayer_idx);
    m_unSample_Counter = 0;
    init_weights();
}

// Constructor: initialize from parameters or files
FCLayer::FCLayer(string sParams, string filename_w, string filename_b, string sName) {
    m_sName = trim(sName).empty() ? "FC_" + to_string(++m_unLayer_idx) : sName;

    try {
        // Parse sParams: Nin, Nout, use_bias
        char delimiter = ',';
        istringstream param_stream(sParams);
        int nparams[] = {0, 0, 0};
        string param_slist[] = {"", "", ""};
        int idx = 0;
        while (getline(param_stream, param_slist[idx], delimiter)) {
            nparams[idx] = stoi(param_slist[idx]);
            idx++;
            if (idx >= 3)
                break;
        }
        if (idx == 2) {
            cout << "use-bias: not specified; to use-bias=true" << endl;
            nparams[2] = 1;
        } else if (idx < 2) {
            throw runtime_error("FC's parameters: must specify at least Nin and Nout");
        }

        m_nNin = nparams[0];
        m_nNout = nparams[1];
        m_bUse_Bias = nparams[2];
        m_unSample_Counter = 0;

        bool weight_file_invalid = !fs::exists(filename_w);
        bool bias_file_invalid = m_bUse_Bias && !fs::exists(filename_b);

        if (weight_file_invalid) {
            cout << fmt::format("{:s}: not exist; initializing weights randomly", filename_w)
                 << endl;
            m_aWeights = xt::random::randn<double>({m_nNout, m_nNin});
            m_aGrad_W = xt::zeros<double>({m_nNout, m_nNin});
        } else {
            double_tensor W = xt::load_npy<double>(filename_w);
            if ((W.dimension() != 2) || (W.shape()[0] != m_nNout) || (W.shape()[1] != m_nNin))
                throw runtime_error("FC::Weights shape mismatch");
            m_aWeights = W;
            m_aGrad_W = xt::zeros<double>({m_nNout, m_nNin});
        }

        if (bias_file_invalid) {
            cout << fmt::format("{:s}: not exist; initializing biases with 0", filename_b) << endl;
            m_aBias = xt::zeros<double>({m_nNout});
            m_aGrad_b = xt::zeros<double>({m_nNout});
        } else if (m_bUse_Bias) {
            double_tensor b = xt::load_npy<double>(filename_b);
            if ((b.dimension() != 1) || (b.shape()[0] != m_nNout))
                throw runtime_error("FC::Bias shape mismatch");
            m_aBias = b;
            m_aGrad_b = xt::zeros<double>({m_nNout});
        }

    } catch (exception& e) {
        cerr << "Exception in FCLayer constructor:\n"
             << "\tsParams: " << sParams << "\n"
             << "\tfilename_w: " << filename_w << "\n"
             << "\tfilename_b: " << filename_b << "\n"
             << "\tsName: " << sName << endl;
        cerr << "-------EXCEPTION: BEGIN-------\n\t" << e.what() << "\n-------EXCEPTION: END-------"
             << endl;
        throw;
    }
}

// Initialize weights and biases
void FCLayer::init_weights() {
    m_aWeights = xt::random::randn<double>({m_nNout, m_nNin});
    m_aGrad_W = xt::zeros<double>({m_nNout, m_nNin});

    if (m_bUse_Bias) {
        m_aBias = xt::zeros<double>({m_nNout});
        m_aGrad_b = xt::zeros<double>({m_nNout});
    }
}

FCLayer::FCLayer(const FCLayer& orig) { m_sName = "FC_" + to_string(++m_unLayer_idx); }
FCLayer::~FCLayer() {}

// Forward pass
xt::xarray<double> FCLayer::forward(xt::xarray<double> X) {
    if (m_trainable)
        m_aCached_X = X;
    xt::xarray<double> Y = xt::linalg::dot(X, xt::transpose(m_aWeights));
    if (m_bUse_Bias)
        Y += m_aBias;
    return Y;
}

// Backward pass
xt::xarray<double> FCLayer::backward(xt::xarray<double> DY) {
    m_unSample_Counter += DY.shape()[0];
    if (m_bUse_Bias)
        m_aGrad_b = xt::sum(DY, {0});

    xt::xarray<double> DW =
        xt::zeros<double>({DY.shape()[0], m_aWeights.shape()[0], m_aWeights.shape()[1]});
    for (size_t i = 0; i < DY.shape()[0]; i++) {
        auto DY_row = xt::view(DY, i);
        auto X_row = xt::view(m_aCached_X, i);
        xt::view(DW, i) = xt::linalg::outer(DY_row, xt::transpose(X_row));
    }
    m_aGrad_W = xt::sum(DW, {0});

    xt::xarray<double> DX = xt::linalg::dot(DY, m_aWeights);
    return DX;
}

// Register parameters to optimizer
int FCLayer::register_params(IParamGroup* ptr_group) {
    ptr_group->register_param("weights", &m_aWeights, &m_aGrad_W);
    int count = 1;
    if (m_bUse_Bias) {
        ptr_group->register_param("bias", &m_aBias, &m_aGrad_b);
        count += 1;
    }
    ptr_group->register_sample_count(&m_unSample_Counter);
    return count;
}

// Layer description
string FCLayer::get_desc() {
    return fmt::format("{:<10s}, {:<15s}: {:<4d}, {:<4d}, {:<4d}", "FC", getname(), m_nNin, m_nNout,
                       m_bUse_Bias);
}

// Save weights and biases
void FCLayer::save(string model_path) {
    string filename_w = model_path + "/" + getname() + "_W.npy";
    string filename_b = model_path + "/" + getname() + "_b.npy";

    xt::dump_npy(filename_w, m_aWeights);
    if (m_bUse_Bias)
        xt::dump_npy(filename_b, m_aBias);
}

// Load weights and biases from file
void FCLayer::load(string model_path, string layer_name) {
    layer_name = trim(layer_name);
    string filename_w = model_path + "/" + (layer_name.empty() ? getname() : layer_name) + "_W.npy";
    string filename_b = model_path + "/" + (layer_name.empty() ? getname() : layer_name) + "_b.npy";

    try {
        if (fs::exists(filename_w)) {
            m_aWeights = xt::load_npy<double>(filename_w);
            m_nNin = m_aWeights.shape()[1];
            m_nNout = m_aWeights.shape()[0];
            m_aGrad_W = xt::zeros<double>({m_nNout, m_nNin});
        } else {
            throw runtime_error(fmt::format("{:s}: weight-file does not exist.", filename_w));
        }

        if (fs::exists(filename_b)) {
            m_aBias = xt::load_npy<double>(filename_b);
            if (m_aBias.shape()[0] != m_nNout)
                throw runtime_error("Bias size mismatch with Nout");
            m_aGrad_b = xt::zeros<double>({m_nNout});
            m_bUse_Bias = true;
        } else {
            m_bUse_Bias = false;
        }

        m_unSample_Counter = 0;
    } catch (exception& e) {
        cout << e.what() << endl;
        throw;
    }
}

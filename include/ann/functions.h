#ifndef FUNTIONS_H
#define FUNTIONS_H

#include <algorithm>
#include <cctype>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "tensor/xtensor_lib.h"

using namespace std;

enum class_metrics {
    ACCURACY = 0,
    PRECISION_MACRO,
    PRECISION_WEIGHTED,
    RECALL_MACRO,
    RECALL_WEIGHTED,
    F1_MEASURE_MACRO,
    F1_MEASURE_WEIGHTED,
    NUM_CLASS_METRICS
};

xt::xarray<double> softmax(xt::xarray<double> X, int axis = -1);
double cross_entropy(xt::xarray<double> Ypred, xt::xarray<double> Ygt, bool mean_reduced = true);
double cross_entropy(xt::xarray<double> Ypred, xt::xarray<unsigned long> ygt,
                     bool mean_reduced = true);
xt::xarray<double> onehot_enc(xt::xarray<unsigned long> x, int nclasses);
xt::xarray<ulong> confusion_matrix(xt::xarray<ulong> y_true, xt::xarray<ulong> y_pred,
                                   int nclasses);
xt::xarray<ulong> class_count(xt::xarray<ulong> confusion);
double_tensor calc_classifcation_metrics(ulong_tensor y_true, ulong_tensor y_pred, int nclasses);

int stringHash(string& str, int size);

string ltrim(std::string& s);
string rtrim(std::string& s);
string trim(std::string& s);

string to_lower(const string& str);

void estimate_params(xt::xarray<double> X, xt::xarray<double>& mu, xt::xarray<double>& sigma);
xt::xarray<double> normalize(xt::xarray<double> X, xt::xarray<double> mu, xt::xarray<double> sigma);

#endif  // FUNTIONS_H

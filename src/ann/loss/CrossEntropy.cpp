#include "loss/CrossEntropy.h"

#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction) : ILossLayer(reduction) {}

CrossEntropy::CrossEntropy(const CrossEntropy &orig) : ILossLayer(orig) {}

CrossEntropy::~CrossEntropy() {}

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {
    m_aCached_Ypred = X;
    m_aYtarget = t;
    return cross_entropy(X, t, (m_eReduction == REDUCE_MEAN));
}

xt::xarray<double> CrossEntropy::backward() {
    int nsamples = m_aCached_Ypred.shape()[0];
    xt::xarray<double> grad = -(m_aYtarget / (m_aCached_Ypred + 1e-7));
    if (m_eReduction == REDUCE_MEAN)
        grad /= nsamples;
    return grad;
}

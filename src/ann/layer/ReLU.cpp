#include "layer/ReLU.h"

#include "ann/functions.h"
#include "sformat/fmt_lib.h"

ReLU::ReLU(string name) {
    if (trim(name).size() != 0)
        m_sName = name;
    else
        m_sName = "ReLU_" + to_string(++m_unLayer_idx);
}

ReLU::ReLU(const ReLU &orig) { m_sName = "ReLU_" + to_string(++m_unLayer_idx); }

ReLU::~ReLU() {}

xt::xarray<double> ReLU::forward(xt::xarray<double> X) {
    m_aMask = (X >= 0);
    xt::xarray<double> Y = m_aMask * X;
    return Y + 0.0;
}
xt::xarray<double> ReLU::backward(xt::xarray<double> DY) {
    xt::xarray<double> DX = m_aMask * DY;
    return DX;
}

string ReLU::get_desc() {
    string desc = fmt::format("{:<10s}, {:<15s}:", "ReLU", this->getname());
    return desc;
}

#include "layer/Sigmoid.h"

#include "ann/functions.h"
#include "sformat/fmt_lib.h"

Sigmoid::Sigmoid(string name) {
    if (trim(name).size() != 0)
        m_sName = name;
    else
        m_sName = "Sigmoid_" + to_string(++m_unLayer_idx);
}

Sigmoid::Sigmoid(const Sigmoid &orig) { m_sName = "Sigmoid_" + to_string(++m_unLayer_idx); }

Sigmoid::~Sigmoid() {}

xt::xarray<double> Sigmoid::forward(xt::xarray<double> X) {
    m_aCached_Y = 1.0 / (1.0 + xt::exp(-X));
    return m_aCached_Y;
}

xt::xarray<double> Sigmoid::backward(xt::xarray<double> DY) {
    xt::xarray<double> DX = DY * m_aCached_Y * (1.0 - m_aCached_Y);
    return DX;
}

string Sigmoid::get_desc() {
    string desc = fmt::format("{:<10s}, {:<15s}:", "Sigmoid", this->getname());
    return desc;
}

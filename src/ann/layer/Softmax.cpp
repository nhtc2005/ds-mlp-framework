#include "layer/Softmax.h"

#include <filesystem>
namespace fs = std::filesystem;

#include "ann/functions.h"
#include "sformat/fmt_lib.h"

Softmax::Softmax(int axis, string name) : m_nAxis(axis) {
    if (trim(name).size() != 0)
        m_sName = name;
    else
        m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax &orig) {}

Softmax::~Softmax() {}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
    xt::svector<unsigned long> shape = X.shape();
    m_nAxis = positive_index(m_nAxis, shape.size());
    shape[m_nAxis] = 1;
    xt::xarray<double> Xmax = xt::amax(X, m_nAxis);
    X = xt::exp(X - Xmax.reshape(shape));
    xt::xarray<double> SX = xt::sum(X, -1);
    SX = SX.reshape(shape);
    m_aCached_Y = X / SX;
    return m_aCached_Y;
}

xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
    if (DY.dimension() == 1) {
        auto y = m_aCached_Y;
        auto jacobian = xt::diag(y) - xt::linalg::outer(y, y);
        return xt::linalg::dot(jacobian, DY);
    }

    xt::xarray<double> DX = xt::zeros_like(DY);

    for (size_t i = 0; i < DY.shape()[0]; i++) {
        auto dy = xt::view(DY, i, xt::all());
        auto y = xt::view(m_aCached_Y, i, xt::all());

        double s = xt::sum(dy * y)();
        xt::view(DX, i, xt::all()) = y * (dy - s);
    }
    return DX;
}

string Softmax::get_desc() {
    string desc = fmt::format("{:<10s}, {:<15s}: {:4d}", "Softmax", this->getname(), m_nAxis);
    return desc;
}

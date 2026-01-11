#ifndef XTENSOR_LIB_H
#define XTENSOR_LIB_H

#include <string>
#include <sstream>
#include <ctime>

#include "tensor/xtensor/xio.hpp"
#include "tensor/xtensor/xview.hpp"
#include "tensor/xtensor/xslice.hpp"
#include "tensor/xtensor/xbuilder.hpp"
#include "tensor/xtensor-blas/xlinalg.hpp"
#include "tensor/xtensor/xrandom.hpp"
#include "tensor/xtensor/xindex_view.hpp"
#include "tensor/xtensor/xsort.hpp"
#include "tensor/xtensor/xarray.hpp"
#include "tensor/xtensor/xnpy.hpp"

using namespace std;

typedef unsigned long ulong;
typedef xt::xarray<ulong> ulong_tensor;
typedef xt::xarray<double> double_tensor;

string shape2str(xt::svector<unsigned long> vec);
int positive_index(int idx, int size);
xt::xarray<double> outer_stack(xt::xarray<double> X, xt::xarray<double>  Y);
xt::xarray<double> diag_stack(xt::xarray<double> X);
xt::xarray<double> matmul_on_stack(xt::xarray<double> X, xt::xarray<double>  Y);

#endif  // XTENSOR_LIB_H

#ifndef DATASET_H
#define DATASET_H

#include "tensor/xtensor_lib.h"

using namespace std;

template <typename DType, typename LType>
class DataLabel {
   private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

   public:
    DataLabel(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label) {}
    xt::xarray<DType> getData() const { return data; }
    xt::xarray<LType> getLabel() const { return label; }
};

template <typename DType, typename LType>
class Batch {
   private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

   public:
    Batch(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label) {}
    virtual ~Batch() {}
    xt::xarray<DType>& getData() { return data; }
    xt::xarray<LType>& getLabel() { return label; }
};

template <typename DType, typename LType>
class Dataset {
   public:
    Dataset(){};
    virtual ~Dataset(){};
    virtual int len() = 0;
    virtual DataLabel<DType, LType> getitem(int index) = 0;
    virtual xt::svector<unsigned long> get_data_shape() = 0;
    virtual xt::svector<unsigned long> get_label_shape() = 0;
};

template <typename DType, typename LType>
class TensorDataset : public Dataset<DType, LType> {
   private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;

   public:
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label) {
        data_shape = data.shape();
        label_shape = label.shape();
    }

    int len() { return data.shape()[0]; }

    DataLabel<DType, LType> getitem(int index) {
        if ((index < 0) || (index >= data.shape()[0]))
            throw std::out_of_range("Index is out of range!");

        auto data_item = xt::view(data, index);

        if (label_shape.size() == 0)  // In case label contains scalar data or empty
            return DataLabel<DType, LType>(data_item, label(0));
        else if (label.shape()[0] == data.shape()[0])  // In case label is the same size as data
        {
            auto label_item = xt::view(label, index, xt::all());
            return DataLabel<DType, LType>(data_item, label_item);
        } else
            throw std::runtime_error("Data and label dimensions do not match!");
    }

    xt::svector<unsigned long> get_data_shape() { return data_shape; }
    xt::svector<unsigned long> get_label_shape() { return label_shape; }
};

#endif  // DATASET_H

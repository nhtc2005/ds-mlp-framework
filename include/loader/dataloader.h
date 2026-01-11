#ifndef DATALOADER_H
#define DATALOADER_H

#include "list/listheader.h"
#include "loader/dataset.h"
#include "tensor/xtensor_lib.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader {
   private:
    Dataset<DType, LType> *ptr_dataset;
    int batch_size;
    bool shuffle;    // True => Data will be shuffled
    bool drop_last;  // True => Ignore the last remaining batch
    xt::xarray<int> indices;
    int dataset_len;
    int index;
    int m_seed;

   public:
    DataLoader(Dataset<DType, LType> *ptr_dataset, int batch_size, bool shuffle = true,
               bool drop_last = false, int seed = -1)
        : ptr_dataset(ptr_dataset), shuffle(shuffle), drop_last(drop_last), index(0), m_seed(seed) {
        dataset_len = ptr_dataset->len();
        this->batch_size = batch_size;
        indices = xt::arange(0, dataset_len);
        if (shuffle) {
            if (m_seed >= 0)
                xt::random::seed(m_seed);
            xt::random::shuffle(indices);
        }
    }

    virtual ~DataLoader() {}
    int get_batch_size() { return batch_size; }
    int get_sample_count() { return ptr_dataset->len(); }
    int get_total_batch() { return int(ptr_dataset->len() / batch_size); }

    // Iterator
    class Iterator {
       private:
        DataLoader *data_loader;
        int index;
        int stop_index;

       public:
        Iterator(DataLoader *data_loader, int index) : data_loader(data_loader), index(index) {
            if (this->index >= data_loader->dataset_len)
                this->stop_index = data_loader->dataset_len;
            else {
                int temp = data_loader->dataset_len - (this->index + data_loader->batch_size);
                if (temp < 0) {
                    this->index = data_loader->dataset_len;
                    this->stop_index = data_loader->dataset_len;
                } else if ((temp < data_loader->batch_size) && (data_loader->drop_last == false))
                    this->stop_index = this->index + data_loader->batch_size + temp;
                else
                    this->stop_index = this->index + data_loader->batch_size;
            }
        }

        Iterator &operator=(const Iterator &iterator) {
            if (this != &iterator) {
                this->data_loader = iterator.data_loader;
                this->index = iterator.index;
            }
            return *this;
        }

        bool operator!=(const Iterator &iterator) const { return this->index != iterator.index; }

        // Get current batch
        Batch<DType, LType> operator*() const {
            size_t current_batch_size = stop_index - index;
            auto data_shape = data_loader->ptr_dataset->get_data_shape();
            auto label_shape = data_loader->ptr_dataset->get_label_shape();

            size_t data_dim = data_shape.size();
            size_t label_dim = label_shape.size();

            xt::xarray<DType> data;
            xt::xarray<LType> label;

            // If data is one dimensional array
            if (data_dim == 1) {
                data = xt::xarray<DType>::from_shape({current_batch_size});

                for (size_t i = index; i < stop_index; i++) {
                    auto item = data_loader->ptr_dataset->getitem(data_loader->indices[i]);
                    xt::view(data, i - index) = item.getData();
                }
            }
            // If data is multi-dimensional array
            else {
                xt::svector<size_t> data_shape_batch = {current_batch_size};
                data_shape_batch.insert(data_shape_batch.end(), data_shape.begin() + 1,
                                        data_shape.end());

                data = xt::xarray<DType>::from_shape(data_shape_batch);

                for (size_t i = index; i < stop_index; i++) {
                    auto item = data_loader->ptr_dataset->getitem(data_loader->indices[i]);
                    xt::view(data, i - index) = item.getData();
                }
            }

            // If label is empty or contains scalar data
            if (label_dim == 0)
                ;
            // If label is one dimensional array
            else if (label_dim == 1) {
                label = xt::xarray<LType>::from_shape({current_batch_size});

                for (size_t i = index; i < stop_index; i++) {
                    auto item = data_loader->ptr_dataset->getitem(data_loader->indices[i]);
                    xt::view(label, i - index) = item.getLabel();
                }
            }
            // If label is multi-dimensional array
            else {
                xt::svector<size_t> label_shape_batch = {current_batch_size};
                label_shape_batch.insert(label_shape_batch.end(), label_shape.begin() + 1,
                                         label_shape.end());

                label = xt::xarray<LType>::from_shape(label_shape_batch);

                for (size_t i = index; i < stop_index; i++) {
                    auto item = data_loader->ptr_dataset->getitem(data_loader->indices[i]);
                    xt::view(label, i - index) = item.getLabel();
                }
            }

            return Batch<DType, LType>(data, label);
        }

        // Prefix ++ overload
        Iterator &operator++() {
            index = stop_index;
            if (index + data_loader->batch_size > data_loader->dataset_len)
                index = data_loader->dataset_len;
            if (index >= data_loader->dataset_len)
                stop_index = data_loader->dataset_len;
            else {
                int temp = data_loader->dataset_len - (index + data_loader->batch_size);
                if ((temp < data_loader->batch_size) && (data_loader->drop_last == false))
                    stop_index = index + data_loader->batch_size + temp;
                else
                    stop_index = index + data_loader->batch_size;
            }
            return *this;
        }

        // Postfix ++ overload
        Iterator operator++(int) {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, dataset_len); }
};

#endif  // DATALOADER_H

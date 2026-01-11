#ifndef IMODEL_H
#define IMODEL_H

#include "config/Config.h"
#include "loader/dataloader.h"
#include "loss/ILossLayer.h"
#include "metrics/IMetrics.h"
#include "optim/IOptimizer.h"
#include "tensor/xtensor_lib.h"

class IModel {
   public:
    IModel(string cfg_filename, string sModelName);
    virtual ~IModel();

    // Inference
    virtual double_tensor predict(double_tensor X, bool make_decision = false) = 0;
    virtual double_tensor predict(DataLoader<double, double>* pLoader,
                                  bool make_decision = false) = 0;
    virtual double_tensor evaluate(DataLoader<double, double>* pLoader) = 0;

    // Training
    virtual void compile(IOptimizer* pOptimizer, ILossLayer* pLossLayer,
                         IMetrics* pMetricLayer) = 0;

    // Train model
    virtual void fit(DataLoader<double, double>* pTrainLoader,
                     DataLoader<double, double>* pValidLoader, unsigned int nepoch = 10,
                     unsigned int verbose = 1);  // Implemented in IModel

    virtual void set_working_mode(bool trainable) = 0;

    // Save/Load
    virtual bool save(string model_path = "") = 0;
    virtual bool load(string model_path, bool use_name_in_file = false) = 0;

   protected:
    virtual double_tensor forward(double_tensor X) = 0;
    virtual void backward() = 0;

   protected:
    bool m_trainable;  // Training mode
    string m_sModelName;
    string m_cfg_filename;
    Config* m_pConfig;

    // Variables for fit()
    void on_begin_training(DataLoader<double, double>* pTrainLoader,
                           DataLoader<double, double>* pValidLoader, unsigned int nepoch = 10,
                           int verbose = 1);
    void on_end_training();
    void on_begin_epoch();
    void on_end_epoch();
    void on_begin_step(int batch_size);
    void on_end_step(double batch_loss);

    IOptimizer* m_pOptimizer;
    ILossLayer* m_pLossLayer;
    IMetrics* m_pMetricLayer;

    DataLoader<double, double>* m_pTrainLoader;
    DataLoader<double, double>* m_pValidLoader;
    int m_nepoches;
    int m_current_epoch;
    int m_current_batch;
    int m_verbose;
    double m_epoch_loss;
    int m_curent_batch_size;
    int m_sample_counter;
};

#endif  // IMODEL_H

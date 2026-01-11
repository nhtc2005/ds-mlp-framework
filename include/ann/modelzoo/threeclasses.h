#ifndef THREECLASSES_H
#define THREECLASSES_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "ann/annheader.h"
#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "loader/dataloader.h"
#include "loader/dataset.h"
#include "optim/Adagrad.h"
#include "optim/Adam.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"

using namespace std;

void threeclasses_classification() {
    DSFactory factory("./config.txt");
    xmap<string, TensorDataset<double, double>*>* pMap = factory.get_datasets_3cc();
    TensorDataset<double, double>* train_ds = pMap->get("train_ds");
    TensorDataset<double, double>* valid_ds = pMap->get("valid_ds");
    TensorDataset<double, double>* test_ds = pMap->get("test_ds");
    DataLoader<double, double> train_loader(train_ds, 50, true, false);
    DataLoader<double, double> valid_loader(valid_ds, 50, false, false);
    DataLoader<double, double> test_loader(test_ds, 50, false, false);

    int nClasses = 3;
    ILayer* layers[] = {new FCLayer(2, 50, true),        new ReLU(),
                        new FCLayer(50, 20, true),       new ReLU(),
                        new FCLayer(20, nClasses, true), new Softmax()};
    MLPClassifier model("./config.txt", "3c-classification", layers,
                        sizeof(layers) / sizeof(ILayer*));

    Adam optim(1e-3, 0.9, 0.99);
    CrossEntropy loss;
    ClassMetrics metrics(nClasses);

    // Train + eval
    model.compile(&optim, &loss, &metrics);
    model.fit(&train_loader, &valid_loader, 1000);
    string base_path = "./models";
    model.save(base_path + "/" + "3c-classification-1");
    double_tensor eval_rs = model.evaluate(&test_loader);
    cout << "Evaluation result on the testing dataset: " << endl;
    cout << eval_rs << endl;

    // Load + eval
    MLPClassifier pretrained1("./config.txt");
    pretrained1.load(base_path + "/" + "3c-classification-1", true);
    double_tensor eval_rs1 = pretrained1.evaluate(&test_loader);
    cout << "Load + Eval a pretrained model : " << endl;
    cout << eval_rs1 << endl;
}

#endif  // THREECLASSES_H

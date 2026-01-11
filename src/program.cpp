#include <filesystem>
namespace fs = std::filesystem;
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "ann/annheader.h"
#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "list/listheader.h"
#include "loader/dataloader.h"
#include "loader/dataset.h"
#include "modelzoo/threeclasses.h"
#include "modelzoo/twoclasses.h"
#include "optim/Adagrad.h"
#include "optim/Adam.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"

using namespace std;

int main(int argc, char **argv) {
    twoclasses_classification();
    // threeclasses_classification();

    return 0;
}

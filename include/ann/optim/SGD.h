#ifndef SGD_H
#define SGD_H

#include "optim/IOptimizer.h"

class SGD : public IOptimizer {
   public:
    SGD(double lr = 1e-4);
    SGD(const SGD& orig);
    virtual ~SGD();

    IParamGroup* create_group(string name);
};

#endif  // SGD_H

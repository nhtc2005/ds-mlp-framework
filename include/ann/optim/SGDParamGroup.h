#ifndef SGDPARAMGROUP_H
#define SGDPARAMGROUP_H

#include "optim/IParamGroup.h"

class SGDParamGroup : public IParamGroup {
   public:
    SGDParamGroup();
    SGDParamGroup(const SGDParamGroup& orig);
    virtual ~SGDParamGroup();

    void register_param(string param_name, xt::xarray<double>* ptr_param,
                        xt::xarray<double>* ptr_grad);
    void register_sample_count(unsigned long long* pCounter);
    void zero_grad();
    void step(double lr);

   protected:
    xmap<string, xt::xarray<double>*>* m_pParams;
    xmap<string, xt::xarray<double>*>* m_pGrads;
    unsigned long long* m_pCounter;
};

#endif  // SGDPARAMGROUP_H

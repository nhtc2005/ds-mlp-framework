#include "optim/SGD.h"

#include <string>

#include "list/DLinkedList.h"
#include "optim/SGDParamGroup.h"

using namespace std;

SGD::SGD(double lr) : IOptimizer(lr) {}

SGD::SGD(const SGD &orig) {}

SGD::~SGD() {}

IParamGroup *SGD::create_group(string name) {
    IParamGroup *pGroup = new SGDParamGroup();
    m_pGroupMap->put(name, pGroup);
    return pGroup;
}

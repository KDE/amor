
#ifndef __AmorIface_h__
#define __AmorIface_h__

#include <dcopobject.h>

class AmorIface : virtual public DCOPObject
{
  K_DCOP
public:

k_dcop:
    virtual void showTip(QString tip) = 0;
};

#endif


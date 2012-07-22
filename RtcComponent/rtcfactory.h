#ifndef RTCFACTORY_H
#define RTCFACTORY_H

#include "RtcComponent_global.h"

#include <component.h>

class RTCCOMPONENTSHARED_EXPORT RtcFactory : public ComponentFactory
{
public:
    Component create();
};

PUBLISH_PLUGIN(RtcFactory)

#endif // RTCFACTORY_H

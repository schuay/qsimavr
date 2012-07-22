#include "rtcfactory.h"

#include "rtclogic.h"

Component RtcFactory::create()
{
    QSharedPointer<RtcLogic> logic(new RtcLogic());
    Component component = { QSharedPointer<ComponentGui>(), logic };
    return component;
}

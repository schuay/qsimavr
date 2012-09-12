#include "rtcfactory.h"

#include "rtcgui.h"
#include "rtclogic.h"

Component RtcFactory::create()
{
    QSharedPointer<RtcLogic> logic(new RtcLogic());
    QSharedPointer<RtcGui> gui = QSharedPointer<RtcGui>(new RtcGui());
    Component component = { gui , logic };

    QObject::connect(logic.data(), SIGNAL(dataChanged(QByteArray)),
                     gui.data(), SLOT(onDataChange(QByteArray)), Qt::QueuedConnection);
    QObject::connect(gui.data(), SIGNAL(dataChanged(QByteArray)),
                     logic.data(), SLOT(onDataChange(QByteArray)), Qt::QueuedConnection);
    return component;
}

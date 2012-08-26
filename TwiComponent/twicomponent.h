#ifndef TWICOMPONENT_H
#define TWICOMPONENT_H

#include "TwiComponent_global.h"

#include <component.h>

class TWICOMPONENTSHARED_EXPORT TwiSlave
{
public:
    virtual uint8_t transmitByte() = 0;
    virtual bool matchesAddress(uint8_t address) = 0;
    virtual void received(const QByteArray &data) = 0;
};

class TWICOMPONENTSHARED_EXPORT TwiComponent : public ComponentLogic
{
public:
    TwiComponent(TwiSlave *slave);

    void wireHook(avr_t *avr);
    void unwireHook();

private:
    void reset();

    void receiveHook(struct avr_irq_t *irq, uint32_t value);
    static void receiveHook(struct avr_irq_t *irq, uint32_t value, void *param);

    avr_t *avr;
    avr_irq_t *irq;

    TwiSlave *slave;

    /** Buffers incoming data until message is completed. */
    QByteArray data;
    uint8_t state;

    /* From i2c_eeprom.h. */
    uint8_t selected;
};

#endif // TWICOMPONENT_H

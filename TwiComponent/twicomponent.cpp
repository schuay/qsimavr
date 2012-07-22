#include "twicomponent.h"

#include <avr_twi.h>

#define IRQ_COUNT (2)

TwiComponent::TwiComponent(TwiSlave *slave) : slave(slave)
{
}

void TwiComponent::connect(avr_t *avr)
{
    this->avr = avr;

    const char *names[] = { "twi.miso", "twi.mosi" };

    irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_COUNT, names);
    avr_irq_register_notify(irq + TWI_IRQ_MOSI, TwiComponent::receiveHook, this);

    avr_connect_irq(irq + TWI_IRQ_MISO, avr_io_getirq(avr, AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_MISO));
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_MOSI), irq + TWI_IRQ_MOSI);
}

void TwiComponent::disconnect()
{
    avr_unconnect_irq(irq + TWI_IRQ_MISO, avr_io_getirq(avr, AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_MISO));
    avr_unconnect_irq(avr_io_getirq(avr, AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_MOSI), irq + TWI_IRQ_MOSI);

    avr_free_irq(irq, IRQ_COUNT);
}

void TwiComponent::receiveHook(avr_irq_t *irq, uint32_t value, void *param)
{
    TwiComponent *twi = (TwiComponent *)param;
    twi->receiveHook(irq, value);
}

void TwiComponent::reset()
{
    if (state == TWI_COND_WRITE) {
        slave->received(data);
    }

    selected = 0;
    state = 0;
    data.clear();
}

void TwiComponent::receiveHook(avr_irq_t *, uint32_t value)
{
    avr_twi_msg_irq_t v;
    v.u.v = value;

    /*
     * If we receive a STOP, check it was meant to us, and reset the transaction.
     */
    if (v.u.twi.msg & TWI_COND_STOP) {
        reset();
    }

    /*
     * If we receive a start, reset status, check if the slave address is
     * meant to be us, and if so reply with an ACK bit.
     */
    if (v.u.twi.msg & TWI_COND_START) {
        reset();
        if (slave->matchesAddress(v.u.twi.addr)) {
            selected = v.u.twi.addr;
            avr_raise_irq(irq + TWI_IRQ_MISO,
                          avr_twi_irq_msg(TWI_COND_ACK, selected, 1));
        }
    }

    /*
     * If it's a data transaction, first check it is meant to be us (we
     * received the correct address and are selected)
     */
    if (!selected) {
        return;
    }

    state = v.u.twi.msg & (TWI_COND_WRITE | TWI_COND_READ);

    /*
     * This is a write transaction.
     */
    if (v.u.twi.msg & TWI_COND_WRITE) {
        avr_raise_irq(irq + TWI_IRQ_MISO,
                avr_twi_irq_msg(TWI_COND_ACK, selected, 1));

        data.append(v.u.twi.data);
    }

    /*
     * It's a read transaction, just send the next byte back to the master
     */
    else if (v.u.twi.msg & TWI_COND_READ) {
        avr_raise_irq(irq + TWI_IRQ_MISO,
                      avr_twi_irq_msg(TWI_COND_READ, selected, slave->transmitByte()));
    }
}

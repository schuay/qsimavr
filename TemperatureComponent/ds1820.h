/* *************************************************************************
 *  Copyright 2012 Jakob Gruber                                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */



#ifndef DS1820_H
#define DS1820_H

#include <QObject>
#include <sim_cycle_timers.h>

class DS1820 : public QObject
{
    Q_OBJECT
public:
    explicit DS1820(QObject *parent = 0);

    void wire(avr_t *avr);

    uint8_t pinLevel() const { return level; }
    void pinChanged(uint8_t level);

    void setScratchpad(QByteArray data);
    void setEeprom(QByteArray data);
    
signals:
    void setPin();
    void scratchpadChanged(QByteArray data);
    void eepromChanged(QByteArray data);

private:
    static avr_cycle_count_t timerHook(avr_t *avr, avr_cycle_count_t cycles, void *param);
    void timer();
    void wait(uint32_t usec);

    void write(uint8_t bit);
    uint8_t read(uint32_t duration) const;
    void romCommand();
    void functionCommand();
    void updateCRC();
    void setScratchpad(uint8_t index, uint8_t data);
    void setEeprom(uint8_t index, uint8_t data);
    
private:
    typedef enum {
        IDLE,
        RESET_WAIT,
        PRESENCE_PULSE,
        ROM_COMMAND,
        MATCH_ROM,
        SEARCH_ROM,
        FUNCTION_COMMAND,
        READ_SCRATCHPAD,
        WRITE_SCRATCHPAD,
        RECALL_E,
        CONVERT,
    } state_t;

    avr_t *avr;
    avr_cycle_count_t lastChange;

    uint8_t level;
    state_t state;

    uint8_t in;
    uint8_t incount;

    uint8_t out;
    uint8_t outcount;

    QByteArray scratchpad;
    QByteArray eeprom;
};

#endif // DS1820_H

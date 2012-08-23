#
# Auto Makefile
#
# author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
# date:		14.03.2011
#

PROJ_NAME = temperature_control

MCU = atmega16

# if you do not use subdirectories, set them to '.'
SRC_DIR = ./src
BIN_DIR = ./bin
LIST_DIR = ./list
BUILD_DIR = ./build

CFILES = $(shell ls $(SRC_DIR)/*.c)

CC_FLAGS = -Os -Wall
LD_FLAGS = 

VPATH = $(SRC_DIR)
OUT = $(BIN_DIR)/$(PROJ_NAME).elf
FLASH = $(BIN_DIR)/$(PROJ_NAME).ihex
EEPROM = $(BIN_DIR)/$(PROJ_NAME).eep
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CFILES))
DEPS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.d,$(CFILES))

.PHONY: size clean distclean

#all: $(BIN_DIR) $(FLASH) $(EEPROM) $(OUT)
all: $(BIN_DIR) $(FLASH) $(OUT)

-include $(DEPS)

$(BIN_DIR):
	@echo create bin directory: $@
	@mkdir $(BIN_DIR)
	
$(BUILD_DIR):
	@echo create build directory: $@
	@mkdir $(BUILD_DIR)
	
$(LIST_DIR):
	@echo create list directory: $@
	@mkdir $(LIST_DIR)

%.d: ;

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/%.d $(BUILD_DIR) $(LIST_DIR)
	@echo build"\\t"$*.o
	@avr-gcc $(CC_FLAGS) -mmcu=$(MCU) -MMD -Wa,-a=$(LIST_DIR)/$*.list -c -o $@ $<
	
$(OUT): $(OBJS)
	@echo link "\\n"$(patsubst $(BUILD_DIR)/%.o,"\\t"%.o"\\n",$(OBJS))to $(patsubst $(BIN_DIR)/%,%,$(OUT))
	@avr-gcc $(OBJS) -mmcu=$(MCU) -o $(OUT)
#	@echo
#	@make -s size
	
$(FLASH): $(OUT)
	@echo extract flash content to $(patsubst $(BIN_DIR)/%,%,$(FLASH))
	@avr-objcopy -O ihex -j .text -j .data $< $@
	
$(EEPROM): $(OUT)
	@echo extract eeprom connent to $(patsubst $(BIN_DIR)/%,%,$(EEPROM))
	@avr-objcopy -O ihex -j .eeprom $< $@
	
install_flash: $(FLASH)
	at16prog -dpart=$(MCU) --erase --upload if=$< --verify
	
install_eeprom: $(EEPROM)
	at16prog -dpart=$(MCU) --segment=eeprom --upload if=$< --verify

#install: install_flash install_eeprom
install: install_flash

size: $(OUT)
	avr-size -A $(OUT)

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -rf $(BUILD_DIR)
	rm -f $(LIST_DIR)/*.list
	rm -rf $(LIST_DIR)
	
distclean: clean
	rm -f $(EEPROM)
	rm -f $(FLASH)
	rm -f $(OUT)
	rm -rf $(BIN_DIR)
	

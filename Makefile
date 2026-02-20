TYPE = nano
MODEL = 'arduino:avr:$(TYPE)'
PORT = /dev/ttyUSB0

WOKWI = wokwi-cli
WOKWIFLAGS = --interactive --vcd-file output.vcd

BRATE=9600

ACLI = arduino-cli
AFLAGS_C = --output-dir build/ --libraries ./libraries --build-property "compiler.cpp.extra_flags=-I./src"
AFLAGS_U =

MAIN = ./GameConsoleNano.ino

.PHONY: all compile upload monitor clean

all: clean compile wokwi

compile:
	@echo "-- Compiling..."
	$(ACLI) compile $(AFLAGS_C) -b $(MODEL) $(MAIN)

wokwi:
	@echo "-- Run simulation with wokwi..."
	$(WOKWI) . $(WOKWIFLAGS)

upload: compile
	@echo "-- Flashing..."
	$(ACLI) upload $(AFLAGS_U) -p $(PORT) -b $(MODEL)

monitor:
	@echo "-- Monitoring..."
	$(ACLI) monitor -p $(PORT) --config baudrate=$(BRATE)

clean:
	@rm -rf build/*

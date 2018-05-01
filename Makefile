CXXFLAGS := -pthread -std=c++11 -Isrc -Iiniparser/src -I$(GENN_ROBOTICS_PATH)/common `pkg-config --cflags opencv x11`
INIPARSER := iniparser/libiniparser.a
LDFLAGS := -O2 -lstdc++ -lm `pkg-config --libs opencv x11` $(INIPARSER)
OBJ_DIR := ./build
TARGET := beerobot
OBJECTS := \
	${OBJ_DIR}/beeeye.o \
	${OBJ_DIR}/beerobot.o \
	${OBJ_DIR}/imagereceiver.o \
	${OBJ_DIR}/imagesender.o \
	${OBJ_DIR}/mainclient.o \
	${OBJ_DIR}/mainserver.o \
	${OBJ_DIR}/socketcommon.o \
	${OBJ_DIR}/videotype.o
NPROC:=$(shell nproc)
MAKEFLAGS+=-j $(NPROC)

all: build $(TARGET)

$(TARGET): $(OBJECTS) $(INIPARSER)
	$(CXX) -o $(TARGET) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS)
	
$(OBJ_DIR)/%.o: src/%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(INIPARSER):
	$(MAKE) -C iniparser

.PHONY: all build clean

build:
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rf $(OBJ_DIR)
	-@rm -f $(TARGET)


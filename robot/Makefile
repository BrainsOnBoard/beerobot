NAME := beerobot
BUILD := build

ifndef USE_LOCAL_BOB_ROBOTICS
	BOB_ROBOTICS_PATH := bob_robotics
endif
CXXFLAGS := -pthread -std=c++14 -Isrc \
	-I$(BOB_ROBOTICS_PATH) -Wall -Wpedantic
LDFLAGS := -lstdc++ -lm
ifdef DEBUG
	CXXFLAGS += -g
else
	LDFLAGS += -O2
endif

ifdef NO_I2C_ROBOT
	CXXFLAGS += -DNO_I2C_ROBOT
endif

TARGET := $(NAME)
OBJ_DIR := $(BUILD)
CXXFLAGS += `pkg-config --cflags x11`
LDFLAGS += `pkg-config --libs opencv x11`

CXXFLAGS += $(shell pkg-config --cflags opencv)
LDFLAGS += $(shell pkg-config --libs opencv)

.PHONY: all build clean git_submodules

all: build $(TARGET)

$(TARGET): git_submodules $(OBJECTS)
	$(CXX) -o $(TARGET) src/$(TARGET).cc $(CXXFLAGS) $(LDFLAGS)

git_submodules:
	git submodule update --init

build:
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rf $(BUILD)
	-@rm -f $(NAME)

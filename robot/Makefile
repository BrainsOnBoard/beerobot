EXECUTABLE := beerobot
ifndef USE_LOCAL_BOB_ROBOTICS
	BOB_ROBOTICS_PATH := bob_robotics
endif

include $(BOB_ROBOTICS_PATH)/common/flags.mk
CXXFLAGS += -I$(BOB_ROBOTICS_PATH)

ifdef NO_I2C_ROBOT
	CXXFLAGS += -DNO_I2C_ROBOT
endif

CXXFLAGS += `pkg-config --cflags x11`
LINK_FLAGS += `pkg-config --libs x11`

.PHONY: all clean git_submodules

all: $(EXECUTABLE)

-include $(EXECUTABLE).d

$(EXECUTABLE): src/$(EXECUTABLE).cc $(EXECUTABLE).d git_submodules 
	$(CXX) -o $(EXECUTABLE) src/$(EXECUTABLE).cc $(CXXFLAGS) $(LINK_FLAGS)

git_submodules:
	git submodule update --init

%.d: ;

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).d

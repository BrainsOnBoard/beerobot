CXX := x86_64-w64-mingw32-g++
CC := x86_64-w64-mingw32-gcc
#INIPARSER := iniparser/libiniparser.a
CXXFLAGS := -static -static-libgcc -static-libstdc++ -pthread -std=c++11 -L/usr/x86_64-w64-mingw32 -Isrc -Iiniparser/src -I$(GENN_ROBOTICS_PATH)/common -mwindows -L/usr/x86_64-w64-mingw32/lib `x86_64-w64-mingw32-pkg-config --cflags opencv`
LDFLAGS := -O2 -lstdc++ -lm -lws2_32 -lwsock32 -lmingw32 /usr/x86_64-w64-mingw32/lib/*.a
OBJ_DIR := ./build
TARGET := beerobot.exe
OBJECTS := \
	${OBJ_DIR}/beeeye.o \
	${OBJ_DIR}/beerobot.o \
	${OBJ_DIR}/imagereceiver.o \
	${OBJ_DIR}/imagesender.o \
	${OBJ_DIR}/mainclient.o \
	${OBJ_DIR}/mainserver.o \
	${OBJ_DIR}/socketcommon.o \
	${OBJ_DIR}/videotype.o \
	${OBJ_DIR}/iniparser.o \
	${OBJ_DIR}/dictionary.o

all: build $(TARGET)

$(TARGET): $(OBJECTS) $(INIPARSER)
	$(CXX) -o $(TARGET) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS)
	
$(OBJ_DIR)/%.o: src/%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

iniparser: ${OBJ_DIR}/{iniparser,dictionary}.o

${OBJ_DIR}/iniparser.o:
	$(CC) -O2 -c -o ${OBJ_DIR}/iniparser.o iniparser/src/iniparser.c

${OBJ_DIR}/dictionary.o:
	$(CC) -O2 -c -o ${OBJ_DIR}/dictionary.o iniparser/src/dictionary.c	

.PHONY: all build clean iniparser

build:
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rf $(OBJ_DIR)
	-@rm -f $(TARGET) iniparser/libiniparser.* iniparser/src/iniparser/*.o

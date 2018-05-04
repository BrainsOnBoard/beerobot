NAME := beerobot
BUILD := build

CXXFLAGS := -pthread -std=c++11 -Isrc -Iiniparser/src \
	-I$(GENN_ROBOTICS_PATH)/common
LDFLAGS := -O2 -lstdc++ -lm

ifdef MINGW
	TARGET := $(NAME).exe
	OBJ_DIR := $(BUILD)_mingw
	_PREFIX := x86_64-w64-mingw32-
	LDFLAGS += -static -static-libgcc -static-libstdc++ \
		-lwsock32 -lmingw32 \
		-L/usr/x86_64-w64-mingw32/lib \
		-L/usr/x86_64-w64-mingw32/share/OpenCV/3rdparty/lib \
		-L/usr/x86_64-w64-mingw32/lib -lopencv_stitching341 \
		-lopencv_superres341 -lopencv_videostab341 -lopencv_aruco341 \
		-lopencv_bgsegm341 -lopencv_bioinspired341 -lopencv_ccalib341 \
		-lopencv_dnn_objdetect341 -lopencv_dpm341 -lopencv_face341 \
		-lopencv_photo341 -lopencv_fuzzy341 -lopencv_hfs341 -lopencv_img_hash341 \
		-lopencv_line_descriptor341 -lopencv_optflow341 -lopencv_reg341 \
		-lopencv_rgbd341 -lopencv_saliency341 -lopencv_stereo341 \
		-lopencv_structured_light341 -lopencv_phase_unwrapping341 \
		-lopencv_surface_matching341 -lopencv_tracking341 -lopencv_datasets341 \
		-lopencv_text341 -lopencv_dnn341 -lopencv_plot341 \
		-lopencv_xfeatures2d341 -lopencv_shape341 -lopencv_video341 \
		-lopencv_ml341 -lopencv_ximgproc341 -lopencv_calib3d341 \
		-lopencv_features2d341 -lopencv_highgui341 	-lopencv_videoio341 \
		-lopencv_flann341 -lopencv_xobjdetect341 -lopencv_imgcodecs341 \
		-lopencv_objdetect341 -lopencv_xphoto341 -lopencv_imgproc341 \
		-lopencv_core341 -lprotobuf -llapack -lblas \
		-lcblas -lcomctl32 -lgdi32 -lole32 -lsetupapi -lws2_32 -lavifil32 \
		-lavicap32 -lwinmm -lmsvfw32 -lgdi32 -lcomdlg32 -ljpeg -ltiff -llzma \
		-ljpeg -lz -lpng16 -lwebp
else
	TARGET := $(NAME)
	OBJ_DIR := $(BUILD)
	CXXFLAGS += `pkg-config --cflags x11`
	LDFLAGS += `pkg-config --libs opencv x11`
endif

CXXFLAGS += $(shell $(_PREFIX)pkg-config --cflags opencv)

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

$(TARGET): $(OBJECTS)
	$(_PREFIX)$(CXX) -o $(TARGET) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS)
	
$(OBJ_DIR)/%.o: src/%.cc
	$(_PREFIX)$(CXX) -o $@ -c $< $(CXXFLAGS)

${OBJ_DIR}/iniparser.o:
	$(_PREFIX)$(CC) -O2 -c -o ${OBJ_DIR}/iniparser.o iniparser/src/iniparser.c

${OBJ_DIR}/dictionary.o:
	$(_PREFIX)$(CC) -O2 -c -o ${OBJ_DIR}/dictionary.o iniparser/src/dictionary.c

.PHONY: all build clean

build:
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rf $(BUILD){,_mingw}
	-@rm -f $(NAME){,.exe}

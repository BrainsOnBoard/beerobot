NAME := beerobot
BUILD := build

ifndef USE_LOCAL_GENN_ROBOTICS
	GENN_ROBOTICS_PATH := GeNN_Robotics
endif
CXXFLAGS := -g -pthread -std=c++11 -Isrc \
	-I$(GENN_ROBOTICS_PATH) -Wall
LDFLAGS := -O2 -lstdc++ -lm

ifdef NO_I2C_ROBOT
	CXXFLAGS += -DNO_I2C_ROBOT
endif

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
LDFLAGS += $(shell $(_PREFIX)pkg-config --libs opencv)

.PHONY: all build clean git_submodules mingw

all: build $(TARGET)

$(TARGET): git_submodules $(OBJECTS)
	$(_PREFIX)$(CXX) -o $(TARGET) src/$(TARGET).cc $(CXXFLAGS) $(LDFLAGS)

git_submodules:
	git submodule update --init

mingw:
	MINGW=1 $(MAKE)

build:
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rf $(BUILD){,_mingw}
	-@rm -f $(NAME){,.exe}

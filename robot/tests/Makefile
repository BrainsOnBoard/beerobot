.PHONY: all clean

CXXFLAGS=-g -I../src `pkg-config --cflags --libs opencv`

all: yaml_read_test yaml_write_test

yaml_read_test: yaml_read_test.cc ../src/image/camparams.h
	$(CXX) -o $@ $< $(CXXFLAGS)

yaml_write_test: yaml_write_test.cc ../src/image/camparams.h
	$(CXX) -o $@ $< $(CXXFLAGS)

clean:
	rm -f yaml_read_test yaml_write_test{,.yaml}

.PHONY: all clean

CXXFLAGS=-g -I../src `pkg-config --cflags --libs yaml-cpp opencv`

all: yaml_read_test yaml_write_test

yaml_read_test: yaml_read_test.cc
	$(CXX) -o $@ $< $(CXXFLAGS)

yaml_write_test: yaml_write_test.cc
	$(CXX) -o $@ $< $(CXXFLAGS)

clean:
	rm -f yaml_read_test yaml_write_test{,.yaml}

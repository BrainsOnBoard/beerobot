.PHONY: all clean

all: yaml_read_test yaml_write_test

yaml_read_test: yaml_read_test.cc
	$(CXX) -o $@ $< `pkg-config --cflags --libs yaml-cpp opencv`

yaml_write_test: yaml_write_test.cc
	$(CXX) -o $@ $< `pkg-config --cflags --libs yaml-cpp opencv`

clean:
	rm -f yaml_read_test yaml_write_test{,.yaml}

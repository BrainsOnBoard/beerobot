.PHONY: clean

yaml_read_test: yaml_read_test.cc
	$(CXX) -o yaml_read_test yaml_read_test.cc `pkg-config --cflags --libs yaml-cpp opencv`

clean:
	rm -f yaml_read_test
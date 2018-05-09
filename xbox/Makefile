.PHONY: clean

xbox_test: xbox_test.cc xbox_linux.h xbox_defines.h
	$(CXX) -O2 -o xbox_test -pthread xbox_test.cc

clean:
	rm -f xbox_test

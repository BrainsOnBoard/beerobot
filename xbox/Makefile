.PHONY: clean

xbox_test: xbox_test.cc xbox.h xbox_linux.h xbox_base.h
	$(CXX) -o xbox_test -pthread xbox_test.cc

clean:
	rm -f xbox_test

CXX := g++
CXXFLAGS := -std=c++98 -Wall -Wextra -g
LIB_SRC := src/http.cc src/request_parser.cc src/request.cc src/response.cc src/worker.cc src/configuration.cc src/static_file_handler.cc

all:
	$(CXX) $(CXXFLAGS) $(LIB_SRC) src/main.cc -o httpd98

tests:
	$(CXX) $(CXXFLAGS) -Isrc $(LIB_SRC) test/*.cc -o run_tests
	./run_tests

clean:
	rm -f httpd98
	rm -f run_tests
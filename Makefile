all:
	g++ -std=c++98 -Wall -Wextra -g src/*.cc -o httpd98

clean:
	rm httpd98
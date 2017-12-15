all:
	g++ -O3 -m32 -std=c++11 main.cpp Stack.cpp Function.cpp -o mxf.exe -Wall -Werror -Wextra -pedantic

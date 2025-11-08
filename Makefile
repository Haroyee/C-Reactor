CC:=g++
CFLAGS:=-Wall -Iinclude -g -std=c++11
LDFLAGS:= -Llib
LDLIBS:= -lpthread

# 为 .cpp 文件指定搜索目录：src/
vpath %.cpp src/
#为 .o 文件指定搜索目录：build/
vpath %.o build/


all:


build/%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)



clean:
	rm -f build/*.o
	rm -f output/*.exe


.PHONY: clean all

run: all
	echo "执行完毕"



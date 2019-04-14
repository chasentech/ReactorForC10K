# 源文件目录 输出执行文件目录 头文件目录
SRC_PATH = ./src
OUT_PATH = ./output
INCLUDE = -I./include -I/usr/local/include

# 在./src文件夹中查找.cpp文件
SRC = $(wildcard $(SRC_PATH)/*.cpp)
# .cpp文件名生成.o文件名
OBJ = $(patsubst %.cpp, $(OUT_PATH)/%.o, $(notdir $(SRC)))

#自定义依赖的库
USER_LIB = -lpthread -lv4l2

# 编译器参数 目标名称
CPPFLAGS = -std=c++11 -Wall
TARGET = cli
G+ = g++

$(TARGET): $(OBJ)
	@$(G+) $(OBJ) $(USER_LIB) $(OPCV_LIBS_PATH) $(OPCV_LIBS) -o $(TARGET)

$(OUT_PATH)/%.o: $(SRC_PATH)/%.cpp
	@$(G+) $(INCLUDE) $(CPPFLAGS) -c $< -o $@

# 虚目标:删除生成的文件 
.PHONY:clean
clean:
	rm -f $(TARGET) $(OBJ)




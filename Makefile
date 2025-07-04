CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

SRC_DIR = src
INCLUDE_DIRS = -Iinclude -Icommon -Imycommon

TARGET = tanks_game

SRC = $(wildcard $(SRC_DIR)/*.cpp)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $^ -o $@

clean:
	rm -f $(TARGET)

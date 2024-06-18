CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

TARGET = poll

SRC = poll.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

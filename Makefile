CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic-errors -I.

SFML_LIBS := $(shell pkg-config sfml-all --libs 2>/dev/null || echo -lsfml-graphics -lsfml-window -lsfml-system)

SRCS := main.cpp $(shell find Game -name "*.cpp")

all: app

app: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(SFML_LIBS) -o app

clean:
	rm -f app
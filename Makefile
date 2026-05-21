CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic-errors

SFML_LIBS := $(shell pkg-config sfml-all --libs 2>/dev/null || echo -lsfml-graphics -lsfml-window -lsfml-system)

all: app

app: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp $(SFML_LIBS) -o app

clean:
	rm -f app
CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
DEPS = test.h mfs.h
OBJ = test.o mirofs.o 

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

test: $(OBJ)
	$(CXX) -o $@ $^ -I. $(LDFLAGS)

all: test

clean:
	rm test $(OBJ)

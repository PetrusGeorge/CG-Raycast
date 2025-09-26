CXX = g++
CXXFLAGS = -std=c++17 -O3
LDFLAGS = -lglut -lGLU -lGL

# Ativa OpenMP para paralelização (Opcional)
CXXFLAGS += -fopenmp
LDFLAGS += -fopenmp

TARGET = raycast

SRCDIR = src
OBJDIR = obj

SRCS = main.cpp Renderer.cpp
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

$(shell mkdir -p $(OBJDIR) $(BINDIR))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

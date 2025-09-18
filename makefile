CXX = g++
CXXFLAGS = -std=c++17 -O3
LDFLAGS = -lglut -lGLU -lGL

# Enable OpenMP for parallel rendering (optional)
CXXFLAGS += -fopenmp
LDFLAGS += -fopenmp

TARGET = raycast

SRCDIR = src
OBJDIR = obj

SRCS = main.cpp Renderer.cpp
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
# DEPS = $(OBJS:.o=.d)

# Header files (for dependency tracking)
# HEADERS = $(INCDIR)/Renderer.h $(INCDIR)/Camera.h $(INCDIR)/Vector3.h

$(shell mkdir -p $(OBJDIR) $(BINDIR))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Include dependencies
# -include $(DEPS)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run debug profile help install-deps-ubuntu install-deps-macos
# # Prevent make from deleting intermediate files
# .SECONDARY:

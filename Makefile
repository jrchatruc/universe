TARGET=universe
TARGET_RELEASE=release

CXX=g++
CXXFLAGS=-std=c++11 -Wall -I/usr/local/include
LNFLAGS=-L/usr/local/lib -lSDL2
RELEASE_FLAGS=-O3

OBJDIR=build
SRCDIR=src
RLSDIR=rls

OBJECTS=$(addprefix $(OBJDIR)/, main.o linalg.o universe.o renderer.o)
RLSOBJECTS=$(addprefix $(RLSDIR)/, main.o linalg.o universe.o renderer.o)
HEADERS=$(addprefix $(SRCDIR)/, linalg.h universe.h renderer.h)

default: $(TARGET)

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(RLSOBJECTS): | $(RLSDIR)

$(RLSDIR):
	mkdir -p $(RLSDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(RLSDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) $(FLAGS) $(LNFLAGS) $(OBJECTS) -o $(OBJDIR)/$(TARGET)

$(TARGET_RELEASE): $(RLSOBJECTS)
	$(CXX) $(FLAGS) $(RELEASE_FLAGS) $(LNFLAGS) $(RLSOBJECTS) -o $(RLSDIR)/$(TARGET_RELEASE)

run: $(TARGET)
	./$(OBJDIR)/$(TARGET)

run_release: $(TARGET_RELEASE)
	./$(RLSDIR)/$(TARGET_RELEASE)

clean:
	rm -rf $(OBJDIR) \
	rm -rf $(RLSDIR)
CXX ?= c++

CXXFLAGS ?= -std=c++17 -Wall -Wextra
CPPFLAGS += -MMD -MP

SRCS := $(wildcard *.cpp)
OBJDIR := build/obj
BUILDDIR := build
TARGET := $(BUILDDIR)/termrex
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS := $(OBJS:.o=.d)

PREFIX  ?= /usr
BINDIR  ?= $(PREFIX)/games
DESTDIR ?=

.PHONY: all clean run debug release install uninstall

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

$(BUILDDIR):
	mkdir -p $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

debug: CXXFLAGS += -g -O0
debug: all

release: CXXFLAGS += -O2
release: all

install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/termrex

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/termrex

clean:
	rm -rf build

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		elf_parser.o

LIBS =

TARGET =	elf_parser

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

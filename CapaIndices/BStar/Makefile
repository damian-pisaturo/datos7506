CXXFLAGS =	-O2 -g -Wall

OBJS =		bstar.o

LIBS =

TARGET =	bstar.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

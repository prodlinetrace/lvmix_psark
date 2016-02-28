CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -fpermissive

OBJS =		psark.o

LIBS = 

PRODALIB =  prodllng
PRODALIBPATH = prodllng\Release_11g

TARGET =	psark.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -l $(PRODALIB) -L $(PRODALIBPATH) -static-libgcc -static-libstdc++ 

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

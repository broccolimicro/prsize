CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =	src/chp.o src/common.o src/constant.o src/expression.o src/instance.o src/instruction.o src/message.o src/rule.o src/tokenizer.o src/variable_name.o src/variable.o src/variable_space.o

LIBS =

TARGET =	size

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	

OBJS = main.o sessionization.o utils.o

EXEDIR = ../bin/
EXEC = ${EXEDIR}SGenerator
SRCS = $(OBJS:.o=.cpp)


CC = g++
LDR = g++
CFLAGS = -O0 -g
CFLAGS += -std=c++11

# implicit rules
%.o: %.cpp
	$(CC)  $(CFLAGS) -c  $<
	
# targets
all:	compile

.PHONY: compile
compile: ${EXEC}

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)   #$(LDFLAGS)


clean:
	rm -f *.o  $(EXEC)



CC = g++
CFLAGS = -O0 -g
CFLAGS += -std=c++11

EXEC = SGenerator
OBJS = main.o sessionization.o utils.o
SRCS = $(OBJ:.o=.cpp)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	-\rm  $(EXEC) $(OBJS)


CC = g++
CFLAGS = -Wall -O3
LDFLAGS = -flto
LIBS = -lm -lhwloc -lboost_graph
SRC = ./src/
#BUILD = ./build/
INCLUDE = ./include/
OBJS = init_graph.o  main.o# output_graph.o
HEADER = ./include/hwloc-test.h ./include/init_graph.h ./include/output_graph.h ./include/distances.h ./include/visitors.h ./include/predicates.h ./include/algo.h 



all: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${LIBS} ${OBJS} ${HEADER} -o hwloc-test

main.o: ${SRC}main.cc
	${CC} ${CFLAGS} -c $<

init_graph.o: ${SRC}init_graph.cc
	${CC} ${CFLAGS} -c $<

#output_graph.o: ${SRC}output_graph.cc
#	${CC} ${CFLAGS} -c $<

clean:
	-rm -f *.o *.svg *.pdf *.dot

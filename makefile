PROJECT=index
CC=g++
BOOST_PATH=/usr/lib/boost

${PROJECT}_test: ${PROJECT}.o ${PROJECT}_test.o
	${CC} -std=c++11 -I${BOOST_PATH} -o ${PROJECT}_test ${PROJECT}.o ${PROJECT}_test.o

${PROJECT}_test.o: ${PROJECT}_test.cpp
	${CC} -std=c++11 -I${BOOST_PATH} -c ${PROJECT}_test.cpp

${PROJECT}.o: ${PROJECT}.cpp
	${CC} -std=c++11 -I${BOOST_PATH} -c ${PROJECT}.cpp

clean:
	rm -rf *.o



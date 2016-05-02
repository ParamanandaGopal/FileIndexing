PROJECT=index
CC=g++
CC += -std=c++11
CC += -g
BOOST_INCLUDE_PATH=/usr/local/include
BOOST_LIB_PATH=/usr/local/lib
TCLAP_PATH=/usr/local
LIBS=-lboost_thread-mt -lboost_system-mt -lboost_regex-mt -lboost_program_options-mt


${PROJECT}_test: ${PROJECT}.o ${PROJECT}_test.o
	${CC} -g3 -fno-omit-frame-pointer  -std=c++11 -Wall -o ${PROJECT}_test ${PROJECT}.o ${PROJECT}_test.o -L${BOOST_LIB_PATH} ${LIBS}

${PROJECT}_test.o: ${PROJECT}_test.cpp
	${CC} -g3 -fno-omit-frame-pointer -std=c++11 -Wall -I${TCLAP_PATH} -I${BOOST_INCLUDE_PATH} -c ${PROJECT}_test.cpp

${PROJECT}.o: ${PROJECT}.cpp
	${CC} -g3 -fno-omit-frame-pointer  -std=c++11 -Wall -I${TCLAP_PATH} -I${BOOST_INCLUDE_PATH} -c ${PROJECT}.cpp

clean:
	rm -rf *.o



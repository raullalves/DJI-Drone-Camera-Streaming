CC = g++ -std=c++11
CFLAGS = -g -Wall
SRCS = VideoSocket.cpp Decodificador.cpp droneControleFaceDetection.cpp
PROG = drone

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS) 
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
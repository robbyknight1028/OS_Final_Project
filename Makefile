CC	= g++
LDFLAGS	= -lm -lnsl -lpthread
CFLAGS	= -ggdb
TARGET	= server client

default: $(TARGET)

server: server.o
	g++ $(CFLAGS) -o server server.cpp sem.cpp FileGet.cpp commandLineCheck.cpp dispatch.cpp scheduler_factory.cpp task.cpp scheduler.cpp scheduler_FIFO.cpp header.cpp $(LDFLAGS)

client: client.o
	g++ $(CFLAGS) -o client client.cpp commandLineCheck.cpp lineParser.cpp request.cpp getLine.cpp excessParser.cpp $(LDFLAGS)

clean:
	-rm -f *.o *~

cleanall: clean
	-rm -f $(TARGET) 



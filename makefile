LIBS := -lpthread -lrt

all: printer client

printer: printer.c
	gcc queue.c $^ -o $@ $(LIBS)

client: client.c
	gcc queue.c $^ -o $@ $(LIBS)

clean:
	-rm printer client

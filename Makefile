CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
SOURCES= mandel.c jpegrw.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mandel
MOVIE_LDFLAGS=-lm
MOVIE_SOURCES= mandelMovie.c
MOVIE_OBJECTS=$(MOVIE_SOURCES:.c=.o)
MOVIE_EXECUTABLE=mandelMovie

all: $(SOURCES) $(MOVIE_SOURCES) $(EXECUTABLE) $(MOVIE_EXECUTABLE)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d) $(MOVIE_OBJECTS:.o=.d)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(MOVIE_EXECUTABLE): $(MOVIE_OBJECTS)
	$(CC) $(MOVIE_OBJECTS) $(MOVIE_LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
	rm -rf $(OBJECTS) $(MOVIE_OBJECTS) $(EXECUTABLE) $(MOVIE_EXECUTABLE) *.d
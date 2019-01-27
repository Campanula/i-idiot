target := draw

all: $(target)

draw: new.o Point.o Circle.o main.o
	$(CC) -o $@ $^

clean:
	-rm -f $(target) *.o


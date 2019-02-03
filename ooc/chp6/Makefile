target := any points circles
object := Object.o Point.o Circle.o

all: $(target)

any: any.o $(object)
	$(CC) -o $@ $^

points: points.o $(object)
	$(CC) -o $@ $^

circles: circles.o $(object)
	$(CC) -o $@ $^

clean:
	-rm -f $(target) *.o


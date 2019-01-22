target := main
objects := main.o Set.o

all: $(target)

main: $(objects)
	$(CC) -o $@ $(objects)

clean:
	-rm -f $(target) $(objects)


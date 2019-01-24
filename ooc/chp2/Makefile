target := testString testAtom

all: $(target)

testString: new.o String.o main.o
	$(CC) -o $@ $^

testAtom: new.o Atom.o main.o
	$(CC) -o $@ $^

clean:
	-rm -f $(target) *.o


target := testString testAtom

all: $(target)

testString: new.o String.o testString.o
	$(CC) -o $@ $^

testAtom: new.o Atom.o testString.o
	$(CC) -o $@ $^

clean:
	-rm -f $(target) new.o String.o testString.o Atom.o testAtom.o


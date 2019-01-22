target := testString
objects := new.o String.o testString.o

all: $(target)

testString: $(objects)
	$(CC) -o $@ $(objects)

clean:
	-rm -f $(target) $(objects)


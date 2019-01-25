target := value postfix infix

all: $(target)

value: value.o parse.o
	$(CC) -o $@ $^

postfix: postfix.o parse.o
	$(CC) -o $@ $^

infix: infix.o parse.o
	$(CC) -o $@ $^

clean:
	-rm -f $(target) *.o


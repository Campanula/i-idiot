target := parse

all: $(target)

parse: binary.o Name.o value.o Var.o mathlib.o parse.o
	$(CC) -o $@ $^ -lm

clean:
	-rm -f $(target) *.o


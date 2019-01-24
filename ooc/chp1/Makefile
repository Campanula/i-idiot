target := testSet testBag

all: $(target)

testSet: Set.o main.o
	$(CC) -o $@ $^

testBag: Bag.o main.o
	$(CC) -o $@ $^

clean:
	-rm -f $(target) *.o


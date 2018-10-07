target := i_idiot maxlen_line
i_objects := hello.o fahr_celsius.o echo_eof.o count_char.o
m_objects := text_line.o maxlen_line.o

all: $(target)

i_idiot: $(i_objects)
	$(CC) -o $@ $(i_objects)

maxlen_line: $(m_objects)
	$(CC) -o $@ $(m_objects)

clean:
	-rm -f $(i_objects) $(m_objects) $(target)

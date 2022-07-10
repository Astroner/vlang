SOURCES = main.c linkedList/linkedList.c utils/utils.c token/token.c lexemes/lexemes.c ast/ast.c HashTable/HashTable.c runtime/runtime.c
OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = ./start

build: $(OBJECTS)
	gcc -o $(EXECUTABLE) $^

start: build
	$(EXECUTABLE) main.vlang

.c.o: 
	gcc -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
SOURCES = main.c linkedList/linkedList.c utils/utils.c token/token.c ast/ast.c ast/creators/creators.c ast/parsers/parsers.c ast/parsers/parseTokenExpression.c HashTable/HashTable.c runtime/runtime.c
OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = ./vlang.exe

build: $(OBJECTS)
	gcc -o $(EXECUTABLE) $^

start: build
	$(EXECUTABLE) main.vlang

.c.o: 
	gcc -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
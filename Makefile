SOURCES = main.c \
		  linkedList/linkedList.c \
		  utils/utils.c \
		  token/token.c \
		  ast/ast.c \
		  	ast/creators/creators.c \
			ast/parsers/parsers.c \
			ast/parsers/parseExpression.c \
			ast/parsers/parseStatements.c \
		  HashTable/HashTable.c \
		  runtime/runtime.c \
		  	runtime/native/native.c \
			runtime/runners/runners.c \
			runtime/runners/runNode.c \
			runtime/runtime-utils/runtime-utils.c \
		  Scopes/Scopes.c
OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = ./vlang

build: $(OBJECTS)
	gcc -o $(EXECUTABLE) $^

start: build
	$(EXECUTABLE) main.vlang $(START_FLAGS)

.c.o: 
	gcc -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
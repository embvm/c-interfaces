all: main.o

main.o: test/main.c
	@$(CC) -I. -c $< -o $@
	@$(RM) $@

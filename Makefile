all: main.o

ALL_HEADERS=$(wildcard interface_patterns/*.h) \
 $(wildcard os/*.h) \
 $(wildcard template_methods/*.h) \
 $(wildcard virtual_devices/*.h) \

main.o: test/main.c
	@$(CC) -I. -c $< -o $@
	@$(RM) $@

format:
	@ clang-format -i $(ALL_HEADERS)

docs:
	@ mkdir -p buildresults/documentation
	@ doxygen

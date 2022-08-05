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

.PHONY: docs
docs:
	@ mkdir -p buildresults/docs
	@ doxygen

.PHONY: deploy-docs
deploy-docs: docs
	@ git branch | grep -q "gh-pages" && git branch -D gh-pages
	@ git branch gh-pages master
	@ git checkout gh-pages
	@ mv buildresults/docs/html/ docs/
	@ git add docs/
	@ git commit -am "Publish generated documentation"
	@ git push -f origin gh-pages:gh-pages
	@ git checkout -
	@ rm -rf docs/

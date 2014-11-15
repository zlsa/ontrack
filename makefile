
export NAME=ontrack

all: src

src:
	$(MAKE) -C src src

install:
	cp src/$(NAME) bin/

clean:
	$(MAKE) -C src clean

.PHONY: all src install clean

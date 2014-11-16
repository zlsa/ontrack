
export NAME=ontrack

all: src release

src:
	$(MAKE) -C src src

install:
	cp src/$(NAME) bin/

clean:
	$(MAKE) -C src clean

release:
	svg2png data/logo/*.svg
	cp data/logo/*.png tools/railway-edit/assets/logo/

.PHONY: all src install clean

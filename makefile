
export NAME=ontrack

all: src release

src:
	$(MAKE) -C src src

install:
	cp src/$(NAME) bin/

clean:
	$(MAKE) -C src clean

release: convert-svg
	cp data/logo/*.png  tools/route-creator/assets/logo/
	cp data/icons/*.png tools/route-creator/assets/icons/

convert-svg:
	svg2png data/logo/*.svg
	svg2png data/icons/*.svg

.PHONY: all src install clean

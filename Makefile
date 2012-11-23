# Top level makefile.

all:
	cd src && $(MAKE) $@
	cd cli && $(MAKE) $@
	cd formula && $(MAKE) $@

clean:
	cd src && $(MAKE) $@
	cd cli && $(MAKE) $@
	cd formula && $(MAKE) $@
	find . -name nohup.out |xargs rm -rf

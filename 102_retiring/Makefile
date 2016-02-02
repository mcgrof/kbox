all: modules

.PHONY: modules
modules:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

.PHONY: clean
clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

obj-m += sandbox.o

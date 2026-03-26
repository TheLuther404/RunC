CC       ?= gcc
CFLAGS   ?= -Wall -Wextra -O2
PREFIX   ?= /usr/local

SRC      = src/main.c src/cli.c src/config.c src/compiler.c src/utils.c
TARGET   = runc

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)
	ln -sf $(PREFIX)/bin/$(TARGET) $(PREFIX)/bin/rc
	@echo "Installed runc and rc to $(PREFIX)/bin/"

uninstall:
	rm -f $(PREFIX)/bin/runc $(PREFIX)/bin/rc
	@echo "Uninstalled runc and rc from $(PREFIX)/bin/"

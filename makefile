
CC       ?= gcc
CFLAGS   ?= -std=c11 -Wall -Wextra -g
CPPFLAGS ?= 
LDFLAGS  ?=
LDLIBS   ?= 

NAME     ?= sssaf24-tyf24-gtp24

NAME_TGZ   ?= $(NAME).tgz

TARGET = fdtool

SRCS = $(wildcard *.c)
OBJDIR := obj
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

.PHONY: all tgz clean purge
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

tgz:
	@echo "Packaging all source code into $(TARGET)..."
	@tar -czf /tmp/$(TARGET) \
		--exclude='*.tgz' \
		--exclude='*.o' \
		--exclude='.git' \
		--exclude='.vscode' \
		. && mv /tmp/$(TARGET) .


clean:
	rm -rf $(OBJDIR)

purge: clean
	rm -f $(TARGET)
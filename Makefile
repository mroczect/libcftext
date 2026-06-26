CC      ?= gcc
AR      ?= ar
CFLAGS  ?= -Wall -Wextra -O2
CFLAGS  += -Iinclude
LDFLAGS := -L. -lcftext

SRCDIR    = src
OBJDIR    = build

LIB_SRCS  = $(SRCDIR)/core.c $(SRCDIR)/error.c $(SRCDIR)/version.c
LIB_OBJS  = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIB_SRCS))

LIB_STATIC = libcftext.a
TARGET     = libftext

.PHONY: all clean install uninstall

all: $(LIB_STATIC) $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c include/libcftext.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIB_STATIC): $(LIB_OBJS)
	$(AR) rcs $@ $^

$(TARGET): main.c $(LIB_STATIC)
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $@

install: $(LIB_STATIC) include/libcftext.h
	install -d $(DESTDIR)/usr/local/lib
	install -m 644 $(LIB_STATIC) $(DESTDIR)/usr/local/lib/
	install -d $(DESTDIR)/usr/local/include
	install -m 644 include/libcftext.h $(DESTDIR)/usr/local/include/

uninstall:
	rm -f $(DESTDIR)/usr/local/lib/$(LIB_STATIC)
	rm -f $(DESTDIR)/usr/local/include/libcftext.h

clean:
	rm -rf $(OBJDIR) $(LIB_STATIC) $(TARGET)
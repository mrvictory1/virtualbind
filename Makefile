CC := gcc
XFLAGS := -Wall
LIBRARIES := -levdev
INCLUDES := -I/usr/include/libevdev-1.0
CFLAGS := $(XFLAGS) $(INCLUDES)

OUTDIR := out
SOURCES := virtualbind.c
OBJS := $(SOURCES:%.c=$(OUTDIR)/%.o)
TARGET := virtualbind

.PHONY: all clean

$(OUTDIR)/%.o: %.c
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@

all: $(TARGET)
clean:
	rm -rf $(OUTDIR)

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SRCS = main.c vk_instance.c validation_layers.c app.c
OBJS = $(SRCS:%.c=bin/%.o)
TARGET = bin/VulkanTest

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

bin/%.o: %.c | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p bin

.PHONY: test clean

test: $(TARGET)
	./$<

clean:
	rm -rf bin/

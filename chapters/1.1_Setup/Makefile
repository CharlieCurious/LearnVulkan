CFLAGS = -std=c11 -O1 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
LIBS = utils.h utils.c

VulkanTest: main.c
	gcc $(CFLAGS) -o VulkanTest main.c $(LIBS) $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest

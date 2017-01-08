LUA_INC=$(shell pkg-config --cflags lua51)
LUA_LIB=$(shell pkg-config --libs lua51)
CFLAGS=$(LUA_INC) -fpic -O2
LDFLAGS=$(LUA_LIB) -fpic -shared

CBUFFER_SO=cbuffer.so
CBUFFER_OBJS= \
	buffer.o

build: $(CBUFFER_SO)

install:
	cp $(CBUFFER_SO) $(LUA_LUVDIR)/
	find . -type f -name \*.lua -exec install -D {} $(LUA_LUVDIR)/{} \;

clean:
	rm $(CBUFFER_OBJS) $(CBUFFER_SO)

$(CBUFFER_SO): $(CBUFFER_OBJS)
	$(LD) -o $@ $(CBUFFER_OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

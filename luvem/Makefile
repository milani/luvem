LUA_INC=$(shell pkg-config --cflags lua51)
LUA_LIB=$(shell pkg-config --libs lua51)
CFLAGS=$(LUA_INC) -fpic -O2
LDFLAGS=$(LUA_LIB) -fpic -shared

#LUVEM_OBJS = $(shell find . -name '*.lua' | sed '{s/\.lua/\.l/};{s/\.\///}')
#LUVEM_SO=luvem.so
CBUFFER_SO=cbuffer.so
CBUFFER_OBJS=buffer.o

build: $(CBUFFER_SO)

#bytecode: $(LUVEM_SO)

install:
	cp $(CBUFFER_SO) $(LUA_LUVDIR)/
	find . -type f -name \*.lua -exec install -D {} $(LUA_LUVDIR)/{} \;

clean:
	rm -f $(CBUFFER_OBJS) $(CBUFFER_SO) #$(LUVEM_OBJS) $(LUVEM_SO)

$(CBUFFER_SO): $(CBUFFER_OBJS)
	$(LD) -o $@ $(CBUFFER_OBJS) $(LDFLAGS)

#$(LUVEM_SO): $(LUVEM_OBJS)
#	$(LD) -o $@ $(LUVEM_OBJS) $(LDFLAGS) -E

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

#%.l: %.lua
#	luajit -b -t o $< $@

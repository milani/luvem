PREFIX ?= /usr/local
LUAV ?= $(shell lua -e "_,_,v=string.find(_VERSION,'Lua (.+)');print(v)")
LUA_LIBDIR ?= $(PREFIX)/lib/lua/$(LUAV)
LUA_LUVDIR ?= $(LUA_LIBDIR)/luvem

all: build

build:
	$(MAKE) -C luvem $@

install:
	mkdir -p $(LUA_LUVDIR)
	$(MAKE) -C luvem $@ LUA_LIBDIR=$(LUA_LIBDIR) LUA_LUVDIR=$(LUA_LUVDIR)

test:
	busted --run=unit --cpath=./luvem/?.so

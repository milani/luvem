PREFIX ?= /usr/local
LUAV ?= $(shell lua -e "_,_,v=string.find(_VERSION,'Lua (.+)');print(v)")
LUA_LIBDIR ?= $(PREFIX)/lib/lua/$(LUAV)
LUA_LUVDIR ?= $(LUA_LIBDIR)/luv

all: build

build:
	$(MAKE) -C src $@

install:
	mkdir -p $(LUA_LUVDIR)
	$(MAKE) -C src $@ LUA_LIBDIR=$(LUA_LIBDIR) LUA_LUVDIR=$(LUA_LUVDIR)

test:
	busted --run=unit --cpath=./src/?.so

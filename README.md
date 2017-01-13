# Luvem

Luvem is Luvit for the rest of us. Those who don't want to use Luvi.

The fork made for two reasons:

* To drop dependency on Luajit
* To eliminate luvi dependency so that we can use it inside lua ecosystem easily

Also I want to use it on OpenWRT/LEDE, it should be usable on other platforms.

## Dependencies

Originally, Luvit depends on Luajit to run. I removed FFI (and Windows support) to work with pure Lua.

* lua-bitop (not needed on Luajit)
* lua-openssl (not needed if you don't use TLS)
* luv

#define LUA_LIB

#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#define element unsigned char

typedef struct NumArray {
  int size;
  element *values;  /* variable part */
} NumArray;

static void cpyarray(NumArray* dest, NumArray* source, int destindex, int srcindex, int length) {
  memcpy(dest->values + destindex, source->values + srcindex, sizeof(element)*length);
}

static NumArray *initarray(lua_State *L,int length) {
  size_t nbytes = sizeof(NumArray);
  NumArray *a = (NumArray *)lua_newuserdata(L, nbytes);
  element *values = malloc(sizeof(element)*length);
  memset(values, 0, sizeof(element)*length);
  if (values == NULL) {
    lua_pushstring(L,"error allocating buffer");
    lua_error(L);
  }
  a->values = values;
  a->size = length;
  luaL_getmetatable(L, "luv.buffer");
  lua_setmetatable(L, -2);
  return a;
}

static int newarray (lua_State *L) {
  int n = luaL_checkint(L, 1);
  initarray(L,n);
  return 1;  /* new userdatum is already on the stack */
}

static NumArray *checkarray (lua_State *L, int index) {
  void *ud = luaL_checkudata(L, index, "luv.buffer");
  return (NumArray *)ud;
}

static element *getelem (lua_State *L) {
  NumArray *a = checkarray(L, 1);
  int index = luaL_checkint(L, 2);

  luaL_argcheck(L, 1 <= index && index <= a->size, 2,
                   "index out of range");

  /* return element address */
  return &a->values[index - 1];
}

static int setarray (lua_State *L) {
  element newvalue = luaL_checknumber(L, 3);
  *getelem(L) = newvalue;
  return 0;
}

static int getarray (lua_State *L) {
  lua_pushnumber(L, *getelem(L));
  return 1;
}

static int getsize (lua_State *L) {
  NumArray *a = checkarray(L, 1);
  lua_pushnumber(L, a->size);
  return 1;
}

static int concat(lua_State *L) {
  NumArray *a = checkarray(L, 1);
  NumArray *b = checkarray(L, 2);
  // Create new array
  int n = a->size + b->size;
  NumArray *c = initarray(L, n);
  element *values = c->values;
  memcpy(values, a->values, a->size);
  memcpy(values + sizeof(element)*a->size, b->values, b->size);

  return 1;
}

static int resize (lua_State *L) {
  NumArray *a = checkarray(L, 1);
  int newsize = luaL_checkint(L, 2);
  element *values = realloc(a->values,sizeof(element)*newsize);
  if (values == NULL) {
    lua_pushstring(L,"buffer resize failed.");
    lua_error(L);
  }
  a->values = values;
  a->size = newsize;
  lua_pop(L,1);
  return 1;
}

static int sub(lua_State *L) {
  NumArray *a = checkarray(L, 1);
  int index = luaL_checkint(L, 2) - 1;
  int length = a->size - index;// because index starts at 1 not 0
  if (lua_isnumber(L,3))
    length = luaL_checkint(L, 3);

  NumArray *newarr = initarray(L,length);
  cpyarray(newarr,a,0,index,length);
  return 1;
}

static int copy(lua_State *L) {
  NumArray *src;
  int destindex = 1;
  int srcindex = 1;
  int length;
  NumArray *dest = checkarray(L, 1);
  if (lua_isstring(L,2)) {
    size_t srcsize = 0;
    const char* srcvalues = lua_tolstring(L,2,&srcsize);
    src = initarray(L,srcsize);
    strcpy(src->values,srcvalues);
  } else {
    src = checkarray(L, 2);
  }
  if (lua_isnumber(L,3)) {
    destindex = luaL_checkint(L, 3);
    srcindex = luaL_checkint(L, 4);
    length = luaL_checkint(L, 5);
  } else {
    length = src->size;
  }

  luaL_argcheck(L, 1 <= destindex && destindex <= dest->size, 3,
                   "index out of range");

  luaL_argcheck(L, 1 <= srcindex && srcindex <= src->size, 4,
                   "index out of range");

  luaL_argcheck(L, 1 <= length, 5,
                   "length can not be less than 1");

  // check if resize is needed
  if (dest->size < destindex + length - 1) {
      int newsize = dest->size + length - destindex;
      element *values = realloc(dest->values,sizeof(element)*newsize);
      if (values == NULL) {
        lua_pushstring(L,"buffer resize failed.");
        lua_error(L);
      }
      dest->values = values;
      dest->size = newsize;
  }

  // index should be zero based in C
  cpyarray(dest, src, destindex - 1, srcindex - 1, length);
  return 0;
}

static int buffer2string(lua_State *L) {
  NumArray *a = checkarray(L, 1);
  int index = 1;
  int length = a->size;
  if(lua_isnumber(L,2)) {
    index = luaL_checkint(L,2);
    length = luaL_checkint(L,3);
  }
  luaL_argcheck(L, 1 <= index  && index <= a->size, 2,
                   "index out of range");

  luaL_argcheck(L, 1 <= length && index + length - 1 <= a->size, 3,
                   "length out of range");

  // take one extra element to zero-terminate the string
  char *str = malloc(sizeof(char)*length+1);
  memcpy(str,a->values + index - 1,sizeof(char)*length);
  str[length] = '\0';
  lua_pushstring(L,str);
  free(str);
  return 1;
}

static int tostring(lua_State *L) {
  NumArray *a = checkarray(L, 1);
  lua_pushfstring(L,"<buffer(%d)>", a->size);
  return 1;
}

static int gc(lua_State *L) {
  NumArray *a = checkarray(L, 1);
  free(a->values);
}

static const struct luaL_reg arraylib_m [] = {
  {"new", newarray},
  {"size", getsize},
  {"resize",resize},
  {"copy",copy},
  {"sub",sub},
  {"tostring",buffer2string},
  {NULL, NULL}
};

static const struct luaL_reg arraylib_meta [] = {
  {"__index",getarray},
  {"__newindex",setarray},
  {"__len",getsize},
  {"__concat",concat},
  {"__add",concat},
  {"__tostring",tostring},
  {"__gc",gc},
  {NULL, NULL}
};

int luaopen_luvem_cbuffer (lua_State *L) {
  luaL_newmetatable(L, "luv.buffer");
  luaL_register(L, NULL, arraylib_meta);
  luaL_register(L, "luvcbuffer", arraylib_m);
  return 1;
}

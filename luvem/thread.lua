--[[

Copyright 2014 The Luvit Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS-IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

--]]
local uv = require('luv')
local Object = require('luvem.core').Object

local function start(thread_func, ...)
  local dumped = type(thread_func)=='function'
    and string.dump(thread_func) or thread_func

  local package_path = package.path
  local package_cpath = package.cpath

  local function thread_entry(dumped, package_path, package_cpath, ...)

    -- Inject the global process table
    package.path = package_path
    package.cpath = package_cpath
    _G.process = require('luvem.process').globalProcess()
    -- _G = package_path
    -- Run function with require injected
    local fn = loadstring(dumped)
    fn(...)

    -- Start new event loop for thread.
    require('luv').run()
  end
  return uv.new_thread(thread_entry, dumped, package_path, package_cpath, ...)
end

local function join(thread)
    return uv.thread_join(thread)
end

local function equals(thread1,thread2)
    return uv.thread_equals(thread1,thread2)
end

local function self()
    return uv.thread_self()
end

--- luvit threadpool
local Worker = Object:extend()

function Worker:queue(...)
    uv.queue_work(self.handler, self.dumped, self.package_path, self.package_cpath, ...)
end

local function work(thread_func, notify_entry)
  local worker = Worker:new()
  worker.dumped = type(thread_func)=='function'
    and string.dump(thread_func) or thread_func
  worker.package_path = package.path
  worker.package_cpath = package.cpath

  local function thread_entry(dumped, package_path, package_cpath, ...)
    if not _G._uv_works then
      _G._uv_works = {}
    end

    --try to find cached function entry
    local fn
    if not _G._uv_works[dumped] then
      fn = loadstring(dumped)

      -- Inject the global process table
      package.path = package_path
      package.cpath = package_cpath
      _G.process = _G.process or require('luvem.process').globalProcess()

      -- cache it
      _G._uv_works[dumped] = fn
    else
      fn = _G._uv_works[dumped]
    end
    -- Run function

    return fn(...)
  end

  worker.handler = uv.new_work(thread_entry,notify_entry)
  return worker
end

local function queue(worker, ...)
  worker:queue(...)
end

return {
  start = start,
  join = join,
  equals = equals,
  self = self,
  work = work,
  queue = queue,
}

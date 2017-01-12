local original_require = require

local function isRelativePath(path)
  return path:match("^./") == "./"
end

function require(path)
  if (isRelativePath(path)) then
    root = debug.getinfo(2)["source"]:gsub("@","")
    root_path = string.match(root,"(.-)([^\\/]-%.?([^%.\\/]*))$")
    path = root_path .. path:gsub("^./","")
  end
  return original_require(path:gsub(".lua",""):gsub("/","."))
end

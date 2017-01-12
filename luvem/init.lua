local uv = require('luv')

local function init(app)
  _G.process = require('luvem.process').globalProcess()

  -- Load Resolver
  do
    local dns = require('luvem.dns')
    dns.loadResolver()
  end

  -- EPIPE ignore
  do
    local sig = uv.new_signal()
    uv.signal_start(sig, 'sigpipe')
    uv.unref(sig)
  end

  local success, err = xpcall(function ()
		app()
    uv.run()
  end,debug.traceback)

  if success then
    -- Allow actions to run at process exit.
    require('luvem.hooks'):emit('process.exit')
    uv.run()
  else
    _G.process.exitCode = -1
    require('luvem.pretty-print').stderr:write("Uncaught exception:\n" .. err .. "\n")
  end

  uv.walk(function (handle)
    if handle and not handle:is_closing() then handle:close() end
  end)
  uv.run()

	return _G.process.exitCode
end

return init

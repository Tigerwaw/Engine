dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root .. "Temp/")
dirs["lib"]				= os.realpath(dirs.root .. "Lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "Local/")
dirs["source"] 			= os.realpath(dirs.root .. "Source/")

dirs["engine"]			= os.realpath(dirs.source .. "GameEngine/")
dirs["graphics"]			= os.realpath(dirs.source .. "Graphics/")
dirs["graphicsengine"]			= os.realpath(dirs.graphics .. "GraphicsEngine/")
dirs["graphicsengineshaders"]			= os.realpath(dirs.graphics .. "GraphicsEngineShaders/")
dirs["assetmanager"]			= os.realpath(dirs.source .. "AssetManager/")

dirs["utilities"] 	= os.realpath(dirs.source .. "Utilities/")
dirs["imgui"] 	= os.realpath(dirs.utilities .. "Imgui/")
dirs["logger"] 	= os.realpath(dirs.utilities .. "Logger/")
dirs["math"] 	= os.realpath(dirs.utilities .. "Math/")
dirs["commonutilities"] 	= os.realpath(dirs.utilities .. "CommonUtilities/")
dirs["projectcreator"] 	= os.realpath(dirs.utilities .. "ProjectCreator/")

dirs["application"]			= os.realpath(dirs.source .. "Application/")
dirs["network"]			= os.realpath(dirs.source .. "Network/")
dirs["networkengine"]			= os.realpath(dirs.network .. "NetworkEngine/")

dirs["shaders"]	= {}
dirs.shaders["absolute"] = os.realpath(dirs.root .. "Assets/EngineAssets/Shaders/")
dirs.shaders["relative"] = "../Assets/EngineAssets/Shaders/"

engine_settings = os.realpath("ApplicationSettings.json")


function default_settings(app_name)
	return {
		title = app_name,
		assetsDir = path.getrelative(dirs.bin .. "/*/", dirs.root .. "Assets/") .. "/",
		resolution = { width=1920, height=1080 },
		windowSize = { width=1920, height=1080 },
        windowPos = { top=0, left=0 },
		fullscreen = true,
		borderless = true,
		allowdropfiles = false,
		autoregisterassets = true,
	}
end

if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
end

function verify_or_create_settings(app_name)
	local settings_filename = "ApplicationSettings.json"
	defines { 'APP_SETTINGS_PATH="' .. settings_filename .. '"' }
	local app_settings = dirs["bin"] .. "/" .. app_name .."/" .. settings_filename
	
	local settings = default_settings(app_name)
	if os.isfile(app_settings) then
		local old_settings = json.decode(io.readfile(app_settings))
		for k,v in pairs(old_settings) do
			settings[k] = v
		end
	end

	io.writefile(
		app_settings,
		json.encode(settings)
	)
end

-- ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
-- ┃ Manual VSPROJ fixes         ┃
-- ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

-- Utility functions
local srep = string.rep

local lpad = function(s, l, c)
    local res = srep(c or ' ', l - #s) .. s
    return res, res ~= s
end

local rpad = function(s, l, c)
    local res = s .. srep(c or ' ', l - #s)
    return res, res ~= s
end

local printTable = function(_table)
    local sortedKeys = {}
    local maxLength = 0;

    for k in pairs(_table) do
        table.insert(sortedKeys, k)
        maxLength = math.max(maxLength, string.len(k))
    end

    table.sort(sortedKeys)

    for _, k in ipairs(sortedKeys) do
        print(rpad(k, maxLength, " "), _table[k])
    end
end

local write = function(content)
    premake.w(content)
end

local writeOpenTag = function(tag)
    write(string.format("<%s>", tag))
end

local writeCloseTag = function(tag)
    write(string.format("</%s>", tag))
end

-- Actual stuff
local hiddenFunction = function()
    require('vstudio')

    premake.override(premake.vstudio.vc2010, "propertyGroup", function(base, prj)
        base(prj)
        premake.w('<MultiProcFXC>true</MultiProcFXC>')
    end)

    premake.override(premake.vstudio.vc2010, "fxCompile", function(base, prj)
        if prj.filename == "GraphicsEngineShaders" and prj.buildcfg == "Debug" then
            writeOpenTag("FxCompile");
            write("<ShaderModel>5.0</ShaderModel>")
            write("<DisableOptimizations>true</DisableOptimizations>")
            write("<EnableDebuggingInformation>true</EnableDebuggingInformation>")
            writeCloseTag("FxCompile");
        else
            base(prj)
        end

    end)
end
hiddenFunction()
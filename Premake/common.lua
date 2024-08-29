outputdir = "%{cfg.buildcfg}"
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
dirs["projectcreator"] 	= os.realpath(dirs.utilities .. "ProjectCreator/")

dirs["application"]			= os.realpath(dirs.source .. "Application/")

dirs["shaders"]	= {}
dirs.shaders["absolute"] = os.realpath(dirs.root .. "Assets/EngineAssets/Shaders/")
dirs.shaders["relative"] = "../Assets/EngineAssets/Shaders/"

engine_settings = os.realpath("ApplicationSettings.json")


function default_settings(app_name)
	return {
		title = app_name,
		assetsDir = dirs["root"] .. "Assets/",
		resolution = { width=1920, height=1080 },
		windowSize = { width=1920, height=1080 },
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
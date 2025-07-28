include( "gmod_tests/sh_init.lua" )

local testFilePath = "garrysmod/lua/autorefresh-testfile.lua"
local f = io.open(testFilePath, "r")

if f then
    f:close()
    print("[BlaBla] TestFile already existed: " .. testFilePath)
else
    -- Neue Datei schreiben
    f = io.open(testFilePath, "w")
    if f then
        f:write([[print("[Sigma] Initial loaded")]])
        f:close()
        print("[BlaBla] File created: " .. testFilePath)
    else
        print("[BlaBla] File niet created")
    end
end

hook.Add("HolyLib:OnMapChange", "HookOnMapChangeTest", function(levelName, levelLandmark)
	local currentMap = game.GetMap()

	if not file.Exists("HookOnMapChangeData.txt", "DATA") then
		file.Write("HookOnMapChangeData.txt", currentMap .. "\n" .. levelName .. "\n" .. levelLandmark)
	end
end)

-- We change the level once and run everything again as in rare cases a crash might only ocurr after a map change.
hook.Add("GLuaTest_Finished", "ChangeLevel", function()
	if not file.Exists("waschanged.txt", "DATA") then
		file.Write("waschanged.txt", "yes")

		RunConsoleCommand("changelevel", game.GetMap())
	end
end)

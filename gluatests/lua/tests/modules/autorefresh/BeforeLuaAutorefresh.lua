return {
    groupName = "HolyLib:BeforeLuaAutorefresh",
    cases = {
        {
            name = "Hook is triggered",
            when = HolyLib_IsModuleEnabled("autorefresh"),
            func = function()
                local triggered = false
                hook.Add("HolyLib:BeforeLuaRefresh", "BeforeLuaRefreshTest", function(filePath, filename)
                    print("[BEFORE]: Triggered")
                    triggered = true
                end)

                local path = "garrysmod/lua/autorefresh-testfile.lua"
                local f = io.open(path, "w")
                if f then
                    f:write("-- Test File\n")
                    f:write("print('AutoRefresh Test File Loaded')\n")
                    f:close()
                    print("Datei geschrieben: " .. path)
                else
                    print("Konnte Datei nicht öffnen!")
                end
                
                expect(triggered).to.beTrue()
            end
        },
    }
}

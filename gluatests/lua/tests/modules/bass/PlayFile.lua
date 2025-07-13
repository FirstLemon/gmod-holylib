return {
    groupName = "bass.PlayFile",

    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( bass.PlayFile ).to.beA( "function" )
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( bass.PlayFile ).to.beA( "nil" )
            end
        },
        {
            name = "Print working directory and list files",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                print("[DEBUG] Current working directory:")

                print("[DEBUG] engine.GetGameDir():", engine.GetGameDir())

                local f = io.popen("pwd && ls -R", "r")
                if f then
                    print("[DEBUG] Full Directory Listing:\n" .. f:read("*a"))
                    f:close()
                else
                    print("[ERROR] Could not read directory structure")
                end
            end
        },
    }
}
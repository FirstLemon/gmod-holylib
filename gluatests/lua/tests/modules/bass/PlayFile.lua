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
            name = "Print current working directory and file structure",
            func = function()
                -- Use io.popen to run OS-level commands
                local f = io.popen("pwd && ls -R", "r")
                if f then
                    local output = f:read("*a")
                    f:close()

                    print("[TEST DEBUG] === BEGIN FILE LISTING ===")
                    print(output)
                    print("[TEST DEBUG] === END FILE LISTING ===")
                else
                    print("[TEST DEBUG] Failed to execute 'pwd && ls -R'")
                end
            end
        },
    }
}
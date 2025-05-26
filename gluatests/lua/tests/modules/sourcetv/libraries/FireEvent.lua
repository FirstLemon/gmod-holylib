return {
    groupName = "sourcetv:FireEvent",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.FireEvent).to.beA("function")
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(cvar.FireEvent).to.beA("nil")
            end
        },
    }
}
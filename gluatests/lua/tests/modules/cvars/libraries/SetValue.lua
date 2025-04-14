return {
    groupName = "cvars:SetValue",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect(cvar.SetValue).to.beA("function")
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect(cvar.SetValue).to.beA("nil")
            end
        },
        {
            name = "Returns an Boolean object",
            when = HolyLib_IsModuleEnabled("cvars"),
            func = function()
                expect(cvar.SetValue("ai_block_damage", "0")).to.beTrue()
            end
        },
    }
}

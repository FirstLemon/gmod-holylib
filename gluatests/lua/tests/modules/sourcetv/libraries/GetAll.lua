return {
    groupName = "sourcetv:GetAll",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetAll).to.beA("function")
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetAll).to.beA("nil")
            end
        },
        {
            name = "Returns an Table object",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetAll()).to.beA("table")
            end
        },
    }
}
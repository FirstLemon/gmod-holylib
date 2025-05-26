return {
    groupName = "sourcetv:GetClient",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetClient).to.beA("function")
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv).to.beA("nil")
            end
        },
        {
            name = "Is CHLTVClient",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetClient(sourcetv.GetHLTVSlot())).to.beA("CHLTVClient")
            end
        },
        {
            name = "Is not CHLTVClient",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetClient(-1).to.beNil())
            end
        },
    }
}
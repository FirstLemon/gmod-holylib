return {
    groupName = "sourcetv:GetAll",
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
            name = "Returns an table object",
            when = HolyLib_IsModuleEnabled("sourcetv"),
            func = function()
                expect(sourcetv.GetClient(sourcetv.GetHLTVSlot())).to.beA("CHLTVClient")
                expect(sourcetv.GetClient(-1).to.beNil())
            end
        },
    }
}
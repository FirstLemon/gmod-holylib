return {
    groupName = "bass.PlayFile",
    cases = {
        {
            name = "Function exists globally",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( PlayFile ).to.beA( "function" )
            end
        },
        {
            name = "Function doesn't exists globally",
            when = not HolyLib_IsModuleEnabled("bass"),
            func = function()
                expect( PlayFile ).to.beA( "nil" )
            end
        },
        {
            name = "Playing Sample File",
            when = HolyLib_IsModuleEnabled("bass"),
            func = function()
                bass.PlayFile("../../../assets/audio/bass-test.wav", "3d", function(channel, errorCode, error)
                    expect(channel).toNot.beNil
                    print("ErrorCode: " .. errorCode .. "\n")
                    print("Error: ".. error .. "\n")
                end)
            end
        },
    }
}
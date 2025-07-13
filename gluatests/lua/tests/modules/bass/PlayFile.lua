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
            name = "Playing Sample Wave File",
            when HolyLib_IsModuleEnabled("bass"),
            async = true,
            timeout = 15,
            func = function()
                local filePath = "assets/bass-test.wav"
                local flags = "mono"

                bass.PlayFile(filePath, flags, function(channel, errorCode, errorMsg)
                    expect(channel).toNot.beNil()
                    print("ErrCode: " .. errorCode .. "\n")
                    print("ErrMsg: " .. errorMsg .. "\n")
                end)
            end
        },
    }
}
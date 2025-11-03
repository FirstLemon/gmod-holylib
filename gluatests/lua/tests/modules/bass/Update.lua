return {
    groupName = "bass.Update()",

    cases = {
        {
            name = "Function exists when module enabled",
            when = HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.Update ).to.beA( "function" )
            end
        },
        {
            name = "Function is nil when module disabled",
            when = not HolyLib_IsModuleEnabled( "bass" ),
            func = function()
                expect( bass.Update ).to.errWith("attempt to index global 'bass' (a nil value)")
            end
        },
        {
            name = "",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = ""
        
                bass.PlayFile( filePath, flags, function( channel, errorCode, errorMsg )
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()

                    expect( channel:GetState() ).to.equal( 1 )
                    
                    done()
                end )
            end
        },
    }
}
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
            name = "Calling bass.Update() without any channel should not error",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                bass.Update( 1 )
            end
        },
        {
            name = "Calling bass.Update() does not error on one valid and active channel",
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

                    bass.Update()
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()
                    expect( channel:GetState() ).to.equal( 1 )
                    
                    done()
                end )
            end
        },
        {
            name = "Calling bass.Update() does not error on one valid and active channel",
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

                    bass.Update()
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
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

                done()
            end
        },
        {
            name = "bass.Update() returns a boolean",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                local ret = bass.Update()
                expect( ret ).to.beA( "boolean" )

                done()
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

                    local success = bass.Update()
                    expect( success ).to.beTrue()
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()
                    expect( channel:GetState() ).to.equal( 1 )
                    
                    done()
                end )
            end
        },
        {
            name = "Calling bass.Update() on an stopped channel is being handled correctly",
            when = HolyLib_IsModuleEnabled( "bass" ),
            async = true,
            timeout = 2,
            func = function()
                local filePath = "sound/bass_testsound.wav"
                local flags = "noplay"
        
                bass.PlayFile( filePath, flags, function( channel, errorCode, errorMsg )
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()
                    expect( channel:GetState() ).to.equal( 0 )

                    local success = bass.Update()
                    expect( success ).to.beTrue()
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()
                    expect( channel:GetState() ).to.equal( 0 )
                    
                    done()
                end )
            end
        },
        {
            name = "Calling bass.Update() on an paused channel is being handled correctly",
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

                    channel:Pause()
                    expect( channel:GetState() ).to.beBetween( 2, 3 ) -- checking for BASS_ACTIVE_PAUSED (2) or BASS_ACTIVE_PAUSED_DEVICE (3)

                    local success = bass.Update()
                    expect( success ).to.beTrue()
                    expect( channel ).toNot.beNil()
                    expect( errorCode ).to.equal( 0 )
                    expect( errorMsg ).to.beNil()
                    expect( channel:GetState() ).to.beBetween( 2, 3 ) -- checking for BASS_ACTIVE_PAUSED (2) or BASS_ACTIVE_PAUSED_DEVICE (3)
                    
                    done()
                end )
            end
        },
    }
}
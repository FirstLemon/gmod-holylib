--[[
	tbl = {
		failed = function(reason) end,
		success = function(body) end,
		method = "GET POST HEAD PUT DELETE PATCH OPTIONS",
		url = "www.google.com",
		body = "",
		type = "text/plain; charset=utf-8",
		timeout = 60,
		headers = {
			["APIKey"] = "SaliBonani" 
		}
	}

	ToDo: Make this far better
]]

CreateDir("http")

local last_added_request = os.time()
local requests = {}
function HTTP_WaitForAll()
	local i = 0
	print("Called!")
	for key, tbl in pairs(requests) do
		i = i + 1

		local httpcontent = ReadFile(tbl.httpfile)
		print(requests, httpcontent)
		if httpcontent and not (httpcontent == "") then
		    local success = true --tbl.handle:close()
		    if success then
		        if tbl.success then
		        	local status, err = pcall(function()
		           		tbl.success(httpcontent)
		           	end)
		        end
		    else
		        if tbl.failed then
		            tbl.failed("Request failed: " .. success)
		        end
		    end

			table.remove(requests, key)
		end
	end

	if (os.time() - last_added_request) > 60 then
		error("HTTP Took way too long. Assuming something broke!")
	end

	return i > 0
end

--function HTTP_WaitForAll()
	--while HTTP_WaitForAllInternal() do end
--end

local i = 0
function HTTP(tbl)
	i = i + 1

    local method = tbl.method or "GET"
    local url = tbl.url or ""
    local body = tbl.body or ""
    local contentType = tbl.type or ""
    local timeout = tbl.timeout or 60
    local headers = ""
    local params = ""
    if tbl.params then
    	if type(tbl.params) == "string" then
    		params = " --data-urlencode \"" .. tbl.params .. "\""
    	else
    		for _, param in ipairs(tbl.params) do
    			params = params .. " --data-urlencode \"" .. param .. "\""
    		end
    	end
    end

    if tbl.headers then
        for key, value in pairs(tbl.headers) do
            headers = headers .. " -H \"" .. key .. ":" .. value .. "\""
        end
    end
    tbl.httpfile = "http/" .. i .. ".txt"

    local curlCommand = "curl -sb -X " .. method .. " " .. url .. params .. (not (contentType == "") and (" -H \"Content-Type:".. contentType .. "\"") or "") .. headers .. (body == "" and "" or ("--data-raw \"" .. body .. "\"")) .. " --max-time " .. timeout .. " > " .. tbl.httpfile
   	--local handle = io.popen(curlCommand)
   	--tbl.handle = handle
   	os.execute(curlCommand)

   	HTTP_WaitForAll()

   	table.insert(requests, tbl)
   	last_added_request = os.time()
end

function HTTPDownload(tbl)
	i = i + 1

    local url = tbl.url or ""
    tbl.httpfile = tbl.file or "http/" .. i .. ".txt"

    local curlCommand = "curl -L " .. url .. " -s -o '" .. tbl.httpfile .. "'"
   	local handle = io.popen(curlCommand)
   	tbl.handle = handle

   	table.insert(requests, tbl)
end

function JSONHTTP(tbl)
	local func = tbl.success
	tbl.success = function(body)
		local json = json.decode(body)
		func(json)
	end
	tbl.headers = tbl.headers or {}
	tbl.headers["Accept"] = "application/json"

	HTTP(tbl)
end
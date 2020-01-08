function string.split(str, delimiter)
	if str==nil or str=='' or delimiter==nil then
		return nil
	end
	
    local result = {}
    for match in (str..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match)
    end
    return result
end

function stripextension(filename)  
    local idx = filename:match(".+()%.%w+$")  
    if(idx) then  
        return filename:sub(1, idx-1)  
    else  
        return filename  
    end  
end



function main()
	--遍历所有文件
	script_manager:LoadFilesInDir("monster", false);
	script_manager:LoadFilesInDir("map", false);
	script_manager:LoadFilesInDir("npc", false);
	script_manager:LoadFilesInDir("task", false);
	script_manager:LoadFilesInDir("item", false);
	script_manager:LoadFilesInDir("skill", false);
	script_manager:LoadFilesInDir("status", false);
	script_manager:LoadFilesInDir("system", false);
end


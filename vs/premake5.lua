--
-- premake5 file to build RecastDemo
-- http://premake.github.io/
--
require "lib"

local action = _ACTION or ""
local outdir = action


WorkSpaceInit  "prj"

Project "prj"

	SrcPath { 
		"../bin/**.txt",
		"../bin/**.sh",
	}
	files {
	"../*.sh",
	"../*.txt",
	"../vs/premake5.lua",
	"../bin/**.txt",
	"../bin/**.sh",
	"../tool/**.sh",
	"../samples/**.txt",
	"../test/**.txt",
	"../test/**.sh",
	}
	
Project "src"
	includedirs { 
		"../external/hiredis-0.14.0/",
	}

	SrcPath { 
		"../src/**",  --**递归所有子目录，指定目录可用 "cc/*.cpp" 或者  "cc/**.cpp"
		"../include/**",
	}
	
	
Project "test"
	includedirs { 
		"../include/",
		"../samples/external/",
	}

	SrcPath { 
		"../test/**",  
	}
	
Project "samples"
	includedirs { 
		"../include/",
		"../samples/external/",
	}

	SrcPath { 
		"../samples/base/**",  
		"../samples/client_prj/**",  
	}
	
	
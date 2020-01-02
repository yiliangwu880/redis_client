--
-- premake5 file to build RecastDemo
-- http://premake.github.io/
--
require "lib"

local action = _ACTION or ""
local outdir = action


WorkSpaceInit  "hiredis"

Project "hiredis"
	files {
	"../**.sh",
	"../**.txt",
	"../**.md",
	"../**Makefile",
	"../vs/premake5.lua",
	}
	
local COM_INCLUDE={ 
		"../adapters",
	}

Project("c")
	IncludeFile(COM_INCLUDE)

	SrcPath { 
		"../**",
	}







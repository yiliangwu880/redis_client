简介：
	开源hiredis基础上封装成C++接口。简单易用安全。
	
编译：
	整个文件夹放到linux目录，安装cmake gcc git等。

	去目录 external/hiredis-0.14.0/执行make生成libhiredis.a
	去目录 redis_client\samples\external\libevent_cpp\ 编译测试网络库	
	主目录执行：sh clearBuild.sh 完成编译	
			 sh combine_lib.sh 生成redis_client库
			 再执行一次 clearBuild.sh
	

目录结构：





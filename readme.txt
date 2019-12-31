简介：
	演示用游戏服务器架构，验证RS里面所有组件
	
编译：
	整个文件夹放到linux目录，安装cmake gcc git等。

	git submodule init      --更新子模块
	git submodule update			
	git checkout -f	master		--强制删掉本地分支， track远程master分支
	去目录 external里面，参考说明编译每个文件夹，生成依赖库。
	主目录执行：sh clearBuild.sh 完成编译	
	
详细说明参考：说明.docx

目录结构：
	这里目录结构不好，有重复的libevent_cpp, svr_util,应该设置层平衡目录结构，避免重复导致不一致。
	但为了方便不断更新子模块，和子模块的完整性，先保持这样结构吧。 正式项目不要参考这种目录结构。
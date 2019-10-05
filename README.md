# xdbg

#### 项目介绍
WinDbg 扩展插件，支持在 WinDbg 命令行中调用 Python 脚本，并在 Python 脚本中调用 WindDbg 提供的接口。

#### 编译说明
1. 依赖 boost.python 库，需下载编译 boost.python 并在工程中设置包含路径
2. 依赖 python 3.7（理论上 3.x 版本都可以），需下载安装 python 开发环境，并在工程中设置包含路径
3. IDE 为 Visual Studio 2017
4. 编译输出的目标文件为 xdbg.pyd
5. 仅支持编译为 x64 版本 

#### 使用说明

1. 拷贝目标文件 xdbg.pyd 到 WindDbg 安装路径的 winext 文件夹下 
2. 在 WindDbg 输入命令 .load xdbg.pyd 载入本插件
3. 输入命令 !help 查看帮助
4. 输入命令 !py hello.py 执行 Python 脚本（需提前设置 PATH 环境变量或者输入脚本全路径）
5. 输入命令 .unload xdbg.pyd 卸载插件

#### 参考项目 
1. PyDbgExt (https://sourceforge.net/projects/pydbgext/)
2. Pkyd (https://githomelab.ru/pykd)
3. kdlibcpp (https://githomelab.ru/kdlibcpp/kdlibcpp) 

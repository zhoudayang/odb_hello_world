## ODB MySQL - Windows 配置 ##
> **参考资料**
> 1. [ODB - C++访问数据库的利器](http://blog.csdn.net/calmreason/article/details/49492151)
> 2. [ODB学习笔记之基础环境搭建](http://blog.csdn.net/feng______/article/details/33411119)

### 文件夹中内容 ###
* include ODB 包含路径
* mysql MySQL 包含路径
* lib 链接库和动态链接库路径

### 使用详解 ###
1. 首先将odb.exe加入环境变量
2. 然后将include 和 mysql 文件夹加入包含路径，将 lib文件夹加入链接路径
3. 生成的可执行文件所在路径中需要有lib文件夹中的dll文件。或者将上述dll文件复制到  *C:\Windows\system* 路径
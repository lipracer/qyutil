## 项目介绍

网络诊断工具，功能：ping、dnsquery、tracetouter、http

### 项目结构
```
|-- build 工程构建脚本，android目录下包括java接口封装文件（注意全类名的路径）
|-- common
|--- |
|--- | - net_common 网络相关数据结构
|--- | - qylog 简单跨平台同步log实现，可以输出到console，file
|-- include 项目头文件目录，ios端可以将global_func.h中c接口直接使用
|-- src
|--- |
|--- | - dnsquery dns解析
|--- | - http http请求实现
|--- | - jni jni层接口封装，与安卓编译make文件
|--- | - ping ping功能实现，ios端使用socket、android端调用shell
|--- | - tracerouter
|-- Test Mac上功能测试
```
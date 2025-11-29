# LibIProj

## 项目概述

`LibIProj` 是一个用于读取与解析 INIWeaver 项目文件（`.iproj`）的轻量级 C++ 库。该库将二进制剪贴板/项目数据解析为易于访问的结构和 API，便于在其它应用中查询项目信息、模块（module）及其键值对等。

该仓库包含：
- 字节流读取实现：`ByteStream.h` / `ByteStream.cpp`
- 对象定义：`WeaverObj.h`
- 对外 API：`LibIProj.h` / `LibIProj.cpp`
- 一个参考单元测试程序：`Test/Main.cpp`（示例如何加载 `.iproj` 并把内容输出到控制台）

## 主要功能与设计要点

- 通过 `ByteInputStream` 从字节缓冲区按类型连续解析数据，提供对各种整数、浮点、字符串、向量与自定义结构的逐项读取。
- `INIWeaverProject` 封装了加载逻辑，提供便捷的访问器获取创建时间、视角信息、模块列表等。
- `INIWeaverModule` 提供对单个模块数据的只读访问和快速查找表（lines、vars、default link keys）。

## 文件说明

- `ByteStream.h/cpp`：实现 `ByteInputStream`，以及大量 `operator>>` 重载，用于按二进制格式解析基础类型和自定义结构。
- `WeaverObj.h`：定义数据结构（`IBS_Project`, `ModuleClipData`, `IniToken` 等），以及版本判断工具函数。
- `LibIProj.h/cpp`：提供库的高层 API：`INIWeaverProject` 和 `INIWeaverModule`，并实现从字节数组或文件缓冲区加载项目的逻辑。
- `Test/Main.cpp`：演示如何构建并运行程序来加载 `example.iproj`，以及如何遍历并打印解析结果。

## 使用

包含 `LibIProj.h` ，并在项目当中加入 `LibIProj.cpp` `ByteStream.cpp`。

之后通过 `INIWeaverProject::Set` 从字节流载入工程文件。

载入之后可以访问其他方法。

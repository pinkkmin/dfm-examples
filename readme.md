## Intro

> UOS20-1050-PRO
> dde-file-manager 5.6.12
> Qt5

UOS系统文件管理器dde-file-manager右键菜单、扩展角标的示例工程。

## Files

```bash
dfm_usr # libdde-file-manager-dev 和 libdfm-extension-dev的include头文件
ext-menu-example #基于dfm-extension实现的右键菜单
ext-emblem-example #基于dfm-extension实现的扩展角标
dfm-emblem-example-plugin #基于generic接口的扩展角标
out #编译输出的.so文件
dfm-examples.pro #示例工程Qt文件 
```

## Usage

基于dfm-extension生成的.so需放置于 /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/extensions/

```bash
mkdir -p /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/extensions/
cp ./out/libext-menu-example.so /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/extensions/
cp libext-emblem-demo.so /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/extensions/
```

基于generic接口的扩展角标.so需放置于 /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/generics/

```bash
mkdir -p /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/generics/
cp libemblem-example-plugin.so /usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/generics/
```

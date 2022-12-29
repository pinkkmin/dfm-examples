> :thought_balloon:此文档为请教dde-file-manager开发人员(zhangsheng@uniontech.com)的回复。
> 
> :warning: 此文档为初稿文档，请优先参考官方正式文档。
> 
> :date: 2022年9月5日10:36:36

# 文件管理器扩展插件开发接口说明

UOS20 的文件管理器（以下简称文管，dde-file-manager）提供一套插件开发接口来支持**右键菜单**和 **文件角标**两个需求的开发接口。

[TOC]

## 插件的工作原理

插件是一种在不需要改动并重新编译主程序本身的情况下去扩展主程序功能的一种机制。

文件管理器的插件机制是由文件管理器提供一个扩展库 `libdfm-extension.so` 来提供接口，这是一个纯 C/C++ 接口的库。第三方开发者通过加载该扩展库，按照文管提供的标准来调用和实现相应接口，共同完成定制需求的功能扩展。

### dde-file-manager 第三方扩展插件加载流程

在文件管理器启动时，会去检测目录 `/usr/lib/[arch]/dde-file-manager/plugins/extensions` 下的所有文件（其中 [arch] 代表平台架构，如 ARM64 下为 aarch64-linux-gnu，AMD 64下为 x86_64-linux-gnu）， 并检测是否是一个正常的动态库文件，如果是则尝试加载，因此第三方开发者应该将扩展插件**安装到此目录下**。如果加载成功，文管将会检查扩展插件是否实现了相应的**元接口**，如果通过了检查，文管将与第三方扩展插件开始正常业务交互。

### 接口列表

这里列出文管提供的所有接口，其中元接口是 C 风格的接口，右键菜单和角标展示是通过 C++ 类接口进行实现的。

#### 1. 元接口

接口定义在头文件 `dfm-extension.h` 中，第三方开发者需要自行创建 C++ 源码文件**实现**接口，当前元接口定义如下：

```C++
extern "C" void dfm_extension_initialize();
extern "C" void dfm_extension_shutdown();
extern "C" DFMExtMenuPlugin       *dfm_extension_menu();
extern "C" DFMExtEmblemIconPlugin *dfm_extension_emblem();
```

| 名称                       | 简介                           |
|:------------------------ |:---------------------------- |
| dfm_extension_initialize | 插件初始化入口函数，插件被加载后将首先调用它       |
| dfm_extension_shutdown   | 插件释放的函数，当插件被卸载前将会调用它         |
| dfm_extesion_menu        | 返回右键菜单扩展对象的函数，该对象的类由第三方开发者实现 |
| dfm_extesion_emblem      | 返回角标对象的函数，该对象的类由第三方开发者实现     |

#### 2. 类接口

由于此扩展方式提供了共享库 `libdfm-extension.so`，因此为了避免 ABI 兼容性的问题，以下 C++ 类接口采用了 `pimpl` 技法隐藏类属性，采用 `std::function` ＋ `std::bind` 来替代虚函数（伪虚函数）。

> 【伪虚函数】
> 
> 与普通的 C++ 接口类不同的是，普通的 C++ 接口类通常在基类使用虚函数定义可重写的接口，派生类继承进行实现。而在 `dfm-extension` 中的，使用了宏来“标记“接口是一个可重写的接口（伪虚函数），定义在一个全局的头文件头文件 `dfm-extension-global.h` 中：
> 
> ```c++
> // 仅用于标记，没有具体意义
> #define DFM_FAKE_VIRTUAL
> #define DFM_FAKE_OVERRIDE
> ```
> 
> 宏 `DFM_FAKE_VIRTUAL` （伪虚函数）用来标记它是第三方开发者应该去注册的接口函数，开发者也可以在派生类用 `DFM_FAKE_OVERRIDE` 来标记将要 bind 的方法。

##### 角标

###### DFMExtEmblemIconPlugin

`DFMExtEmblemIconPlugin` 类是第三方开发者应该继承并实现的接口类，并且在元接口 `dfm_extesion_emblem` 中返回派生的类对象，接口定义在头文件 `dfmextemblemiconplugin.h` 中。需要特别注意的是，需要提前将角标资源图片文件安装到系统主题中，规则遵循 freedesktop 规范。

```c++
class DFMExtEmblemIconPluginPrivate;
class DFMExtEmblemIconPlugin
{
    DFM_DISABLE_COPY(DFMExtEmblemIconPlugin)
public:
    using IconsType = std::vector<std::string>;
    using EmblemIcons = std::function<IconsType(const std::string &)>;
    using LocationEmblemIcons = std::function<DFMExtEmblem(const std::string &, int)>;

public:
    DFMExtEmblemIconPlugin();
    ~DFMExtEmblemIconPlugin();

    // Note: If the corner mark set by emblemIcons conflicts with the corner mark position set by locationEmblemIcons,
    // the conflict position will only display the corner mark set by locationEmblemIcons
    DFM_FAKE_VIRTUAL IconsType emblemIcons(const std::string &fileUrl) const;
    void registerEmblemIcons(const EmblemIcons &func);

    DFM_FAKE_VIRTUAL DFMExtEmblem locationEmblemIcons(const std::string &fileUrl, int systemIconCount) const;
    void registerLocationEmblemIcons(const LocationEmblemIcons &func);

private:
    DFMExtEmblemIconPluginPrivate *d { nullptr };
};
```

接口说明：

emblemIcons 和 registerEmblemIcons 是旧接口，locationEmblemIcons 和 registerLocationEmblemIcons 可以代替

| 名称                          | 简介                                                 |
|:--------------------------- |:-------------------------------------------------- |
| emblemIcons                 | 文管主动调用，传入文件路径，返回被安装的角标图片名称列表                       |
| registerEmblemIcons         | emblemIcons 接口的注册函数，第三方开发者主动注册                     |
| locationEmblemIcons         | 文管主动调用，传入文件路径和这个文件的现有系统角标数量，如果这个值达到了4，那么就位置绘制扩展角标了 |
| registerLocationEmblemIcons | locationEmblemIcons 接口的注册函数，第三方开发者主动注册             |

###### DFMExtEmblemIconLayouth

扩展角标布局信息，描述一个角标位置和角标图片路径

```c++
class DFMExtEmblemIconLayoutPrivate;
class DFMExtEmblemIconLayout
{
    friend class DFMExtEmblemIconLayoutPrivate;

public:
    enum class LocationType : uint8_t
    {
        BottomRight = 0,
        BottomLeft,
        TopLeft,
        TopRight,
        Custom = 0xff
    };

    explicit DFMExtEmblemIconLayout(LocationType type,
                           const std::string &path,
                           int x = 0, int y = 0);
    ~DFMExtEmblemIconLayout();
    DFMExtEmblemIconLayout(const DFMExtEmblemIconLayout &emblem);
    DFMExtEmblemIconLayout &operator=(const DFMExtEmblemIconLayout &emblem);
    LocationType locationType() const;
    std::string iconPath() const;
    int x() const;
    int y() const;

private:
    DFMExtEmblemIconLayoutPrivate *d { nullptr };
};
```

接口说明：

| 名称           | 简介                                                        |
|:------------ |:--------------------------------------------------------- |
| locationType | 需要第三方开发者返回角标的位置信息，目前有 `LocationType` 的 4 个位置，custom 暂时未使用 |
| iconPath     | 需要第三方开发者返回返回角标图片路径的字符串                                    |
| x            | 预留接口，暂时未实现                                                |
| y            | 预留接口，暂时未实现                                                |

###### DFMExtEmblem

角标操作对象类，管理一个文件扩展角标的所有布局信息。

```C++
class DFMExtEmblemPrivate;
class DFMExtEmblem
{
    friend class DFMExtEmblemPrivate;
public:
    explicit DFMExtEmblem();
    ~DFMExtEmblem();
    DFMExtEmblem(const DFMExtEmblem &emblem);
    DFMExtEmblem &operator=(const DFMExtEmblem &emblem);
    void setEmblem(const std::vector<DFMExtEmblemIconLayout> &iconPaths);
    std::vector<DFMExtEmblemIconLayout> emblems() const;

private:
    DFMExtEmblemPrivate *d { nullptr };
};
```

接口说明：

| 名称        | 简介                                  |
|:--------- |:----------------------------------- |
| setEmblem | 需要第三方开发者自行设置一个角标 Layout 信息，最多支持 4 个 |
| emblems   | 返回 setEmblem 设置的 Layout 信息          |

##### 右键菜单

###### DFMExtMenuPlugin

`DFMExtMenuPlugin` 是第三方开发者应该继承并实现的接口类，并且在元接口 `dfm_extesion_menu` 中返回派生的类对象，接口定义在头文件 `dfmextmenuplugin.h` 中：

```c++
class DFMExtMenuPlugin
{
    DFM_DISABLE_COPY(DFMExtMenuPlugin)
public:
    using InitializeFunc = std::function<void (DFMEXT::DFMExtMenuProxy *proxy)>;
    using BuildNormalMenuFunc = std::function<bool (DFMExtMenu *, 
                                const std::list<std::string> &,
                                const std::string &, bool)>;
    using BuildEmptyAreaMenuFunc = std::function<bool (DFMEXT::DFMExtMenu *, const std::string &, bool)>;

public:
    DFMExtMenuPlugin();
    virtual ~DFMExtMenuPlugin();

    DFM_FAKE_VIRTUAL void initialize(DFMEXT::DFMExtMenuProxy *proxy);
    DFM_FAKE_VIRTUAL bool buildNormalMenu(DFMEXT::DFMExtMenu *main,
                                          const std::string &currentUrl,
                                          const std::string &focusUrl,
                                          const std::list<std::string> &urlList,
                                          bool onDesktop);
    DFM_FAKE_VIRTUAL bool buildEmptyAreaMenu(DFMExtMenu *main, const std::string &currentUrl, bool onDesktop);

public:
    void registerInitialize(const InitializeFunc &func);
    void registerBuildNormalMenu(const BuildNormalMenuFunc &func);
    void registerBuildEmptyAreaMenu(const BuildEmptyAreaMenuFunc &func);

private:
    DFMExtMenuPluginPrivate *d;
};
```

接口说明：

| 名称                         | 简介                                                                                                                                                      |
|:-------------------------- |:------------------------------------------------------------------------------------------------------------------------------------------------------- |
| initialize                 | 文管主动调用，在其他所有接口被文管调用前调用，文管将 proxy 传入给扩展插件，参数 proxy 可认为是文管主程序的进程                                                                                          |
| buildNormalMenu            | 文管主动调用，右键菜单在文件上触发时调用。main 代表当前的右键菜单，urlList 代表选中的所有文件列表的文件路径/URL，currentUrl 代表当前右键点击的文件路径所在目录的URL， focusUrl代表当前右键选中文件的url， onDesktop 为 true 代表操作是在桌面触发的 |
| buildEmptyAreaMenu         | 文管主动调用，右键菜单在空白区域触发时调用。                                                                                                                                  |
| registerInitialize         | initialize 接口的注册函数，第三方开发者主动注册                                                                                                                           |
| registerBuildNormalMenu    | buildNormalMenu 接口的注册函数，第三方开发者主动注册                                                                                                                      |
| registerBuildEmptyAreaMenu | buildEmptyAreaMenu 接口的注册函数，第三方开发者主动注册                                                                                                                   |

###### DFMExtMenuProxy

接口定义在头文件 `dfmextmenuproxy.h` 中，由于上面的接口对于插件来说都是被动的，即插件本身无法确定这些接口什么时刻会被调用，很明显这对于插件机制来说是不完整的，因此便有了 `DFMExtMenuProxy`，它定义了一些让插件主动调用以控制文管右键菜单的一些行为的接口。`DFMExtMenuProxy` 的具体实例可以认为是抽象了的文管主程序的右键菜单，这个实例将会通过 `DFMExtMenuPlugin` 中的 `initialize` 接口传递给插件，因此在上述 `initialize` 接口中总是会先把这个传入的对象保存起来以供后续使用。

```c++
class DFMExtMenuProxy
{
public:
    explicit DFMExtMenuProxy(DFMExtMenuProxyPrivate *d_ptr);
    virtual ~DFMExtMenuProxy();

    DFMExtMenu *createMenu();
    bool deleteMenu(DFMExtMenu *menu);

    DFMExtAction *createAction();
    bool deleteAction(DFMExtAction *action);

private:
    DFMExtMenuProxyPrivate *d;
};
```

接口说明：

| 名称           | 简介                                                                                                                                                            |
|:------------ |:------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| createMenu   | 在堆上创建一个自定义的菜单对象，通常在 DFMExtMenuPlugin 的 buildNormalMenu 或 buildEmptyAreaMenu 接口中去调用                                                                            |
| deleteMenu   | 释放创建的自定义的菜单对象，如果不调用将会造成内存泄露！需要在DFMExtMenuPlugin 的 buildNormalMenu 或 buildEmptyAreaMenu 接口传入的参数 main  中去调用 DFMExtMenu 的 registerDeleted 注册释放函数来调用本接口释放内存       |
| createAction | 在堆上创建一个自定义的 action 对象，通常在 DFMExtMenuPlugin 的 buildNormalMenu 或 buildEmptyAreaMenu 接口中去调用                                                                      |
| deleteAction | 释放创建的自定义的 action 对象，如果不调用将会造成内存泄露！需要在DFMExtMenuPlugin 的 buildNormalMenu 或 buildEmptyAreaMenu 接口传入的参数 main  中去调用 DFMExtMenu 的 registerDeleted 注册释放函数来调用本接口释放内存 |

###### DFMExtMenu

接口定义在头文件 `dfmextmenu.h` 中，其接口设计参考 `QMenu`。

```c++
class DFMExtMenu
{
    friend class DFMExtMenuPrivate;
public:
    using TriggeredFunc = std::function<void (DFMExtAction *action)>;
    using HoveredFunc = std::function<void (DFMExtAction *action)>;
    using DeletedFunc = std::function<void (DFMExtMenu *)>;

public:
    std::string title() const;
    void setTitle(const std::string &title);

    std::string icon() const;
    void setIcon(const std::string &iconName);

    bool addAction(DFMExtAction *action);
    bool insertAction(DFMExtAction *before, DFMExtAction *action);

    DFMExtAction *menuAction() const;
    std::list<DFMExtAction *> actions() const;

    DFM_FAKE_VIRTUAL void triggered(DFMExtAction *action);
    DFM_FAKE_VIRTUAL void hovered(DFMExtAction *action);
    DFM_FAKE_VIRTUAL void deleted(DFMExtMenu *self);

public:
    void registerTriggered(const TriggeredFunc &func);
    void registerHovered(const HoveredFunc &func);
    void registerDeleted(const DeletedFunc &func);

protected:
    explicit DFMExtMenu(DFMExtMenuPrivate *d_ptr);
    virtual ~DFMExtMenu();
    DFMExtMenuPrivate *d;
};
```

接口说明：

| 名称                | 简介                                                                                                                                                          |
|:----------------- |:----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| title             | 参考 QMenu 相关接口                                                                                                                                               |
| setTitle          | 参考 QMenu 相关接口                                                                                                                                               |
| icon              | 参考 QMenu 相关接口                                                                                                                                               |
| setIcon           | 参考 QMenu 相关接口，参数应该传具体的 icon 文件路径或主题图标名                                                                                                                      |
| addAction         | 参考 QMenu 相关接口                                                                                                                                               |
| insertAction      | 参考 QMenu 相关接口                                                                                                                                               |
| menuAction        | 参考 QMenu 相关接口                                                                                                                                               |
| actions           | 参考 QMenu 相关接口                                                                                                                                               |
| triggered         | 参考 QMenu 相关接口                                                                                                                                               |
| hovered           | 参考 QMenu 相关接口                                                                                                                                               |
| **deleted**       | **特别重要**，在 menu 对象被析构前调用，用于释放第三方开发者创建的自定义 Menu 的内存                                                                                                          |
| registerTriggered | triggered 接口的注册函数                                                                                                                                           |
| registerHovered   | registerHovered 接口的注册函数                                                                                                                                     |
| registerDeleted   | registerDeleted 接口的注册函数，第三方开发者必须要在DFMExtMenuPlugin 的 buildNormalMenu 或 buildEmptyAreaMenu 接口传入的参数 main  中去调用 DFMExtMenu 的 registerDeleted 注册释放函数来调用本接口释放内存！ |

###### DFMExtAction

接口定义在头文件 `dfmextaction.h` 中，其接口设计参考 `QAction`。

```C++
class DFMExtAction
{
    friend class DFMExtActionPrivate;
public:
    using TriggeredFunc = std::function<void (DFMExtAction *, bool)>;
    using HoveredFunc = std::function<void (DFMExtAction *)>;
    using DeletedFunc = std::function<void (DFMExtAction *)>;

public:
    void setIcon(const std::string &icon);
    std::string icon() const;

    void setText(const std::string &text);
    std::string text() const;

    void setToolTip(const std::string &tip);
    std::string toolTip() const;

    void setMenu(DFMExtMenu *menu);
    DFMExtMenu *menu() const;

    void setSeparator(bool b);
    bool isSeparator() const;

    void setCheckable(bool b);
    bool isCheckable() const;

    void setChecked(bool b);
    bool isChecked() const;

    void setEnabled(bool b);
    bool isEnabled() const;

    DFM_FAKE_VIRTUAL void triggered(DFMExtAction *self, bool checked = false);
    DFM_FAKE_VIRTUAL void hovered(DFMExtAction *self);
    DFM_FAKE_VIRTUAL void deleted(DFMExtAction *self);

public:
    void registerTriggered(const TriggeredFunc &func);
    void registerHovered(const HoveredFunc &func);
    void registerDeleted(const DeletedFunc &func);

protected:
    explicit DFMExtAction(DFMExtActionPrivate *d_ptr);
    virtual ~DFMExtAction();
    DFMExtActionPrivate *d;
};
```

接口说明：

| 名称                | 简介                                       |
|:----------------- |:---------------------------------------- |
| setIcon           | 参考 QAction 相关接口，参数应该传具体的 icon 文件路径或主题图标名 |
| icon              | 参考 QAction 相关接口                          |
| setText           | 参考 QAction 相关接口                          |
| text              | 参考 QAction 相关接口                          |
| setToolTip        | 参考 QAction 相关接口                          |
| toolTip           | 参考 QAction 相关接口                          |
| setMenu           | 参考 QAction 相关接口                          |
| menu              | 参考 QAction 相关接口                          |
| setSeparator      | 参考 QAction 相关接口                          |
| isSeparator       | 参考 QAction 相关接口                          |
| isCheckable       | 参考 QAction 相关接口                          |
| setChecked        | 参考 QAction 相关接口                          |
| isChecked         | 参考 QAction 相关接口                          |
| setEnabled        | 参考 QAction 相关接口                          |
| isEnabled         | 参考 QAction 相关接口                          |
| triggered         | 参考 QAction 相关接口                          |
| hovered           | 参考 QAction 相关接口                          |
| deleted           | **非常重要**，其意义同 DFMExtMenu 的 deleted 方法    |
| registerTriggered | triggered 接口的注册函数                        |
| registerHovered   | hovered 接口的注册函数                          |
| registerDeleted   | 其意义同 DFMExtMenu 的 registerDeleted 方法     |

## dde-file-manager 插件扩展示例

接下来将介绍一个简单的文管插件的开发过程，插件开发者可跟随此步骤熟悉为文管开发扩展插件的步骤。

**NOTE：以下内容可能有点过时，仅做参考**

### 预期功能

实现对右键菜单的扩展

### 安装依赖

在 UOS 中安装以下包（暂定）：

- libdde-file-manager-dev
- cmake
- pkg-config
- 本扩展库 `libdfm-extension` 的包

### cmake 配置

```cmake
cmake_minimum_required(VERSION 2.8)

# 设置项目名称
project(dfmextension-demo)

# 启用 pkg-config 标准
find_package(PkgConfig REQUIRED)

# 只需要依赖 dfm-extension
pkg_search_module(dfm-extension
    REQUIRED
    dfm-extension
    IMPORTED_TARGET
)

# 扩展插件源码
set(SRCS
    demomenuplugin.h
    demomenuplugin.cpp
    )

# 生成共享库
add_library(${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(
    ${PROJECT_NAME}
    PkgConfig::dfm-extension
    )

# 安裝配置
include(GNUInstallDirs)
set(LIB_INSTALL_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/dde-file-manager/plugins/extensions)

# 安裝插件
install(TARGETS
    ${PROJECT_NAME}
    LIBRARY
    DESTINATION
    ${LIB_INSTALL_DIR}
)
```

**NOTE： cmake 实例配置与以下 demo 代码无关**

### 实现元接口

在这里新建 cpp 文件 dfm-extension.cpp，实现元接口：

```C++
static DFMExtMenuPlugin *kMenuPlugin = nullptr;

extern "C" void dfm_extension_initiliaze()
{
    if (!kMenuPlugin)
        kMenuPlugin = new ICBCMenuPlugin;
}

extern "C" DFMExtMenuPlugin *dfm_extesion_menu()
{
    return kMenuPlugin;
}

extern "C" void dfm_extension_shutdown()
{
    if (kMenuPlugin) {
        delete kMenuPlugin;
        kMenuPlugin = nullptr;
    }
}
```

需要特别注意的是，需要在接口前添加 `extern "C"` ，否则文管将无法解析出接口。

## 实现相关类接口

继承右键菜单插件的基类 `DFMExtMenuPlugin` 进行实现：

icbcmenuplugin.h：

```C++
class ICBCMenuPlugin: public DFMEXT::DFMExtMenuPlugin
{
public:
    ICBCMenuPlugin();
    void initilaize(DFMEXT::DFMExtMenuProxy *proxy) DFM_FAKE_OVERRIDE;
    bool buildNormalMenu(DFMEXT::DFMExtMenu *main, const std::list<std::string> &urlList,
                                              const std::string &currentUrl, 
                                               const std::string &focusUrl, bool onDesktop) DFM_FAKE_OVERRIDE;
    bool buildEmptyAreaMenu(DFMEXT::DFMExtMenu *main, const std::string &currentUrl, bool onDesktop) DFM_FAKE_OVERRIDE;
    void hoverd(DFMEXT::DFMExtAction *action);
private:
    DFMEXT::DFMExtMenuProxy *menuProxy = nullptr;
};
```

icbcmenuplugin.cpp:

```c++
#include "icbcmenuplugin.h"

#include <menu/dfmextmenu.h>
#include <menu/dfmextmenuproxy.h>
#include <menu/dfmextaction.h>

#include <iostream>
#include <functional>
#include <unistd.h>

void ICBCMenuPlugin::hoverd(DFMEXT::DFMExtAction *action)
{
    std::cout << "hover!!!!!!!!!!" << std::endl;
    auto top = action->menu();
    if (top->actions().empty()) {
        sleep(1);
        auto a1 = menuProxy->createAction();
        a1->setText("nihao");
        top->addAction(a1);

        auto a2 = menuProxy->createAction();
        a2->setText("no hao");
        top->addAction(a2);
    }
}

ICBCMenuPlugin::ICBCMenuPlugin() : DFMEXT::DFMExtMenuPlugin()
{
    registerInitialize(std::bind(&ICBCMenuPlugin::initilaize, this, std::placeholders::_1));
    registerBuildNormalMenu(std::bind(&ICBCMenuPlugin::buildNormalMenu, this, std::placeholders::_1, std::placeholders::_2,
                                      std::placeholders::_3, std::placeholders::_4));
    registerBuildEmptyAreaMenu(std::bind(&ICBCMenuPlugin::buildEmptyAreaMenu, this, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3));
}

void ICBCMenuPlugin::initilaize(DFMEXT::DFMExtMenuProxy *proxy)
{
    std::cout << "ICBCMenuPlugin::initilaize" << std::endl;
    menuProxy = proxy;
}

bool ICBCMenuPlugin::buildNormalMenu(DFMEXT::DFMExtMenu *main, const std::list<std::string> &urlList, const std::string &currentUrl, const std::string &focusUrl, bool onDesktop)
{
    std::cout << "ICBCMenuPlugin::buildNormalMenu" << std::endl;
    if (main == nullptr)
        return false;

    auto top = menuProxy->createMenu();
    //top->registerHovered(std::bind(&hoverd,std::placeholders::_1));

    auto ac = menuProxy->createAction();
    ac->setMenu(top);
    ac->setText("wps");
    ac->registerHovered(std::bind(&ICBCMenuPlugin::hoverd,this,std::placeholders::_1));
    main->addAction(ac);

    for (auto ac : main->actions()) {
        std::cout << "ssss    :" << ac->text() << std::endl;
    }
    //todo 释放
    return true;
}

bool ICBCMenuPlugin::buildEmptyAreaMenu(DFMEXT::DFMExtMenu *main, const std::string &currentUrl, bool onDesktop)
{
    std::cout << "ICBCMenuPlugin::buildEmptyAreaMenu" << std::endl;
    return false;
}
```

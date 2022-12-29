#include <QMessageLogger>
#include <QDebug>
#include <QString>
#include <iostream>

#include "dfm-extension.h"
#include "emblemicon/dfmextemblem.h"
#include "emblemicon/dfmextemblemiconplugin.h"
#include "emblemicon/dfmextemblemiconlayout.h"

USING_DFMEXT_NAMESPACE

// 全局徽标插件单例
static DFMExtEmblemIconPlugin *kEmblemPlugin = nullptr;

// bind functions
DFMExtEmblemIconPlugin::EmblemIcons emblemIconFunc;
DFMExtEmblemIconPlugin::LocationEmblemIcons localEmblemFunc;


std::vector<std::string> demo_emblemIcons(const std::string &fileUrl)
{
   std::vector<std::string> icons;

    std::cout<<"########################## emblem-example: fileUrl: "<<fileUrl<<std::endl;

    if(fileUrl.empty()) return icons;

   // demo 使用qrc资源
   icons.push_back("://emblems/basketball.png");

   // demo 系统主题默认的emblem
   // /usr/lib/icons/xxx/48x48/emblems/emblem-default.png
   icons.push_back("emblem-default");

   return icons;
}


DFMExtEmblem demo_locationEmblemIcons(const std::string &fileUrl, int systemIconCount)
{
    std::string emblempath="://emblems/basketball.png";
    DFMExtEmblemIconLayout dsmemblem(DFMExtEmblemIconLayout::LocationType::BottomRight,emblempath);

    std::vector<DFMExtEmblemIconLayout> iconPaths;
    iconPaths.push_back(dsmemblem);

    DFMExtEmblem emblems;
    emblems.setEmblem(iconPaths);

    return emblems;
}


extern "C" void dfm_extension_initiliaze()
{
    qInfo()<<"########################## emblem-example: dfm_extension_initiliaze.....";

    if (!kEmblemPlugin)
    {
        kEmblemPlugin = new DFMExtEmblemIconPlugin;

// 方式一：据悉，此接口为旧接口。但实际效果比localEmblem要好很多
        emblemIconFunc = demo_emblemIcons;
        kEmblemPlugin->registerEmblemIcons(emblemIconFunc);

// 方式二：此接口指定了位置，当位置冲突时，徽标可能会不显示
//        localEmblemFunc = demo_locationEmblemIcons;
//        kEmblemPlugin->registerLocationEmblemIcons(localEmblemFunc);
    }
}

extern "C"  void dfm_extension_shutdown()
{
    qInfo()<<"########################## emblem-example: dfm_extension_shutdown.....";

    if (kEmblemPlugin)
    {
        delete kEmblemPlugin;
        kEmblemPlugin = nullptr;
    }
}


extern "C" DFMExtEmblemIconPlugin *dfm_extension_emblem()
{
    qInfo()<<"################# dfm_extension_emblem.......";
    return kEmblemPlugin;
}


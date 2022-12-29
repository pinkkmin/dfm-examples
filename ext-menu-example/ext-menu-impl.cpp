
#include "dfm-extension.h"
#include "menu/dfmextmenu.h"
#include "menu/dfmextaction.h"
#include "menu/dfmextmenuplugin.h"

#include <QDebug>
#include <QWidget>
#include <QMessageLogger>

USING_DFMEXT_NAMESPACE

DFMExtMenuPlugin dsm_inst;
DFMEXT::DFMExtMenuProxy *menuProxy = nullptr;

DFMExtMenuPlugin::InitializeFunc initFunc;
DFMExtMenuPlugin::BuildNormalMenuFunc buildnFunc;
DFMExtAction::TriggeredFunc triggerFunc;

void DSM_ActionTrigger(DFMExtAction *, bool check)
{
    QWidget *dlg = new QWidget();
    dlg->show();
}

void DSM_Initialize(DFMEXT::DFMExtMenuProxy *proxy)
{
    qInfo()<<"########################## DSM_Initialize proxy.......";
    menuProxy = proxy;
}

bool DSM_BuildNormalMenu(DFMEXT::DFMExtMenu *main,
                         const std::string &currentUrl,
                         const std::string &focusUrl,
                         const std::list<std::string> &urlList,
                         bool onDesktop)
{
    qInfo()<<"########################## DSM_BuildNormalMenu begin";
    if (main == nullptr)
            return false;

    // top menu1
    DFMExtAction *action1 =  menuProxy->createAction();
    action1->setText("top-menu1");
    action1->registerTriggered(triggerFunc);
    action1->setIcon("://Dame.png");

    // sub menus
    DFMExtMenu *menu = menuProxy->createMenu();

    {
        // sub menu1
        DFMExtAction *action1 =  menuProxy->createAction();
        action1->setText("sub-menu1");
        action1->registerTriggered(triggerFunc);

        // sub menu2
        DFMExtAction *action2 =  menuProxy->createAction();
        action2->setText("sub-menu2");
        action2->registerTriggered(triggerFunc);

        menu->addAction(action1);
        menu->addAction(action2);
    }

    action1->setMenu(menu);

    // top menu2
    DFMExtAction *action2 =  menuProxy->createAction();
    action2->setText("top-menu2");
    action2->registerTriggered(triggerFunc);
    action2->setIcon("://Dame.png");

    std::list<DFMExtAction *> mainActions = main->actions();
    if(mainActions.size() > 0)
    {
        DFMExtAction* posAction = mainActions.front();
        main->insertAction(posAction, action1);
        main->insertAction(action1, action2);
    }
    else
    {
        main->addAction(action1);
        main->addAction(action2);
    }
//    DFMExtAction *menuAction();
//    std::list<DFMExtAction *> actions();
    return true;
}

extern "C" void dfm_extension_initiliaze()
{
    qInfo()<<"########################## dfm_extension_initiliaze begin";

    initFunc = DSM_Initialize;
    buildnFunc = DSM_BuildNormalMenu;
    triggerFunc = DSM_ActionTrigger;

    dsm_inst.registerInitialize(initFunc);
    dsm_inst.registerBuildNormalMenu(buildnFunc);
    qInfo()<<"########################## dfm_extension_initiliaze end";
}

extern "C"  void dfm_extension_shutdown()
{
    qInfo()<<"########################## dfm_extension_shutdown ";
}

extern "C"  DFMExtMenuPlugin *dfm_extension_menu()
{
    qInfo()<<"########################## dfm_extesion_menu-----------";

    return &dsm_inst;
}


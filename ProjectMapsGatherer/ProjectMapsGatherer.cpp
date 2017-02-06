#include "stdafx.h"

#include "ProjectMapsGatherer.h"

#include "ProjectObjects/Project.h"
#include "ProjectObjects/Project_Hierarchy.h"
#include "ProjectObjects/Project_Location.h"
#include "ProjectObjects/Project_MiniGame.h"
#include "ProjectObjects/Project_HOGame.h"
#include "ProjectObjects/Project_Zoom.h"
#include "ProjectObjects/Project_GameContent.h"
#include "ProjectObjects/Project_Chapter.h"
#include "ProjectObjects/Project_CutScene.h"
#include "Minigames/Core/BaseMinigame.h"
#include "BuildSystem/Utilities/ProjectTraverser.h"
#include "GameObjects/HO/HOInstance.h"
#include "GameObjects/Items/IHOS.h"
#include "GameObjects/Switchers/PopUpSwitcher.h"
#include "GameObjects/Actions/CollectAchievementAction.h"
#include "GameObjects/Actions/AddItemAction.h"
#include "GameObjects/Achievement.h"
#include "GameObjects/Achievements/BasicAchievement.h"
#include "GameObjects/Achievements/EventAchievement.h"
#include "GameObjects/Cutscene/CutsceneInvoker.h"
#include "GameObjects/Actions/CollectCollectibleAction.h"
#include "GameObjects/ItemObject.h"
#include "GameObjects/Actions/UseItemAction.h"
#include "GameObjects/Items/ItemPickup.h"
#include "GameObjects/Items/ItemDef.h"
#include "GameObjects/Zoom/ZoomContent.h"

CProjectMapsGatherer::CProjectMapsGatherer()
{
    m_SparkMapsInfos.clear();
    m_ItemTypesCount = 0;
}

CProjectMapsGatherer::~CProjectMapsGatherer()
{
    m_SparkMapsInfos.clear();
    m_Traverser.reset();
}

void CProjectMapsGatherer::AddGameContentInfo(SSparkGameContentInfoPtr gameContentInfo)
{
    m_GameContents.push_back(gameContentInfo);
}

SSparkGameContentInfoPtr CProjectMapsGatherer::GetGameContentByName(const string &name)
{
    for (uint i = 0; i < m_GameContents.size(); ++i)
    {
        if (m_GameContents[i]->m_Name == name)
            return m_GameContents[i];
    }

    return SSparkGameContentInfoPtr();
}

bool CProjectMapsGatherer::GatherSparkMaps(CProjectPtr project)
{
    m_Project = project;
    AddMyInstructions();
    if (m_Traverser)
        m_Traverser->TraverseProject(project);

    return true;
}

const vector<SSparkMapInfoPtr>& CProjectMapsGatherer::GetMapsByClass(CConstClassTypeInfoPtr typeInfo)
{
    SparkMapInfoCollecton::iterator it, itEnd = m_SparkMapsInfos.end();
    it = m_SparkMapsInfos.find(typeInfo);
    if (it == itEnd)
    {
        static vector<SSparkMapInfoPtr> emptyVector;
        return emptyVector;
    }

    return it->second;
}

void CProjectMapsGatherer::GetMapsTypes(vector<CConstClassTypeInfoPtr> &mapsTypeInfos)
{
    SparkMapInfoCollecton::iterator it, itEnd = m_SparkMapsInfos.end();
    for (it = m_SparkMapsInfos.begin(); it != itEnd; it++)
    {
        mapsTypeInfos.push_back(it->first);
    }
}
const vector<SSparkObjectInfoPtr>& CProjectMapsGatherer::GetObjectsByClass(CConstClassTypeInfoPtr typeInfo)
{
    SparkObjectInfoCollecton::iterator it, itEnd = m_SparkObjectsInfos.end();
    it = m_SparkObjectsInfos.find(typeInfo);
    if (it == itEnd)
    {
        static vector<SSparkObjectInfoPtr> emptyVector;
        return emptyVector;
    }

    return it->second;
}

void CProjectMapsGatherer::GetObjectTypes(vector<CConstClassTypeInfoPtr> &objClassNames)
{
    SparkObjectInfoCollecton::iterator it, itEnd = m_SparkObjectsInfos.end();
    for (it = m_SparkObjectsInfos.begin(); it != itEnd; it++)
    {
        objClassNames.push_back(it->first);
    }
}

uint CProjectMapsGatherer::GetGameContentsCount()
{
    return m_GameContents.size();
}

SSparkGameContentInfoPtr CProjectMapsGatherer::GetGameContentAt(uint index)
{
    return m_GameContents[index];
}

uint CProjectMapsGatherer::GetAchievementsCount()
{
    return m_SparkAchievements.size();
}

SSparkAchievementInfoPtr CProjectMapsGatherer::GetAchievementAt(uint index)
{
    return m_SparkAchievements[index];
}

void CProjectMapsGatherer::SetProjectTraverser(CProjectTraverserPtr traverser)
{
    m_Traverser = traverser;
}

void CProjectMapsGatherer::AddMyInstructions()
{
    if (!m_Traverser)
    {
        LOG_ERROR(0, "ProjectMapsGenerator has not set any Project Traverser! Can't add it's own instructions!");
        return;
    }

    // for CProject_Hierarchy
    TraversingInstruction instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CProject_Location::GetStaticTypeInfo();
        vector<SSparkMapInfoPtr> &projHierarchies = m_SparkMapsInfos[classType];
        SSparkMapInfoPtr mapInfo = ConstructSparkMapInfo(object);
        if (mapInfo)
            projHierarchies.push_back(mapInfo);
    };
    m_Traverser->AddInstruction(CProject_Location::GetStaticTypeInfo(), instruction);

    // for CProject_Zoom
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CProject_Zoom::GetStaticTypeInfo();
        vector<SSparkMapInfoPtr> &projHierarchies = m_SparkMapsInfos[classType];
        SSparkMapInfoPtr mapInfo = ConstructSparkMapInfo(object);
        if (mapInfo)
            projHierarchies.push_back(mapInfo);
    };
    m_Traverser->AddInstruction(CProject_Zoom::GetStaticTypeInfo(), instruction);

    // for CProject_MiniGame
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CProject_MiniGame::GetStaticTypeInfo();
        vector<SSparkMapInfoPtr> &projHierarchies = m_SparkMapsInfos[classType];
        SSparkMapInfoPtr mapInfo = ConstructSparkMapInfo(object);
        if (mapInfo)
            projHierarchies.push_back(mapInfo);
    };
    m_Traverser->AddInstruction(CProject_MiniGame::GetStaticTypeInfo(), instruction);

    // for CProject_HOGame
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CProject_HOGame::GetStaticTypeInfo();
        vector<SSparkMapInfoPtr> &projHierarchies = m_SparkMapsInfos[classType];
        SSparkMapInfoPtr mapInfo = ConstructSparkMapInfo(object);
        if (mapInfo)
            projHierarchies.push_back(mapInfo);
    };
    m_Traverser->AddInstruction(CProject_HOGame::GetStaticTypeInfo(), instruction);

    // for CBaseMinigame
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CBaseMinigame::GetStaticTypeInfo();
        vector<SSparkObjectInfoPtr> &projHierarchies = m_SparkObjectsInfos[classType];
        SSparkObjectInfoPtr minigameInfo = ConstructSparkMinigameInfo(object);
        if (minigameInfo)
            projHierarchies.push_back(minigameInfo);
    };
    m_Traverser->AddInstruction(CBaseMinigame::GetStaticTypeInfo(), instruction);

    // for CHOInstance
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CHOInstance::GetStaticTypeInfo();
        vector<SSparkObjectInfoPtr> &projHierarchies = m_SparkObjectsInfos[classType];
        SSparkObjectInfoPtr minigameInfo = ConstructHOGameInfo(object);
        if (minigameInfo)
            projHierarchies.push_back(minigameInfo);
    };
    m_Traverser->AddInstruction(CHOInstance::GetStaticTypeInfo(), instruction);

    // for CIHOSInstance
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CIHOSInstance::GetStaticTypeInfo();
        vector<SSparkObjectInfoPtr> &projHierarchies = m_SparkObjectsInfos[classType];
        SSparkObjectInfoPtr minigameInfo = ConstructHOGameInfo(object);
        if (minigameInfo)
            projHierarchies.push_back(minigameInfo);
    };
    m_Traverser->AddInstruction(CIHOSInstance::GetStaticTypeInfo(), instruction);

    // for CZoomSwitcher (used for listing popups)
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CZoomSwitcher::GetStaticTypeInfo();
        vector<SSparkObjectInfoPtr> &projHierarchies = m_SparkObjectsInfos[classType];
        SSparkObjectInfoPtr popupInfo = ConstructPopupInfo(object);
        if (popupInfo)
            projHierarchies.push_back(popupInfo);
    };
    m_Traverser->AddInstruction(CZoomSwitcher::GetStaticTypeInfo(), instruction);

    // for CProject_CutScene
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CProject_CutScene::GetStaticTypeInfo();
        vector<SSparkMapInfoPtr> &projHierarchies = m_SparkMapsInfos[classType];
        SSparkMapInfoPtr cutsceneInfo = ConstructSparkMapInfo(object);
        if (cutsceneInfo)
            projHierarchies.push_back(cutsceneInfo);
    };
    m_Traverser->AddInstruction(CProject_CutScene::GetStaticTypeInfo(), instruction);

    // for CutsceneInvoker
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CCutsceneInvoker::GetStaticTypeInfo();
        vector<SSparkObjectInfoPtr> &projHierarchies = m_SparkObjectsInfos[classType];
        SSparkObjectInfoPtr cutsceneInfo = ConstructCutsceneInvokerInfo(object);
        if (cutsceneInfo)
            projHierarchies.push_back(cutsceneInfo);
    };
    m_Traverser->AddInstruction(CCutsceneInvoker::GetStaticTypeInfo(), instruction);

    // for Item collect cases
    // NOTE: We look for CItemObject because items are collected by picking up ItemObjects -> we can get collect case location that way
    instruction = [this](IHierarchyObjectPtr object) {
        AddItemCollectCase(object);
    };
    m_Traverser->AddInstruction(CItemObject::GetStaticTypeInfo(), instruction);

    // for Item collect cases
    // NOTE: We look for CAddItemAction because items are collected by picking up ItemObjects -> we can get collect case location that way
    instruction = [this](IHierarchyObjectPtr object) {
        AddItemCollectCase(object);
    };
    m_Traverser->AddInstruction(CAddItemAction::GetStaticTypeInfo(), instruction);

    // for ItemsV2
    // NOTE: We look for CItemV2Pickup because items are collected by picking up CItemV2Pickup -> we can get collect case location that way
    instruction = [this](IHierarchyObjectPtr object) {
        AddItemV2CollectCase(object);
    };
    m_Traverser->AddInstruction(CItemV2Pickup::GetStaticTypeInfo(), instruction);
    
    // For Items use cases
    instruction = [this](IHierarchyObjectPtr object) {
        AddItemUseCase(object);
    };
    m_Traverser->AddInstruction(CUseItemAction::GetStaticTypeInfo(), instruction);

    // for CCollectAchievementAction
    instruction = [this](IHierarchyObjectPtr object) {
        ConstructSparkAchiCollectCaseInfo(object);
    };
    m_Traverser->AddInstruction(CCollectAchievementAction::GetStaticTypeInfo(), instruction);

    // for CBasicAchievement
    instruction = [this](IHierarchyObjectPtr object) {
        CBasicAchievementPtr basicAchi = spark_dynamic_cast<CBasicAchievement>(object);
        if (basicAchi)
            AddSparkAchievementInfo(basicAchi);
    };
    m_Traverser->AddInstruction(CBasicAchievement::GetStaticTypeInfo(), instruction);

    // for CCollectCollectibleAction
    instruction = [this](IHierarchyObjectPtr object) {
        CConstClassTypeInfoPtr classType = CCollectCollectibleAction::GetStaticTypeInfo();
        vector<SSparkObjectInfoPtr> &projHierarchies = m_SparkObjectsInfos[classType];
        SSparkObjectInfoPtr collectible = ConstructCollectibleInfo(object);
        if (collectible)
            projHierarchies.push_back(collectible);
    };
    m_Traverser->AddInstruction(CCollectCollectibleAction::GetStaticTypeInfo(), instruction);
}

uint CProjectMapsGatherer::GetItemsCount()
{
    return m_SparkItemsInfos.size();
}

SSparkItemInfoPtr CProjectMapsGatherer::GetItemAt(uint index)
{
    return m_SparkItemsInfos[index];
}

int CProjectMapsGatherer::GetItemTypesCount()
{
    return m_ItemTypesCount;
}

const string CProjectMapsGatherer::GetItemTypeName(int itemType)
{
    switch (itemType)
    {
    case EItemType::NORMAL:
        return "Normal";
    case EItemType::PARTIAL:
        return "Partial";
    case EItemType::STACK:
        return "Stackable";
    case EItemType::COMPOSITE:
        return "Composite";
    default:
        return "Other";
    }
}

void CProjectMapsGatherer::Clear()
{
    m_SparkMapsInfos.clear();
}

SSparkMapInfoPtr CProjectMapsGatherer::ConstructSparkMapInfo(IHierarchyObjectPtr object)
{
    SSparkMapInfoPtr mapInfo;

    CProject_HierarchyPtr projectHierarchy = spark_dynamic_cast<CProject_Hierarchy>(object);
    if (!projectHierarchy)
        return mapInfo;
    // Znajdz nazwe contentu i chaptera
    string chapterName, contentName;
    GetGameContentAndChapterNames(projectHierarchy, contentName, chapterName);
    // Zrob mapke
    mapInfo.reset(new SSparkMapInfo(projectHierarchy->GetName(), projectHierarchy->m_Cheat, contentName, chapterName, projectHierarchy->GetTypeInfo(), projectHierarchy->GetGUID()));

    // Jesli znalazl sie contentName i chapterName to warto by to zapamietac
    if (!contentName.empty())
    {
        SSparkGameContentInfoPtr gameContent = GetGameContentByName(contentName);
        // Jesli taki gamecontent nie zostal juz wczesniej dodany...
        if (!gameContent)
        {
            gameContent = ConstructSparkContentInfo(contentName);
            if (gameContent)
            {
                AddGameContentInfo(gameContent);
            }
        }
        // Dodajemy chapter do gameContentInfo jesli jakis wystapil, jesli pusty, bedzie to potraktowane jako mapki bez chaptera
        if (!gameContent->HasGameChapter(chapterName))
            gameContent->AddChapter(chapterName);
    }

    //LOG_WARNING(1, "Gathered Map Info! Name: %s, GameContent: %s, ClassName: %s", mapInfo->m_Name.c_str(),
    //    mapInfo->m_ContentName.empty() ? "NONE" : mapInfo->m_ContentName.c_str(), mapInfo->m_ClassName.c_str());
    return mapInfo;
}

SSparkAchievementInfoPtr CProjectMapsGatherer::AddSparkAchievementInfo(CAchievementPtr object)
{
    string title, desc, completedDesc;
    _CUBE()->GetLocaleSystem()->GetString(object->GetTitle().c_str(), title);
    _CUBE()->GetLocaleSystem()->GetString(object->GetDescription().c_str(), desc);
    _CUBE()->GetLocaleSystem()->GetString(object->GetCompletedDescription().c_str(), completedDesc);
    SSparkAchievementInfoPtr achiInfo(new SSparkAchievementInfo(object->GetName(), title, desc, completedDesc));
    if (achiInfo)
        m_SparkAchievements.push_back(achiInfo);

    return achiInfo;
}

SSparkAchievementInfoPtr CProjectMapsGatherer::AddSparkAchievementInfo(CBasicAchievementPtr object)
{
    string title, desc, completedDesc;
    _CUBE()->GetLocaleSystem()->GetString(object->GetTitle().c_str(), title);
    _CUBE()->GetLocaleSystem()->GetString(object->GetDescription().c_str(), desc);
    _CUBE()->GetLocaleSystem()->GetString(object->GetCompletedDescription().c_str(), completedDesc);
    SSparkAchievementInfoPtr achiInfo(new SSparkAchievementInfo(object->GetName(), title, desc, completedDesc));
    
    if (achiInfo)
    {
        /*
        TODO
            nie da sie na tym etapie zebrac podpietych obiektow do tego achika, bo jeszcze sie nie wczytaly... narazie olewam

        IHierarchyObjectPtr eventObj;
        for (uint i = 0; i < object->GetConnectedObjectsSize(); ++i)
        {
            eventObj = object->GetConnectedObjectAt(i);
            SSparkObjectInfoPtr collectCase = ConstructSparkMinigameInfo(eventObj);
            if (collectCase)
                achiInfo->AddCollectCase(collectCase);
        }
        */
        m_SparkAchievements.push_back(achiInfo);
    }
    
    return achiInfo;
}

SSparkObjectInfoPtr CProjectMapsGatherer::ConstructSparkAchiCollectCaseInfo(IHierarchyObjectPtr object)
{
    SSparkObjectInfoPtr objInfo;

    if (!m_Project)
        return objInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    CCollectAchievementActionPtr action = spark_dynamic_cast<CCollectAchievementAction>(object);
    if (!action)
        return objInfo;
    CAchievementPtr achi = spark_dynamic_cast<CAchievement>( action->GetTargetAchievement() );
    if (!achi)
        return objInfo;
    CProject_HierarchyPtr projectHierarchy;
    projectHierarchy = hierarchy->GetProjectHierarchy();
    // Znalazl sie przypadek zebrania achievementa, trzeba go dodac do tego achika
    // jesli taki achi juz sie zebral, dodaj, jesli nie, dodaj achi i wtedy dodaj ten collect case
    SSparkAchievementInfoPtr achiInfo = HasSparkAchievement(achi->GetName());
    if (!achiInfo)
        achiInfo = AddSparkAchievementInfo(achi);

    // Znajdz mapke na ktorej wystapil collect case
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Znajdz mapke na ktorej wystepuje ten obiekt
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }
    // Zrob collect case
    objInfo.reset( new SSparkObjectInfo(action->GetName(), sparkMap, action->GetTypeInfo(), action->GetGUID()) );

    achiInfo->AddCollectCase(objInfo);

    return objInfo;
}

SSparkObjectInfoPtr CProjectMapsGatherer::ConstructPopupInfo(IHierarchyObjectPtr object)
{
    SSparkObjectInfoPtr objInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
#if (SPARK_SDK_VERSION_CURRENT < SPARK_SDK_VERSION_MAKER(1,12,0,0))
    //CPopupSwitcher is scheduled for deletion because of changes from Zoom System Cleanup
    //as such alternative method to distinguishing popups has to be found. For time being template name is used.
    CPopupSwitcherPtr popupSwitcher = spark_dynamic_cast<CPopupSwitcher>(object);
    CZoomContentPtr popup;
    if (!popupSwitcher)
    {
        if (CZoomSwitcherPtr switcher = spark_dynamic_cast<CZoomSwitcher>(object))
        {
            if (popup = switcher->GetTarget())
            {
                if (auto zoomTemplate = popup->GetZoomFrameTemplate())
                {
                    if (!strstr(Util::ToLower(zoomTemplate->GetName()).c_str(),"popup"))
                        popup.reset();
                }
                else
                    popup.reset();
            }
        }
    }
    else
        popup = popupSwitcher->GetTarget();
#else
    CZoomContentPtr popup;
    if (CZoomSwitcherPtr switcher = spark_dynamic_cast<CZoomSwitcher>(object))
    {
        if (popup = switcher->GetTarget())
        {
            if (auto zoomTemplate = popup->GetZoomFrameTemplate())
            {
                if (!strstr(Util::ToLower(zoomTemplate->GetName()).c_str(), "popup"))
                    popup.reset();
            }
            else
                popup.reset();
        }
    }
#endif
    if (!popup)
        return objInfo;
    CProject_HierarchyPtr projectHierarchy;

    // Znajdz lokacje pod ktora jest dany popup switcher (w przypadku jesli np. byl na zoomie)
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Znajdz mapke na ktorej wystepuje ten obiekt
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }
    // Zrob obiekt
    objInfo.reset(new SSparkObjectInfo(popup->GetName(), sparkMap, popup->GetTypeInfo(), popup->GetGUID()));

    return objInfo;
}

SSparkObjectInfoPtr CProjectMapsGatherer::ConstructHOGameInfo(IHierarchyObjectPtr object)
{
    SSparkObjectInfoPtr objInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    CHierarchyObjectPtr hoObj;
    // is normal HO?
    CHOInstancePtr hoInst = spark_dynamic_cast<CHOInstance>(object);
    if (!hoInst)
    {
        // is IHOS?
        CIHOSInstancePtr ihosInst = spark_dynamic_cast<CIHOSInstance>(object);
        if (!ihosInst)
            return objInfo;
        hoObj = ihosInst;
    }
    else
        hoObj = hoInst;

    CProject_HierarchyPtr projectHierarchy;
    projectHierarchy = hierarchy->GetProjectHierarchy();

    // Find map on which this instance is
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }
    // Create object
    objInfo.reset(new SSparkObjectInfo(hoObj->GetName(), sparkMap, hoObj->GetTypeInfo(), hoObj->GetGUID()));

    return objInfo;
}

SSparkObjectInfoPtr CProjectMapsGatherer::ConstructSparkMinigameInfo(IHierarchyObjectPtr object)
{
    SSparkObjectInfoPtr objInfo;

    CBaseMinigamePtr minigame = spark_dynamic_cast<CBaseMinigame>(object);
    if (!minigame)
        return objInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    CProject_HierarchyPtr projectHierarchy;
    projectHierarchy = hierarchy->GetProjectHierarchy();

    // Znajdz mapke na ktorej wystepuje ten obiekt
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }
    // Zrob obiekt
    objInfo.reset(new SSparkObjectInfo(object->GetName(), sparkMap, object->GetTypeInfo(), object->GetGUID()));

    return objInfo;
}

SSparkObjectInfoPtr CProjectMapsGatherer::ConstructCollectibleInfo(IHierarchyObjectPtr object)
{
    SSparkObjectInfoPtr objInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    shared_ptr<CCollectCollectibleAction> collectCase = spark_dynamic_cast<CCollectCollectibleAction>(object);
    if (!collectCase)
        return objInfo;
    CHierarchyObjectPtr collectible = collectCase->GetTarget();
    if (!collectible)
        return objInfo;

    CProject_HierarchyPtr projectHierarchy;
    // Find location on which collect case occurred
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Find SparkMap on which this is supposed to be
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }
    // Create object
    objInfo.reset(new SSparkObjectInfo(collectible->GetName(), sparkMap, collectCase->GetTypeInfo(), collectCase->GetGUID()));

    return objInfo;
}

SSparkGameContentInfoPtr CProjectMapsGatherer::ConstructSparkContentInfo(const string &name)
{
    SSparkGameContentInfoPtr contentInfo(new SSparkGameContentInfo(name));
    
    return contentInfo;
}

SSparkObjectInfoPtr CProjectMapsGatherer::ConstructCutsceneInvokerInfo(IHierarchyObjectPtr object)
{
    SSparkObjectInfoPtr objInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    CCutsceneInvokerPtr cutsceneInvoker = spark_dynamic_cast<CCutsceneInvoker>(object);
    if (!cutsceneInvoker || !cutsceneInvoker->GetShowBlackbars())
        return objInfo;

    CProject_HierarchyPtr projectHierarchy;
    // Znajdz lokacje pod ktora jest dany cutsceneInvoker
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Znajdz mapke na ktorej wystepuje ten obiekt
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }
    // Zrob obiekt
    objInfo.reset(new SSparkObjectInfo(cutsceneInvoker->GetName(), sparkMap, cutsceneInvoker->GetTypeInfo(), cutsceneInvoker->GetGUID()));

    return objInfo;
}

SSparkItemInfo* CProjectMapsGatherer::AddItemInfo(IHierarchyObjectPtr object)
{
    SSparkItemInfoPtr itemInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    // Is it normal item?
    CItemPtr item = spark_dynamic_cast<CItem>(object);
    if (!item)
    {
        // is it a V2 item?
        CItemV2DefPtr itemV2 = spark_dynamic_cast<CItemV2Def>(object);
        if (!itemV2)
            return NULL;

        // make object
        string itemName;
        _CUBE()->GetLocaleSystem()->GetString(itemV2->GetItemName().c_str(), itemName);
        itemInfo.reset(new SSparkItemInfo(itemName, itemV2->GetTypeInfo(), EItemType::NORMAL, itemV2->GetGUID()));
    }
    else
    {
        // make object
        itemInfo.reset(new SSparkItemInfo(item->GetName(), item->GetTypeInfo(), item->GetType(), item->GetGUID()));

        // Count item types so we can iterate through all of them later
        int itemTypeNumber = (int)item->GetType();
        if (m_ItemTypesCount < itemTypeNumber)
            m_ItemTypesCount = itemTypeNumber;
    }

    m_SparkItemsInfos.push_back(itemInfo);

    return itemInfo.get();
}

void CProjectMapsGatherer::AddItemUseCase(IHierarchyObjectPtr object)
{
    IHierarchyPtr hierarchy = object->GetHierarchy();
    CUseItemActionPtr itemUse = spark_dynamic_cast<CUseItemAction>(object);
    if (!itemUse)
        return;

    CItemPtr item = itemUse->GetTarget();
    if (!item)
        return;

    // Find corresponding SparkItemInfo
    SSparkItemInfo* sparkItem = FindSparkItem(item->GetGUID());
    if (!sparkItem)
    {
        sparkItem = AddItemInfo(item);
    }

    CProject_HierarchyPtr projectHierarchy;
    // Find location on which this ItemObject is
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Find map corresponding to that location
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }

    // Add this location to the item as use case location
    sparkItem->AddUseCaseLocation(sparkMap);
}

void CProjectMapsGatherer::AddItemCollectCase(IHierarchyObjectPtr object)
{
    SSparkItemInfoPtr itemInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    CItemPtr item;
    CItemObjectPtr itemObject = spark_dynamic_cast<CItemObject>(object);
    if (itemObject)
        item = itemObject->GetItem();

    if (!item)
    {
        if (CAddItemActionPtr action = spark_dynamic_cast<CAddItemAction>(object))
            item = spark_dynamic_cast<CItem>(action->GetTarget());
    }
    
    if (!item)
        return;

    // Find corresponding SparkItemInfo
    SSparkItemInfo* sparkItem = FindSparkItem(item->GetGUID());
    if (!sparkItem)
    {
        sparkItem = AddItemInfo(item);
    }

    CProject_HierarchyPtr projectHierarchy;
    // Find location on which this ItemObject is
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Find map corresponding to that location
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }

    // Add this location to the item as collect case location
    sparkItem->AddCollectCaseLocation(sparkMap);
}

void CProjectMapsGatherer::AddItemV2CollectCase(IHierarchyObjectPtr object)
{
    SSparkItemInfoPtr itemInfo;

    IHierarchyPtr hierarchy = object->GetHierarchy();
    CItemV2PickupPtr itemObject = spark_dynamic_cast<CItemV2Pickup>(object);
    if (!itemObject)
        return;

    CItemV2DefPtr item = itemObject->GetItemDefinition();
    if (!item)
        return;

    // Find corresponding SparkItemInfo
    SSparkItemInfo* sparkItem = FindSparkItem(item->GetGUID());
    if (!sparkItem)
    {
        sparkItem = AddItemInfo(item);
    }

    CProject_HierarchyPtr projectHierarchy;
    // Find location on which this ItemObject is
    projectHierarchy = hierarchy->GetProjectHierarchy();
    projectHierarchy = FindParentLocation(projectHierarchy);

    // Find map corresponding to that location
    SSparkMapInfo* sparkMap = NULL;
    if (projectHierarchy)
    {
        sparkMap = FindSparkMap(projectHierarchy->GetGUID());
    }

    // Add this location to the item as collect case location
    sparkItem->AddCollectCaseLocation(sparkMap);
}

SSparkItemInfo* CProjectMapsGatherer::FindSparkItem(CUBE_GUID guid)
{
    // Look for item with this guid
    for (uint i = 0; i < m_SparkItemsInfos.size(); ++i)
    {
        if (m_SparkItemsInfos[i]->m_GUID == guid)
            return m_SparkItemsInfos[i].get();
    }

    return NULL;
}

SSparkAchievementInfoPtr CProjectMapsGatherer::HasSparkAchievement(const sk::string &achiName)
{
    for (uint i = 0; i < m_SparkAchievements.size(); ++i)
    {
        if (m_SparkAchievements[i]->m_Name == achiName)
            return m_SparkAchievements[i];
    }

    return SSparkAchievementInfoPtr();
}

void CProjectMapsGatherer::GetGameContentAndChapterNames(CProject_HierarchyPtr projHier, string &contentName, string &chapterName)
{
    // Przegladamy rodzicow gore, az znajdziemy gamecontent, po drodze sprawdzamy czy mapka znajduje sie w jakims chapterze
    IHierarchyObjectPtr parent = projHier->GetParent();
    while (parent && parent->GetClassName() != "CProject_GameContent")
    {
        if (parent->IsType(CProject_Chapter::GetStaticTypeInfo()))
            chapterName = parent->GetName();
        parent = parent->GetParent();
    }

    if (parent)
        contentName = parent->GetName();
}

void CProjectMapsGatherer::GetGameContent(CProject_HierarchyPtr projHier, string &contentName)
{
    // Przegladamy rodzicow gore, az znajdziemy gamecontent
    IHierarchyObjectPtr parent = projHier->GetParent();
    while (parent && parent->GetClassName() != "CProject_GameContent")
    {
        parent = parent->GetParent();
    }

    if (parent && parent->GetClassName() == "CProject_GameContent")
        contentName = parent->GetName();
}

CProject_HierarchyPtr CProjectMapsGatherer::FindParentLocation(CProject_HierarchyPtr projHierarchy)
{
    bool isLocation = false;
    IHierarchyObjectPtr location = projHierarchy;
    while (location)
    {
        isLocation = location->IsType(CProject_Location::GetStaticTypeInfo());
        isLocation = isLocation || location->IsType(CProject_HOGame::GetStaticTypeInfo());
        isLocation = isLocation || location->IsType(CProject_MiniGame::GetStaticTypeInfo());
        if (isLocation)
            break;
        location = location->GetParent();
    }

    projHierarchy = spark_dynamic_cast<CProject_Hierarchy>(location);

    return projHierarchy;
}

SSparkMapInfo* CProjectMapsGatherer::FindSparkMap(CUBE_GUID guid)
{
    // Look for locations
    vector<SSparkMapInfoPtr>& maps = m_SparkMapsInfos[CProject_Location::GetStaticTypeInfo()];
    for (uint i = 0; i < maps.size(); ++i)
    {
        if (maps[i]->m_GUID == guid)
            return maps[i].get();
    }

    //if not, then on minigames
    vector<SSparkMapInfoPtr>& minigames = m_SparkMapsInfos[CProject_MiniGame::GetStaticTypeInfo()];
    for (uint i = 0; i < minigames.size(); ++i)
    {
        if (minigames[i]->m_GUID == guid)
            return minigames[i].get();
    }

    // finally on ho games
    vector<SSparkMapInfoPtr>& hoGames = m_SparkMapsInfos[CProject_HOGame::GetStaticTypeInfo()];
    for (uint i = 0; i < hoGames.size(); ++i)
    {
        if (hoGames[i]->m_GUID == guid)
            return hoGames[i].get();
    }

    return NULL;
}
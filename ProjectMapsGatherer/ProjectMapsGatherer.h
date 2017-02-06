#pragma once

#include "SparkMapInfo.h"

SK_ENTER_NAMESPACE

class CProjectMapsGatherer;
typedef shared_ptr<CProjectMapsGatherer> CProjectMapsGathererPtr;

class CProjectMapsGatherer
{
public:
    CProjectMapsGatherer();
    ~CProjectMapsGatherer();

    void GatherSparkMapsByClass(IHierarchyObjectPtr root, const sk::string &className);
    bool GatherSparkMaps(CProjectPtr project);

    const vector<SSparkMapInfoPtr>& GetMapsByClass(CConstClassTypeInfoPtr typeInfo);
    void GetMapsTypes(vector<CConstClassTypeInfoPtr> &mapsTypeInfos);
    const vector<SSparkObjectInfoPtr>& GetObjectsByClass(CConstClassTypeInfoPtr typeInfo);
    void GetObjectTypes(vector<CConstClassTypeInfoPtr> &objClassNames);

    uint GetGameContentsCount();
    SSparkGameContentInfoPtr GetGameContentAt(uint index);

    uint GetAchievementsCount();
    SSparkAchievementInfoPtr GetAchievementAt(uint index);

    void SetProjectTraverser(CProjectTraverserPtr traverser);
    void AddMyInstructions();

    uint GetItemsCount();
    SSparkItemInfoPtr GetItemAt(uint index);
    int GetItemTypesCount();
    const string GetItemTypeName(int itemType);

    void Clear();

private:
    void GatherSparkMapsRecursive(IHierarchyObjectPtr node, CConstClassTypeInfoPtr pClass);

    void AddGameContentInfo(SSparkGameContentInfoPtr gameContentInfo);
    SSparkGameContentInfoPtr GetGameContentByName(const string &name);

    SSparkAchievementInfoPtr AddSparkAchievementInfo(CAchievementPtr object);
    SSparkAchievementInfoPtr AddSparkAchievementInfo(CBasicAchievementPtr object);
    SSparkObjectInfoPtr ConstructSparkAchiCollectCaseInfo(IHierarchyObjectPtr object);
    SSparkObjectInfoPtr ConstructPopupInfo(IHierarchyObjectPtr object);
    SSparkObjectInfoPtr ConstructHOGameInfo(IHierarchyObjectPtr object);
    SSparkMapInfoPtr ConstructSparkMapInfo(IHierarchyObjectPtr node);
    SSparkObjectInfoPtr ConstructSparkMinigameInfo(IHierarchyObjectPtr node);
    SSparkGameContentInfoPtr ConstructSparkContentInfo(const string &name);
    SSparkObjectInfoPtr ConstructCutsceneInvokerInfo(IHierarchyObjectPtr object);
    SSparkObjectInfoPtr ConstructCollectibleInfo(IHierarchyObjectPtr object);

    SSparkItemInfo* AddItemInfo(IHierarchyObjectPtr object);
    void AddItemUseCase(IHierarchyObjectPtr object);
    void AddItemCollectCase(IHierarchyObjectPtr object);
    void AddItemV2CollectCase(IHierarchyObjectPtr object);
    SSparkItemInfo* FindSparkItem(CUBE_GUID guid);

    SSparkAchievementInfoPtr HasSparkAchievement(const sk::string &achiName);
    void GetGameContentAndChapterNames(CProject_HierarchyPtr projHier, string &contentName, string &chapterName);
    void GetGameContent(CProject_HierarchyPtr projHier, string &contentName);
    CProject_HierarchyPtr FindParentLocation(CProject_HierarchyPtr projHierarchy);
    SSparkMapInfo* FindSparkMap(CUBE_GUID guid);

    CProjectPtr m_Project;
    SparkAchievementsCollection m_SparkAchievements;
    SparkMapInfoCollecton m_SparkMapsInfos;
    SparkObjectInfoCollecton m_SparkObjectsInfos;
    SparkItemsInfoCollection m_SparkItemsInfos;
    int m_ItemTypesCount;
    vector<SSparkGameContentInfoPtr> m_GameContents;
    string m_CurrentGameContentName;
    string m_CurrentGroupName;

    CProjectTraverserPtr m_Traverser;
};

SK_LEAVE_NAMESPACE
#pragma once

#include "GameObjects/Item.h"

SK_ENTER_NAMESPACE

struct SSparkMapInfo;
typedef shared_ptr<SSparkMapInfo> SSparkMapInfoPtr;

struct SSparkObjectInfo;
typedef shared_ptr<SSparkObjectInfo> SSparkObjectInfoPtr;

struct SSparkItemInfo;
typedef shared_ptr<SSparkItemInfo> SSparkItemInfoPtr;

struct SSparkAchievementInfo;
typedef shared_ptr<SSparkAchievementInfo> SSparkAchievementInfoPtr;

struct SSparkGameContentInfo;
typedef shared_ptr<SSparkGameContentInfo> SSparkGameContentInfoPtr;

typedef map< CConstClassTypeInfoPtr, vector<SSparkMapInfoPtr> > SparkMapInfoCollecton;
typedef map< CConstClassTypeInfoPtr, vector<SSparkObjectInfoPtr> > SparkObjectInfoCollecton;
typedef vector<SSparkItemInfoPtr> SparkItemsInfoCollection;
typedef vector<SSparkAchievementInfoPtr> SparkAchievementsCollection;

// ---------------------------- SSparkMapInfo ---------------------------

// For classes that inherit after CProject_Hierarchy
struct SSparkMapInfo
{
    sk::string m_Name;
    sk::string m_Cheat;
    sk::string m_ContentName;
    sk::string m_Chapter;
    CConstClassTypeInfoPtr m_ClassType;
    CUBE_GUID m_GUID;

    SSparkMapInfo();

    SSparkMapInfo(const string &name, const string &cheat, const string &contentName, const string &chapterName, CConstClassTypeInfoPtr classType, CUBE_GUID guid);
};

// ---------------------------- SSparkObjectInfo ---------------------------

// For classes that inherit after IHierarchyObject
struct SSparkObjectInfo
{
    sk::string m_Name;
    CConstClassTypeInfoPtr m_ClassType;
    CUBE_GUID m_GUID;

    SSparkMapInfo* m_Location;

    SSparkObjectInfo();

    SSparkObjectInfo(const string &name, SSparkMapInfo* location, CConstClassTypeInfoPtr classTyp, CUBE_GUID guid);

    const string &GetLocationName();
    const string &GetLocationCheat();
    virtual const string &GetContentName();
};

// ---------------------------- SSparkItemInfo ---------------------------

// For Items (CItem, CItemObject) 
struct SSparkItemInfo : public SSparkObjectInfo
{
    EItemType::TYPE m_ItemType;

    vector<SSparkMapInfo*> m_CollectLocations;
    vector<uint> m_CollectLocationsCounts;
    vector<SSparkMapInfo*> m_UseLocations;
    vector<uint> m_UseLocationsCounts;

    SSparkItemInfo(const string &name, CConstClassTypeInfoPtr classType, EItemType::TYPE itemType, CUBE_GUID guid);

    // returns map at index and it's count as for how many times it was added (for when item is used multiple times on the same location)
    SSparkMapInfo* GetCollectCaseLocation(uint index, uint& count);
    SSparkMapInfo* GetUseCaseLocation(uint index, uint& count);
    void AddCollectCaseLocation(SSparkMapInfo* sparkMap);
    void AddUseCaseLocation(SSparkMapInfo* sparkMap);
    uint GetCollectCaseLocationCount();
    uint GetUseCaseLocationCount();
    EItemType::TYPE GetType();

    virtual const string &GetContentName();
};

// ---------------------------- SSparkAchievementInfo ---------------------------

struct SSparkAchievementInfo
{
public:
    sk::string m_Name;
    sk::string m_Title;
    sk::string m_Desc;
    sk::string m_CompletedDesc;

    SSparkAchievementInfo();

    SSparkAchievementInfo(const string &name, const string &title, const string &describtion, const string &completedDesc);

    uint GetCollectCasesCount();
    SSparkObjectInfoPtr GetCollectCase(uint index);
    void AddCollectCase(SSparkObjectInfoPtr collectCase);

protected:
    vector<SSparkObjectInfoPtr> m_CollectCases;
};

// ------------------------ SSparkGameContentInfo -----------------------

struct SSparkGameContentInfo
{
public:
    sk::string m_Name;
private:
    vector<sk::string> m_Chapters;

public:

    SSparkGameContentInfo(const string &name);

    void AddChapter(sk::string &chapterName);

    uint GetChapterCount();

    const string &GetChapterAt(uint index);

    bool HasGameChapter(const string &name);

    void ClearChapters();
};

SK_LEAVE_NAMESPACE
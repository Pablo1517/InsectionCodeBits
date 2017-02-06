#include "stdafx.h"

#include "SparkMapInfo.h"
#include "SparkStrings.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////    SSparkMapInfo     //////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SSparkMapInfo::SSparkMapInfo()
{
    m_ContentName = "";
}

SSparkMapInfo::SSparkMapInfo(const string &name, const string &cheat, const string &contentName, const string &chapterName, CConstClassTypeInfoPtr classType, CUBE_GUID guid)
{
    m_Name = name;
    m_Cheat = cheat;
    m_ContentName = contentName;
    m_Chapter = chapterName;
    m_ClassType = classType;
    m_GUID = guid;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////    SSparkObjectInfo     ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

SSparkObjectInfo::SSparkObjectInfo()
{
}

SSparkObjectInfo::SSparkObjectInfo(const string &name, SSparkMapInfo* location, CConstClassTypeInfoPtr classType, CUBE_GUID guid)
{
    m_Name = name;
    m_Location = location;
    m_ClassType = classType;
    m_GUID = guid;
}

const string &SSparkObjectInfo::GetLocationName()
{
    return m_Location ? m_Location->m_Name : strEmpty;
}

const string &SSparkObjectInfo::GetLocationCheat()
{
    return m_Location ? m_Location->m_Cheat : strEmpty;
}

const string &SSparkObjectInfo::GetContentName()
{
    return m_Location ? m_Location->m_ContentName : strEmpty;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////    SSparkItemInfo     /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SSparkItemInfo::SSparkItemInfo(const string &name, CConstClassTypeInfoPtr classType, EItemType::TYPE itemType, CUBE_GUID guid)
{
    m_Name = name;
    m_Location = 0;
    m_ClassType = classType;
    m_ItemType = itemType;
    m_GUID = guid;
}

SSparkMapInfo* SSparkItemInfo::GetCollectCaseLocation(uint index, uint& count)
{
    count = m_CollectLocationsCounts[index];
    return m_CollectLocations[index];
}

SSparkMapInfo* SSparkItemInfo::GetUseCaseLocation(uint index, uint& count)
{
    count = m_UseLocationsCounts[index];
    return m_UseLocations[index];
}

void SSparkItemInfo::AddCollectCaseLocation(SSparkMapInfo* sparkMap)
{
    if( !sparkMap )
        return;
    for (uint i = 0; i < m_CollectLocations.size(); ++i)
    {
        if (m_CollectLocations[i] == sparkMap)
        {
            m_CollectLocationsCounts[i]++;
            return;
        }
    }
    m_CollectLocations.push_back(sparkMap);
    m_CollectLocationsCounts.push_back(1);
}

void SSparkItemInfo::AddUseCaseLocation(SSparkMapInfo* sparkMap)
{
    for (uint i = 0; i < m_UseLocations.size(); ++i)
    {
        if (m_UseLocations[i] == sparkMap)
        {
            m_UseLocationsCounts[i]++;
            return;
        }
    }
    m_UseLocations.push_back(sparkMap);
    m_UseLocationsCounts.push_back(1);
}

uint SSparkItemInfo::GetCollectCaseLocationCount()
{
    return m_CollectLocations.size();
}

uint SSparkItemInfo::GetUseCaseLocationCount()
{
    return m_UseLocations.size();
}

EItemType::TYPE SSparkItemInfo::GetType()
{
    return m_ItemType;
}

const string &SSparkItemInfo::GetContentName()
{
    if( m_CollectLocations.size() > 0 && m_CollectLocations[0] )
        return m_CollectLocations[0]->m_ContentName;
    else return strEmpty;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////    SSparkAchievementInfo     ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

SSparkAchievementInfo::SSparkAchievementInfo()
{
}

SSparkAchievementInfo::SSparkAchievementInfo(const string &name, const string &title, const string &describtion, const string &completedDesc)
{
    m_Name = name;
    m_Title = title;
    m_Desc = describtion;
    m_CompletedDesc = completedDesc;
}

uint SSparkAchievementInfo::GetCollectCasesCount()
{
    return m_CollectCases.size();
}

SSparkObjectInfoPtr SSparkAchievementInfo::GetCollectCase(uint index)
{
    return m_CollectCases[index];
}

void SSparkAchievementInfo::AddCollectCase(SSparkObjectInfoPtr collectCase)
{
    m_CollectCases.push_back(collectCase);
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////    SSparkGameContentInfo     /////////////////////////
//////////////////////////////////////////////////////////////////////////////

SSparkGameContentInfo::SSparkGameContentInfo(const string &name)
{
    m_Name = name;
}

void SSparkGameContentInfo::AddChapter(sk::string &chapterName)
{
    m_Chapters.push_back(chapterName);
}

uint SSparkGameContentInfo::GetChapterCount()
{
    return m_Chapters.size();
}

const string &SSparkGameContentInfo::GetChapterAt(uint index)
{
    return m_Chapters[index];
}

bool SSparkGameContentInfo::HasGameChapter(const string &name)
{
    for (uint i = 0; i < m_Chapters.size(); ++i)
    {
        if (m_Chapters[i] == name)
            return true;
    }

    return false;
}

void SSparkGameContentInfo::ClearChapters()
{
    m_Chapters.clear();
}
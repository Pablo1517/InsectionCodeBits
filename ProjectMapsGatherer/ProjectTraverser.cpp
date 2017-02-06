#include "stdafx.h"
#include "SparkStrings.h"

#include "BuildSystem/Utilities/ProjectTraverser.h"

#include "GameObjects/Item.h"
#include "FileSystem/FileSystem.h"
#include "Objects/WideScene2D.h"
#include "ProjectObjects/Project_Hierarchy.h"
#include "ProjectObjects/Project_Resources.h"
#include "ProjectObjects/Project_Group.h"
#include "ProjectObjects/Project_Dialogs.h"
#include "ProjectObjects/Project_Extras.h"
#include "ProjectObjects/Project_Zoom.h"
#include "ProjectObjects/Project_HOGame.h"
#include "ProjectObjects/Project.h"
#include "HierarchyAndObjects/Hierarchy.h"
#include "BuildSettings/BuildSettings_Build.h"
#include "BuildSettings/BuildSettings_Parameters.h"
#include "Localization/LocaleSystem.h"
#include "GameObjects/Dialogs/Dialog.h"
#include "GameObjects/Zoom/ZoomContent.h"

#ifndef NO_EDITOR

CProjectTraverser::CProjectTraverser()
{
}

CProjectTraverser::~CProjectTraverser()
{
    m_TraversingInstructions.clear();
    m_ClassTypes.clear();
}

void CProjectTraverser::AddInstruction(CConstClassTypeInfoPtr classType, TraversingInstruction &instruction)
{
    TraversingInstructionList& instructionList = m_TraversingInstructions[classType];
    instructionList.push_back(instruction);
    m_ClassTypes.push_back(classType);
    //Func::VectorAddUnique(instructionList, instruction);
}

void CProjectTraverser::TraverseProject(CProjectPtr project)
{
    vector<IHierarchyObjectCollectionPtr> l_hierarchies;
    CConstClassTypeInfoPtr l_filter;

    // Najpierw wyszukujemy wszystkie elementy klasy CProject_Location
    l_filter = _CUBE()->CreateClassFilter( "CProject_Location" );
    l_hierarchies.push_back(project->FindObjects(l_filter));
    l_filter = _CUBE()->CreateClassFilter( "CProject_HUD" );
    l_hierarchies.push_back(project->FindObjects(l_filter));
    l_filter = _CUBE()->CreateClassFilter( "CProject_InGameMenu" );
    l_hierarchies.push_back(project->FindObjects(l_filter));
    l_filter = _CUBE()->CreateClassFilter( "CProject_Hierarchy" );
    l_hierarchies.push_back(project->FindObjects(l_filter));
    l_filter = _CUBE()->CreateClassFilter( "CProject_CutScene" );
    l_hierarchies.push_back(project->FindObjects(l_filter));

    // Zliczamy ile mamy hierarchii do wczytania
    int progressAll = 0;
    int progressCurrent = 0;
    for (uint i = 0; i < l_hierarchies.size(); ++i)
        progressAll += l_hierarchies[i]->GetObjectCount();

    // Zbieramy wszystkie znaleznione projektowe obiekty hierarchii
    vector<CProject_HierarchyPtr> projectHierarchyNodes;
    for (uint h = 0; h < l_hierarchies.size(); ++h)
    {
        for (uint i = 0; i < l_hierarchies[h]->GetObjectCount(); i++)
        {
            IHierarchyObjectPtr map = l_hierarchies[h]->GetObjectAt(i);
            CProject_HierarchyPtr projectHierarchy = spark_dynamic_cast<CProject_Hierarchy>(map);
            
            Func::VectorAddUnique(projectHierarchyNodes, projectHierarchy);

            // Do³adowujemy wszystkie dzieci do lokacji (minigry, ho, zoom'y)
            for (uint j = 0; j < projectHierarchy->GetChildCount(); ++j)
            {
                IHierarchyObjectPtr l_sub_object = projectHierarchy->GetChild(j);
                CProject_HierarchyPtr subProjectHierarchy = spark_dynamic_cast<CProject_Hierarchy>(l_sub_object);
                if (subProjectHierarchy)
                    Func::VectorAddUnique(projectHierarchyNodes, subProjectHierarchy);
            }
        }
    }
    l_hierarchies.clear();

    // load common hierarchies
    vector<LoadedProjectHierarchyInfo> loadedCommonHierarchies;
    for(uint i = 0; i < projectHierarchyNodes.size(); i++)
    {
        CProject_HierarchyPtr projectHierarchy = projectHierarchyNodes[i];
        if(project->GetGameContentType(projectHierarchy) == EGameContentType::UNKNOWN) // hierarchies that are beyond game content
        {
            // Wy³uskujemy nazwê pliku z hierarchi¹
            string hierarchyFilename = projectHierarchy->GetSchema()->FindProperty(strPropertyFilename, "")->AsString();
            if (hierarchyFilename.empty())
                continue;

            // Tworzymy i ³adujemy lokacjê
            IHierarchyPtr hierarchy = _CUBE()->CreateHierarchy(true, true);
            hierarchy->Load(hierarchyFilename, true);
            hierarchy->RegisterProjectHierarchy(projectHierarchy);
            loadedCommonHierarchies.push_back(LoadedProjectHierarchyInfo(projectHierarchy, hierarchy, projectHierarchy->NoResGroup()));
        }
    }

    // traverse common hierarchies
    for(uint i = 0; i < loadedCommonHierarchies.size(); i++)
    {
        if(!loadedCommonHierarchies[i].skipGatheringResources)
            TraverseHierarchy(project, loadedCommonHierarchies[i].projectHierarchy, loadedCommonHierarchies[i].hierarchy);
    }
    // project
    TraverseHierarchy(project, CProject_HierarchyPtr(), project->GetHierarchy());

    for(uint gc = 0; gc < sk::EGameContentType::COUNT; ++gc)
    {
        // load hierarchies for given game content
        vector<LoadedProjectHierarchyInfo> loadedGameContentHierarchies;
        for(uint i = 0; i < projectHierarchyNodes.size(); i++)
        {
            CProject_HierarchyPtr projectHierarchy = projectHierarchyNodes[i];
            if(project->GetGameContentType(projectHierarchy) == EGameContentType::TYPE(gc)) // hierarchies that are in given game content
            {
                // Wy³uskujemy nazwê pliku z hierarchi¹
                string hierarchyFilename = projectHierarchy->GetSchema()->FindProperty(strPropertyFilename, "")->AsString();
                if (hierarchyFilename.empty())
                    continue;

                // Tworzymy i ³adujemy lokacjê
                IHierarchyPtr hierarchy = _CUBE()->CreateHierarchy(true, true);
                hierarchy->Load(hierarchyFilename, true);
                hierarchy->RegisterProjectHierarchy(projectHierarchy);
                loadedGameContentHierarchies.push_back(LoadedProjectHierarchyInfo(projectHierarchy, hierarchy, projectHierarchy->NoResGroup()));
            }
        }

        // traverse loaded game content hierarchies
        for(uint i = 0; i < loadedGameContentHierarchies.size(); i++)
        {
            if(!loadedGameContentHierarchies[i].skipGatheringResources)
                TraverseHierarchy(project, loadedGameContentHierarchies[i].projectHierarchy, loadedGameContentHierarchies[i].hierarchy);
        }
        
        // unload game content hierarchies
        for(uint i = 0; i < loadedGameContentHierarchies.size(); i++)
            loadedGameContentHierarchies[i].hierarchy->Finalize();
        loadedGameContentHierarchies.clear();
    }

    // unload common hierarchies
    for(uint i = 0; i < loadedCommonHierarchies.size(); i++)
        loadedCommonHierarchies[i].hierarchy->Finalize();
    loadedCommonHierarchies.clear();
}

void CProjectTraverser::TraverseHierarchy(CProjectPtr project, CProject_HierarchyPtr projectHierarchy, IHierarchyPtr hierarchy)
{
    bool skipHierarchyResGathering = false;

    // Wy³uskujemy wszystkie mo¿liwe nazwy plików z lokacji
    CConstClassTypeInfoPtr classType;
    HierarchyObjectVector objectVector;
    hierarchy->GetHierarchyObjectVector(objectVector);
    for (HierarchyObjectVector::iterator it = objectVector.begin(); it != objectVector.end(); ++it)
    {
        IHierarchyObjectPtr hierarchyObject = (*it);

        // Tutaj jest przemapowanie na odpowiednia lambde do wykonania
        classType.reset();
        for (uint i = 0; i < m_ClassTypes.size(); ++i)
        {
            if (hierarchyObject->IsEqualType(m_ClassTypes[i]) || hierarchyObject->IsType(m_ClassTypes[i]))
            {
                classType = m_ClassTypes[i];
                break;
            }
        }
        if (!classType)
            continue;
        TraversingInstructionMap::iterator insIt, insItEnd = m_TraversingInstructions.end();
        insIt = m_TraversingInstructions.find(classType);
        if (insIt != insItEnd)
        {
            // Znalazlem liste instrukcji jak postepowac z tym typem obiektu, wykonuj
            TraversingInstructionList& instructionList = insIt->second;
            for (uint h = 0; h < instructionList.size(); ++h)
            {
                TraversingInstruction& instruction = instructionList[h];
                if (instruction)
                    instruction(hierarchyObject);
            }
        }
    }
}

#endif

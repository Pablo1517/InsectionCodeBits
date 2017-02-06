#ifndef __PROJECT_TRAVERSER_H__
#define __PROJECT_TRAVERSER_H__
#ifndef NO_EDITOR

SK_ENTER_NAMESPACE

typedef sk::function<void(IHierarchyObjectPtr)> TraversingInstruction;
typedef vector<TraversingInstruction> TraversingInstructionList;
typedef map < CConstClassTypeInfoPtr, TraversingInstructionList > TraversingInstructionMap;

struct LoadedProjectHierarchyInfo
{
    CProject_HierarchyPtr projectHierarchy;
    IHierarchyPtr hierarchy;
    bool skipGatheringResources;

    LoadedProjectHierarchyInfo(CProject_HierarchyPtr ph, IHierarchyPtr h, bool skip) : projectHierarchy(ph), hierarchy(h), skipGatheringResources(skip) {}
};

class SPARK_CORE_API CProjectTraverser
{
public:
    CProjectTraverser();
    ~CProjectTraverser();
   
    void AddInstruction(CConstClassTypeInfoPtr classType , TraversingInstruction &instruction);

    void TraverseProject(CProjectPtr project);
protected:
    void TraverseHierarchy(CProjectPtr project, CProject_HierarchyPtr projectHierarchy, IHierarchyPtr hierarchy);

    TraversingInstructionMap m_TraversingInstructions;
    vector<CConstClassTypeInfoPtr> m_ClassTypes;
};

SK_LEAVE_NAMESPACE

#endif
#endif
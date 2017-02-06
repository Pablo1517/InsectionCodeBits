#ifndef __TILED_PANEL_H__
#define __TILED_PANEL_H__

#include "Widgets/Widget.h"
#include "Graphics/Mesh/IndexedMesh.h"

SK_ENTER_NAMESPACE

////////////////////////////////////////////////////////////////////////
//////////////////////////// CTiledPanel ///////////////////////////////
////////////////////////////////////////////////////////////////////////

class SPARK_CORE_API CTiledPanel : public CWidget
{
    RTTI_CLASS(CTiledPanel, CWidget);
public:
    CTiledPanel();

    virtual void OnLoad();
    virtual void OnPropertyChange(const CClassField* pField);

protected:

    void InitMesh();
    void RecreateIndexedMesh();
    void RecreateMesh();
    void ShuffleQuads();
    ///Applies progress and returns true if applying progress made changes to visibility  of any tile
    bool ApplyProgress();
    void FullRefresh();

    string m_MeshTexture;
    EGfxAlphaMode::TYPE m_BlendingMode;

    bool m_Wireframe;
    bool m_Randomized;

    vec2i m_Density;
    SIndexedMesh<vertex> m_IndexedMesh;

    IGfxCustom2DPtr m_Mesh;
    float m_AnimationProgress;
    uint m_LastIndexProgres;

    static CStaticFieldPtr s_MeshTextureField;
    static CStaticFieldPtr s_DensityField;
    static CStaticFieldPtr s_ToggleWireframeField;
    static CStaticFieldPtr s_ProgressField;
    static CStaticFieldPtr s_RandomizedField;
    static CStaticFieldPtr s_BlendingModeField;
};

SK_LEAVE_NAMESPACE

#endif // __TILED_PANEL_H__

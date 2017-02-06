#include "stdafx.h"
#include "TiledPanel.h"
#include "SparkStrings.h"

IMPLEMENT_RTTI_CLASS( CTiledPanel );

CStaticFieldPtr CTiledPanel::s_MeshTextureField;
CStaticFieldPtr CTiledPanel::s_DensityField;
CStaticFieldPtr CTiledPanel::s_ToggleWireframeField;
CStaticFieldPtr CTiledPanel::s_ProgressField;
CStaticFieldPtr CTiledPanel::s_RandomizedField;
CStaticFieldPtr CTiledPanel::s_BlendingModeField;

bool CTiledPanel::InitTypeInfo( CClassTypeInfoPtr TypeInfo )
{  
    s_MeshTextureField = DEFINE_FIELD_FILE_DESC("Texture", m_MeshTexture, "", strPropertyFileFilterTexture, "");
    s_BlendingModeField = DEFINE_FIELD_DDL(strPropertyAlphaBlendMode, m_BlendingMode, "", EGfxAlphaMode::CreateDDL);

    s_DensityField = DEFINE_FIELD_DESC("Density", m_Density, "", "This field declares into how many smaller rectangles should the original image be divided");
    s_ProgressField = DEFINE_FIELD_DESC("Animation Progress", m_AnimationProgress, "", "1.0 - every piece is visible, 0.0 - all pieces are invisible.") << RTTI_RANGE(0.0f, 1.0f) << RTTI_PRECISION_STEP(2, 0.01f);
    s_RandomizedField = DEFINE_FIELD_DESC("Randomized", m_Randomized, "", "If this is set, then tiles will disappear in random order; otherwise pieces disappear from the most top left to most bottom by rows.");

    s_ToggleWireframeField = DEFINE_BUTTON("Toggle wireframe", "");
    return true;
}

CTiledPanel::CTiledPanel() :
    m_MeshTexture(""),
    m_BlendingMode(EGfxAlphaMode::Additive),
    m_Wireframe(false),
    m_Randomized(true),
    m_Density(3,3),
    m_AnimationProgress(1.0f),
    m_LastIndexProgres(0)
{
}

void CTiledPanel::OnLoad()
{
    Super::OnLoad();

    if(!GetHierarchy()->IsInEditorMode())
    {
        SetEnabled(false);
        SetNoInput(true);
    }

    InitMesh();
    FullRefresh();
}

void CTiledPanel::OnPropertyChange( const CClassField* pField )
{
    Super::OnPropertyChange(pField);
    
    if (s_MeshTextureField == pField || s_BlendingModeField == pField || s_ColorField == pField)
    {
        InitMesh();
        FullRefresh();
    }
    else if(s_ToggleWireframeField == pField)
    {
        m_Wireframe = !m_Wireframe;
        InitMesh();
        FullRefresh();
    }
    else if (s_ProgressField == pField)
    {
        // Check to avoid recreating mesh in every update when possible
        if (ApplyProgress())
            RecreateMesh();
    }
    else if (s_RandomizedField == pField || s_DensityField == pField || s_WidthField == pField || s_HeightField == pField)
    {
        FullRefresh();
    }
}

void CTiledPanel::InitMesh()
{
    if (!m_Mesh)
        m_Mesh = AddCustom2D();

    m_Mesh->SetSource(m_MeshTexture);
    m_Mesh->SetAlphaMode(m_Wireframe ? EGfxAlphaMode::None : m_BlendingMode);
    m_Mesh->EnableWireframe(m_Wireframe);
    m_Mesh->SetVisible(IsVisible() && GetLayerVisible());
    m_Mesh->SetColor(GetColorAbsolute());
}

void CTiledPanel::RecreateIndexedMesh()
{
    m_IndexedMesh.InitWithRectangularGrid(GetWidth(), GetHeight(), m_Density.x, m_Density.y);

    if (m_Randomized)
        ShuffleQuads();
}

void CTiledPanel::RecreateMesh()
{
    if (!m_Mesh)
        return;

    // If fully hidden then there is no need to recreate mesh, just hide it
    if (m_LastIndexProgres <= 0)
    {
        m_Mesh->SetVisible(false);
        return;
    }

    m_Mesh->SetVisible(IsVisible() && GetLayerVisible());

    // create glow mesh
    {
        // There is no point in calling BeginUpdate since changes here occur only when there is a change in geometry of this gfx (different number of indexes)
        m_Mesh->BeginEdit(EGfxRenderType::TriangleList);

        // Add vertexes
        for (uint i = 0; i < (uint)m_IndexedMesh.vb.size(); ++i)
        {
            m_Mesh->AddPosition(m_IndexedMesh.vb[i].position);
            m_Mesh->AddUV(m_IndexedMesh.vb[i].uv.x, m_IndexedMesh.vb[i].uv.y);
        }

        // Add indexes as quads
        for (uint i = 0; i < (uint)m_IndexedMesh.ib.size()/6; ++i)
        {
            if (i < m_LastIndexProgres)
            {
                m_Mesh->AddIndex(m_IndexedMesh.ib[i * 6]);
                m_Mesh->AddIndex(m_IndexedMesh.ib[i * 6 + 1]);
                m_Mesh->AddIndex(m_IndexedMesh.ib[i * 6 + 2]);
                m_Mesh->AddIndex(m_IndexedMesh.ib[i * 6 + 3]);
                m_Mesh->AddIndex(m_IndexedMesh.ib[i * 6 + 4]);
                m_Mesh->AddIndex(m_IndexedMesh.ib[i * 6 + 5]);
            }
        }

        m_Mesh->EndEdit();
    }
}

void CTiledPanel::ShuffleQuads()
{
    int quadCount = (m_IndexedMesh.GetTrianglesCount() / 2);

    struct int6pack{
        uint16 data[6];
    };

    int6pack* dataBegin = (int6pack*)m_IndexedMesh.ib.data();
    int6pack* dataEnd = ((int6pack*)m_IndexedMesh.ib.data()) + quadCount;

    std::random_shuffle(dataBegin, dataEnd);
}

///<summary>
///Applies progress and returns true if applying progress made changes to visibility  of any tile
///</summary>
bool CTiledPanel::ApplyProgress()
{
    uint progress = sk::math::max(0,sk::math::floor( m_AnimationProgress * (float) (m_IndexedMesh.ib.size()/6) ) );

    if (m_LastIndexProgres != progress)
    {
        m_LastIndexProgres = progress;
        return true;
    }
    
    return false;
}

void CTiledPanel::FullRefresh()
{
    RecreateIndexedMesh();
    ApplyProgress();
    RecreateMesh();
}
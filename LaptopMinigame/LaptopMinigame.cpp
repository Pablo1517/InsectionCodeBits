#include "stdafx.h"
#include "SparkStrings.h"
#include "Minigames/LaptopMinigame/LaptopMinigame.h" 
#include "SchemaAndProperty/DropDownList.h"
#include "Objects/Scenario.h"
#include "Objects/Track.h"
#include "Utilities/DebugShapes.h"


//******************************************************************************
//      CLaptopMinigame
//******************************************************************************

// Strings
const char* strLaptopMGField_ZoomingTime = "Zooming Time";
const char* strLaptopMGField_ZoomContainer = "Zoom Levels Container";
const char* strLaptopMGField_ZoomOutButton = "Zoom Out Button";
const char* strLaptopMGField_ImgDivision = "Image Division";
const char* strLaptopMGField_ImgContainer = "Image Container";
const char* strLaptopMGField_TransitionEff = "Transition Effect Forward";
const char* strLaptopMGField_TransitionEff2 = "Transition Effect Backward";
const char* strLaptopMGField_Highlither = "Highlight widget";
const char* strLaptopMGField_ResizeHL = "Resize Highlight widget";
const char* strLaptopMGField_ActiveAreas = "Active Areas";

// triggers
const char* strLaptopMGTrig_SwitchFinished = "SwitchFinished";

// Events
const char* strLaptopMGTrig_ZoomInStarted = "OnZoomInStarted";
const char* strLaptopMGTrig_ZoomOutStarted = "OnZoomOutStarted";
const char* strLaptopMGTrig_ZoomLvlEnabled = "OnZoomLevelEnabled";

// STATIC FIELDS
CStaticFieldPtr CLaptopMinigame::s_ZoomLevelsField;

IMPLEMENT_RTTI_CLASS( CLaptopMinigame );

CLaptopMinigame::CLaptopMinigame() :
m_ZoomLevelIndex(0),
m_ZoomingTime(1.0f),
m_ResizeHighligher(false)
{
    
}

bool CLaptopMinigame::InitTypeInfo( CClassTypeInfoPtr TypeInfo )
{
    OPEN_CATEGORY("Logic");

    DEFINE_FIELD_DESC(strLaptopMGField_ZoomingTime, m_ZoomingTime, "", "How long should the zooming effect last.");
    DEFINE_FIELD_DESC(strLaptopMGField_ZoomOutButton, m_ZoomOutButton, "", "Button that will zoom out the image on laptop screen.");
    DEFINE_FIELD_DESC(strLaptopMGField_ZoomContainer, m_ZoomLevelsContainer, "", sk::Util::Format("Widget containing all the image containers. For more info look into LaptopMGZoomLevel field \"%s\".", strLaptopMGField_ImgContainer));
    DEFINE_FIELD_DESC(strLaptopMGField_Highlither, m_HighlightWidget, "", "Widget used to highlight elements of screen under mouse cursor.");
    DEFINE_FIELD_DESC(strLaptopMGField_ResizeHL, m_ResizeHighligher, "", "If set, minigame will resize highlight widget to match each area pointed by mouse cursor.");
    s_ZoomLevelsField = DEFINE_CHILD_LIST_DESC("Zoom Levels", "", CreateClassFilterDDL<CLaptopMGZoomLevel>, "List of zoom levels that are possible on the \"Laptop\" screen. Order is important.");

    // Events
    DEFINE_EVENT(strLaptopMGTrig_SwitchFinished);
    DEFINE_EVENT(strLaptopMGTrig_ZoomInStarted);
    DEFINE_EVENT(strLaptopMGTrig_ZoomOutStarted);

    // Triggers
    DEFINE_SIMPLE_TRIGGER(strLaptopMGTrig_SwitchFinished);

    CLOSE_CATEGORY();

    DEFINE_SIMPLE_FUNCTION(ZoomingFinished);
    DEFINE_SIMPLE_FUNCTION(SwitchFinished);
    DEFINE_SIMPLE_FUNCTION(PressedZoomOutButton);

    // Hidden
    DEFINE_FIELD_HIDDEN("CurrentZoomLevelIndex", m_ZoomLevelIndex, "");

    return true;
}

void CLaptopMinigame::OnCreate(bool loading)
{
    Super::OnCreate(loading);

    if (!m_DebugShapes)
    {
        m_DebugShapes = CDebugShapes::Create(GetSelf());
    }

    m_MinigameMode = ELaptopMGState::Idle;
}

void CLaptopMinigame::PreRender()
{
    SUPER::PreRender();

    IGfxRendererPtr pRenderer = (_CUBE())->GetRenderer();
    if (!pRenderer)
        return;

    // In Editor Only
    if (IsInEditorMode())
    {

        // Draw grid
        if (m_CurrentZoomLevel && m_MinigameMode == ELaptopMGState::Idle)
        {
            vec2i imgDiv = m_CurrentZoomLevel->GetImageDivision();
            float width = GetWidth() / imgDiv.x;
            float height = GetHeight() / imgDiv.y;
            vec2 pos1, pos2;
            for (int i = 1; i < imgDiv.x; ++i)
            {
                pos1.x = width * i;
                pos1.y = 0.0f;
                pos2.x = pos1.x;
                pos2.y = GetHeight();
                m_DebugShapes->Line(pos1, pos2, color::GREEN);
            }

            for (int i = 1; i < imgDiv.y; ++i)
            {
                pos1.x = 0.0f;
                pos1.y = height * i;
                pos2.x = GetWidth();
                pos2.y = pos1.y;
                m_DebugShapes->Line(pos1, pos2, color::GREEN);
            }
        }

    }
}

void CLaptopMinigame::OnLoad()
{
    Super::OnLoad();

    GatherZoomLevels();
    RefreshActiveZones();

    if (!IsInEditorMode())
    {
        // Only in game
        if (CWidgetPtr zoomOutButton = m_ZoomOutButton.lock())
            zoomOutButton->ConnectUniqueTriggerTo("OnClick", GetSelf(), "PressedZoomOutButton");

        for (uint i = 0; i < m_ZoomLevels.size(); ++i)
        {
            if (CLaptopMGZoomLevelPtr zoomLevel = m_ZoomLevels[i])
                zoomLevel->SetMinigame(GetSelf());
        }
    }
    
}

void CLaptopMinigame::StartGame()
{
    Super::StartGame();

    // Initialize first zoom level with data required to get back to it later (to zoom out to the first zoom level)
    CLaptopMGZoomLevelPtr zoomLevel = GetCurrentZoomLevel();
    CWidgetPtr zoomLevelsContainer = m_ZoomLevelsContainer.lock();
    if (zoomLevel && zoomLevelsContainer)
    {
        vec2 newScale = zoomLevelsContainer->GetScale();
        vec2 pos = zoomLevelsContainer->GetPosition();
        zoomLevel->SetZoomScale(newScale);
        zoomLevel->SetZoomImgPosition(pos);
    }

    SetMinigameInput(true);
}

void CLaptopMinigame::SkipGame()
{
    Super::SkipGame();
}

void CLaptopMinigame::FinishGame()
{
    SUPER::FinishGame();
    SetMinigameInput(false);
}

void CLaptopMinigame::FastForward()
{
    Super::FastForward();
}

bool CLaptopMinigame::IsFastForwardRequired(float& endTime)
{
    if(IsFastForwardRequiredLocal(endTime))
        return true;
    return Super::IsFastForwardRequired(endTime);
}
bool CLaptopMinigame::IsFastForwardRequiredLocal(float& endTime)
{
    return false;
}

void CLaptopMinigame::GetExpectedGestures(SetOfGestureTypes& gesturesSet)
{
    Super::GetExpectedGestures(gesturesSet);
    gesturesSet.insert(EGestureType::Tap);
}

void CLaptopMinigame::Click(EClickType::TYPE clickType, const vec2& position)
{
    Super::Click(clickType, position);

    RectPtr clickedRect;
    vec2 localPos = PointWorldToLocal(position);
    // Find clicked rect
    for (uint i = 0; i < m_ActiveZones.size(); ++i)
    {
        if (m_ActiveZones[i]->Intersect(localPos))
        {
            clickedRect = m_ActiveZones[i];
            break;
        }
    }

    if (!clickedRect)
    {
        // This shouldn't happen unless LD made some terrible hack, give him a medal.
        LOG_ERROR(0, "LaptopMinigame recieved a click event but couldn't match it to any active zone on the screen!");
        return;
    }

    SwitchToNextZoomLevel(clickedRect);
}

void CLaptopMinigame::MouseMoveOver(const vec2& mousePosition)
{
    Super::MouseMoveOver(mousePosition);

    vec2 localPos = PointWorldToLocal(mousePosition);
    RectPtr overRect;
    if (m_MinigameMode == ELaptopMGState::Idle)
    {
        // Find clicked rect
        for (uint i = 0; i < m_ActiveZones.size(); ++i)
        {
            if (m_ActiveZones[i]->Intersect(localPos))
            {
                overRect = m_ActiveZones[i];
                break;
            }
        }
    }

    HighlightRect(overRect);
}

void CLaptopMinigame::MouseLeave(CWidgetPtr nextWidget, const vec2& mousePos)
{
    Super::MouseLeave(nextWidget, mousePos);

    HighlightRect(RectPtr());
}


uint CLaptopMinigame::GetCurrentZoomLevelIndex()
{
    return m_ZoomLevelIndex;
}

CLaptopMGZoomLevelPtr CLaptopMinigame::GetCurrentZoomLevel()
{
    return m_CurrentZoomLevel;
}

void CLaptopMinigame::CheckIfFinished()
{
    CLaptopMGZoomLevelPtr zoomLevel;
    for (uint i = 0; i < m_ZoomLevels.size(); ++i)
    {
        zoomLevel = m_ZoomLevels[i];
        if (!zoomLevel->IsClear())
            return;
    }

    FinishGame();
}

void CLaptopMinigame::SetMinigameInput(bool on)
{
    CWidgetPtr zoomOutButton = m_ZoomOutButton.lock();
    if (zoomOutButton)
        zoomOutButton->SetNoInput(!on);

    SetNoInput(!on);
    if (m_CurrentZoomLevel)
        m_CurrentZoomLevel->EnableZoomLevel(on);
    HighlightRect(RectPtr());
}

CLaptopMGZoomLevelPtr CLaptopMinigame::GetNextZoomLevel()
{
    if ((m_ZoomLevelIndex + 1) >= m_ZoomLevels.size())
        return CLaptopMGZoomLevelPtr();

    return m_ZoomLevels[m_ZoomLevelIndex + 1];
}

CLaptopMGZoomLevelPtr CLaptopMinigame::GetPreviousZoomLevel()
{
    if ((int)m_ZoomLevelIndex - 1 < 0)
        return CLaptopMGZoomLevelPtr();

    return m_ZoomLevels[m_ZoomLevelIndex - 1];
}

void CLaptopMinigame::SwitchToNextZoomLevel(RectPtr zoneRect)
{
    // Can't zoom in any further
    if (!GetNextZoomLevel())
        return;

    m_MinigameMode = ELaptopMGState::ZoomingIn;
    SetMinigameInput(false);
    ZoomToRect(zoneRect);
}

void CLaptopMinigame::SwitchToPreviousZoomLevel()
{
    // Can't zoom in any further
    if (!GetPreviousZoomLevel())
        return;

    m_MinigameMode = ELaptopMGState::ZoomingOut;
    SetMinigameInput(false);
    // This function zooms out to previous ZoomLevel
    ZoomOut();
}

void CLaptopMinigame::GatherZoomLevels()
{
    m_ZoomLevels.clear();
    GetChildList<CLaptopMGZoomLevel>(m_ZoomLevels, s_ZoomLevelsField.lock());
}

void CLaptopMinigame::RefreshActiveZones()
{
    if (m_ZoomLevels.empty())
        return;
    m_CurrentZoomLevel = m_ZoomLevels[m_ZoomLevelIndex];
    if (!m_CurrentZoomLevel)
        return;
    
    m_ActiveZones.clear();
    vec2i imgDiv = m_CurrentZoomLevel->GetImageDivision();
    uint rectCount = (uint) imgDiv.x * imgDiv.y;
    float width = GetWidth() / imgDiv.x;
    float height = GetHeight() / imgDiv.y;
    for (int y = 0; y < imgDiv.y; ++y)
    {
        for (int x = 0; x < imgDiv.x; ++x)
        {
            RectPtr newRect(new rectangle());
            newRect->left = width * x;
            newRect->top = height * y;
            newRect->SetHeight(height);
            newRect->SetWidth(width);
            m_ActiveZones.push_back(newRect);
        }
    }

    m_CurrentZoomLevel->EnableZoomLevel(true);
}

void CLaptopMinigame::ZoomToRect(RectPtr rect)
{
    CWidgetPtr zoomLevelsContainer = m_ZoomLevelsContainer.lock();
    if (!zoomLevelsContainer)
        return;

    CLaptopMGZoomLevelPtr currentZoomLevel = GetCurrentZoomLevel();
    if (!currentZoomLevel)
        return;

    vec2 pos1 = currentZoomLevel->GetZoomImgPosition(); 
    vec2 pos2 = -rect->GetTopLeft();

    float newXScale = GetWidth() / rect->GetWidth();
    float newYScale = GetHeight() / rect->GetHeight();

    vec2 newScale(newXScale, newYScale);
    
    // Include any previous zooming into this zoom
    vec2 offset(pos1);
    offset.x *= newScale.x;
    offset.y *= newScale.y;

    pos2.x *= newScale.x;
    pos2.y *= newScale.y;
    pos2 += offset;

    vec2 prevScale(currentZoomLevel->GetZoomScale());
    newScale.x *= prevScale.x;
    newScale.y *= prevScale.y;

    // Set up next zooming level so that it remembers data required to get back to it later (when zooming out to this NEXT zooming level)
    if (CLaptopMGZoomLevelPtr nextZoomLevel = GetNextZoomLevel())
    {
        nextZoomLevel->SetZoomScale(newScale);
        nextZoomLevel->SetZoomImgPosition(pos2);
    }

    // Zooming effect
    zoomLevelsContainer->SetScale(currentZoomLevel->GetZoomScale());
    zoomLevelsContainer->ScaleTo(newScale, m_ZoomingTime);
    zoomLevelsContainer->FlyTo(pos1, pos2, m_ZoomingTime, false);

    // Transition effect
    CScenarioPtr scenario = currentZoomLevel->GetTransitionDisappearScenario();
    if (scenario)
        scenario->Play();
    else
        LOG_WARNING(0, "LaptopMGZoomLevel (%s) - No scenario for %s field was found! There will be no transition effect!", currentZoomLevel->GetName().c_str(), strLaptopMGField_TransitionEff);

    ConnectSwitchingEvents(scenario, zoomLevelsContainer);
    FireActions(strLaptopMGTrig_ZoomInStarted);
}

void CLaptopMinigame::ZoomOut()
{
    CWidgetPtr zoomLevelsContainer = m_ZoomLevelsContainer.lock();
    if (!zoomLevelsContainer)
        return;

    CLaptopMGZoomLevelPtr prevZoomLevel = GetPreviousZoomLevel();
    if (!prevZoomLevel)
        return;

    // Get data needed to zoom back to this previous zoom level
    vec2 newScale = prevZoomLevel->GetZoomScale();
    vec2 pos1 = PointWorldToLocal(zoomLevelsContainer->GetPositionAbsolute());
    vec2 pos2 = prevZoomLevel->GetZoomImgPosition();

    // Zooming Effect
    if (CLaptopMGZoomLevelPtr currentZoomLevel = GetCurrentZoomLevel())
        zoomLevelsContainer->SetScale(currentZoomLevel->GetZoomScale());

    zoomLevelsContainer->ScaleTo(newScale, m_ZoomingTime);
    zoomLevelsContainer->FlyTo(pos1, pos2, m_ZoomingTime, false);

    // Transition effect
    CScenarioPtr scenario = prevZoomLevel->GetTransitionAppearScenario();
    if (scenario)
        scenario->Play();
    else
        LOG_WARNING(0, "LaptopMGZoomLevel (%s) - No scenario for %s field was found! There will be no transition effect!", prevZoomLevel->GetName().c_str(), strLaptopMGField_TransitionEff);

    ConnectSwitchingEvents(scenario, zoomLevelsContainer);
    FireActions(strLaptopMGTrig_ZoomOutStarted);
}

void CLaptopMinigame::ZoomingFinished()
{
    // Send Events to LD
    CWidgetPtr zoomLevelsContainer = m_ZoomLevelsContainer.lock();
    if (zoomLevelsContainer)
        zoomLevelsContainer->DisconnectEvent("OnFlightFinished", GetSelf(), "ZoomingFinished");
}

void CLaptopMinigame::SwitchFinished()
{
    DisconnectSwitchingEvents();

    CLaptopMGZoomLevelPtr zoomLevel;
    switch(m_MinigameMode)
    {
    case ELaptopMGState::ZoomingIn:
        zoomLevel = GetNextZoomLevel();
        if (zoomLevel)
            m_ZoomLevelIndex++;
    break;

    case ELaptopMGState::ZoomingOut:
        zoomLevel = GetPreviousZoomLevel();
        if (zoomLevel)
            m_ZoomLevelIndex--;
    break;
    }

    m_MinigameMode = ELaptopMGState::Idle;
    FireActions(strLaptopMGTrig_SwitchFinished);
    CallTrigger(strLaptopMGTrig_SwitchFinished);
    // m_CurrentZoomLevel is set in RefreshActiveZones();
    RefreshActiveZones();
    SetMinigameInput(true);
}

void CLaptopMinigame::PressedZoomOutButton()
{
    SwitchToPreviousZoomLevel();
}

void CLaptopMinigame::ConnectSwitchingEvents(CScenarioPtr scenario, CWidgetPtr zoomsContainer)
{
    // Connect ZoomingFinished to OnFlightEnd so we can send event for LD
    zoomsContainer->ConnectUniqueTriggerTo("OnFlightFinished", GetSelf(), "ZoomingFinished");

    float scenarioTime = 0.0f;
    if (scenario)
    {
        m_CurrentTransitionScenario = scenario;
        scenarioTime = scenario->GetTotalScenarioTime();
    }
    // Connect SwitchingFinished to correct event to notice the actual end of switch (if scenario is longer than zooming then connect to scenario OnEnd)
    if (scenarioTime > m_ZoomingTime)
        scenario->ConnectUniqueTriggerTo("OnEnd", GetSelf(), "SwitchFinished");
    else
        zoomsContainer->ConnectUniqueTriggerTo("OnFlightFinished", GetSelf(), "SwitchFinished");
}

void CLaptopMinigame::DisconnectSwitchingEvents()
{
    CLaptopMGZoomLevelPtr currentZoomLevel = GetCurrentZoomLevel();
    if (currentZoomLevel)
    {
        if (m_CurrentTransitionScenario)
        {
            m_CurrentTransitionScenario->DisconnectEvent("OnEnd", GetSelf(), "SwitchFinished");
            m_CurrentTransitionScenario.reset();
        }
    }
    CWidgetPtr zoomLevelsContainer = m_ZoomLevelsContainer.lock();
    if (zoomLevelsContainer)
        zoomLevelsContainer->DisconnectEvent("OnFlightFinished", GetSelf(), "SwitchFinished");
}

void CLaptopMinigame::HighlightRect(RectPtr rect)
{
    CWidgetPtr widget = m_HighlightWidget.lock();
    if (!widget)
        return;

    if (!rect)
    {
        widget->SetVisible(false);
        return;
    }

    widget->SetPosition(rect->GetTopLeft());
    if (m_ResizeHighligher)
    {
        widget->SetWidth(rect->GetWidth());
        widget->SetHeight(rect->GetHeight());
    }
    widget->SetVisible(true);
}

//******************************************************************************
//      CLaptopMGElement
//******************************************************************************

IMPLEMENT_RTTI_CLASS(CLaptopMGElement);

bool CLaptopMGElement::InitTypeInfo(CClassTypeInfoPtr TypeInfo)
{
    DEFINE_FIELD_HIDDEN("IsFound", m_Found, "");

    DEFINE_EVENT("OnFound");

    return true;
}

CLaptopMGElement::CLaptopMGElement()
{
    m_Found = false;
}

void CLaptopMGElement::Click(EClickType::TYPE clickType, const vec2& position)
{
    Super::Click(clickType, position);

    m_Found = true;
    SetNoInput(true);
    if (m_Minigame)
        m_Minigame->CheckIfFinished();

    FireActions("OnFound");
}

bool CLaptopMGElement::IsFound()
{
    return m_Found;
}

void CLaptopMGElement::SetMinigame(CLaptopMinigamePtr minigame)
{
    m_Minigame = minigame;
}

//******************************************************************************
//      CLaptopMGZoomLevel
//******************************************************************************

IMPLEMENT_RTTI_CLASS_EX(CLaptopMGZoomLevel, eClassFlags::CF_HIDDEN);

bool CLaptopMGZoomLevel::InitTypeInfo(CClassTypeInfoPtr TypeInfo)
{
    DEFINE_FIELD_DESC(strLaptopMGField_ImgDivision, m_ImgDivision, "", "This field determines how image on the screen is being divaded where \"x\" value says how many lines divide the image horizontally and \"y\" vertically.") << RTTI_PRECISION(1);
    DEFINE_FIELD_DESC(strLaptopMGField_ImgContainer, m_ImageContainer, "", "Widget containing graphical representation of this zoom level. If it's a single image it can also be a Panel");
    DEFINE_FIELD_DESC(strLaptopMGField_TransitionEff, m_TransitionDisappearScenario, "", "Scenario that will play transition effect when zooming from THIS zoom level to next one.");
    DEFINE_FIELD_DESC(strLaptopMGField_TransitionEff2, m_TransitionAppearScenario, "", "Scenario that will play transition effect when switchig from next zoom level to THIS one.");
    DEFINE_FIELD_DESC(strLaptopMGField_ActiveAreas, m_ActiveAreas, "", "Active areas that player has to find on this zoom level.");

    // Events
    DEFINE_EVENT(strLaptopMGTrig_ZoomLvlEnabled);

    // Hidden
    DEFINE_FIELD_HIDDEN("ZoomScale", m_ZoomScale, "");
    DEFINE_FIELD_HIDDEN("ZoomImgPosition", m_ZoomImgPosition, "");

    return true;
}

CLaptopMGZoomLevel::CLaptopMGZoomLevel()
{

}

CWidgetPtr CLaptopMGZoomLevel::GetImageContainer()
{
    return m_ImageContainer.lock();
}

const vec2& CLaptopMGZoomLevel::GetImageDivision()
{
    return m_ImgDivision;
}

CScenarioPtr CLaptopMGZoomLevel::GetTransitionDisappearScenario()
{
    return m_TransitionDisappearScenario.lock();
}

CScenarioPtr CLaptopMGZoomLevel::GetTransitionAppearScenario()
{
    return m_TransitionAppearScenario.lock();
}

const vec2& CLaptopMGZoomLevel::GetZoomScale()
{
    return m_ZoomScale;
}

const vec2& CLaptopMGZoomLevel::GetZoomImgPosition()
{
    return m_ZoomImgPosition;
}

void CLaptopMGZoomLevel::SetZoomScale(vec2& zoomScale)
{
    m_ZoomScale = zoomScale;
}

void CLaptopMGZoomLevel::SetZoomImgPosition(vec2& pos)
{
    m_ZoomImgPosition = pos;
}

void CLaptopMGZoomLevel::EnableZoomLevel(bool on)
{
    CLaptopMinigamePtr minigame = FindParentOfType<CLaptopMinigame>();
    if (!minigame)
        return;
    CLaptopMGElementPtr element;
    for (uint i = 0; i < m_ActiveAreas.size(); ++i)
    {
        element = m_ActiveAreas[i].lock();
        if (!element || element->IsFound() || !element->Intersect(minigame))
            continue;

        element->SetNoInput(!on);
    }

    FireActions(strLaptopMGTrig_ZoomLvlEnabled);
}

bool CLaptopMGZoomLevel::IsClear()
{
    CLaptopMGElementPtr element;
    for (uint i = 0; i < m_ActiveAreas.size(); ++i)
    {
        element = m_ActiveAreas[i].lock();
        if (element && !element->IsFound())
            return false;
    }

    return true;
}

void CLaptopMGZoomLevel::SetMinigame(CLaptopMinigamePtr minigame)
{
    CLaptopMGElementPtr element;
    for (uint i = 0; i < m_ActiveAreas.size(); ++i)
    {
        element = m_ActiveAreas[i].lock();
        if (!element)
            continue;

        element->SetMinigame(minigame);
    }
}
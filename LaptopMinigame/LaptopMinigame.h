#ifndef _LAPTOP_MINIGAME_H
#define _LAPTOP_MINIGAME_H

#include "Minigames/Core/BaseMinigame.h"

SK_ENTER_NAMESPACE

SPARK_DECLARE_POINTERS(CLaptopMinigame);
typedef reference_ptr<CLaptopMinigame> CLaptopMinigameRefPtr;

SPARK_DECLARE_POINTERS(CLaptopMGElement);
typedef reference_ptr<CLaptopMGElement> CLaptopMGElementRefPtr;

SPARK_DECLARE_POINTERS(CLaptopMGZoomLevel);
typedef reference_ptr<CLaptopMGZoomLevel> CLaptopMGZoomLevelRefPtr;

typedef shared_ptr<rectangle> RectPtr;

ENUM_DECLARATION_BEGIN(ELaptopMGState)
Idle,
ZoomingIn,
ZoomingOut,
Transition,
ENUM_DECLARATION_MIDDLE()
ENUM_DECLARATION_END();

//******************************************************************************
//      CLaptopMinigame
//******************************************************************************

class CLaptopMinigame : public CBaseMinigame
{
    RTTI_CLASS( CLaptopMinigame , CBaseMinigame );
public:

    CLaptopMinigame();

    virtual void OnCreate(bool loading);
    virtual void PreRender();
    virtual void OnLoad();
    virtual void StartGame();
    virtual void SkipGame();
    virtual void FinishGame();
    virtual void FastForward();
    virtual bool IsFastForwardRequired(float& endTime);
    bool IsFastForwardRequiredLocal(float& endTime);

    virtual void GetExpectedGestures(SetOfGestureTypes& gesturesSet);
    virtual void Click(EClickType::TYPE clickType, const vec2& position);
    virtual void MouseMoveOver(const vec2& mousePos);
    virtual void MouseLeave(CWidgetPtr nextWidget, const vec2& mousePos);
    
    uint GetCurrentZoomLevelIndex();
    CLaptopMGZoomLevelPtr GetCurrentZoomLevel();
    
    void CheckIfFinished();
protected:

    void SetMinigameInput(bool on);

    CLaptopMGZoomLevelPtr GetNextZoomLevel();
    CLaptopMGZoomLevelPtr GetPreviousZoomLevel();

    void SwitchToNextZoomLevel(RectPtr zoneRect);
    void SwitchToPreviousZoomLevel();

    static CStaticFieldPtr s_ZoomLevelsField;
    void GatherZoomLevels();
    void RefreshActiveZones();
    void ZoomToRect(RectPtr rect);
    void ZoomOut();
    void ZoomingFinished();
    void SwitchFinished();
    void PressedZoomOutButton();
    void ConnectSwitchingEvents(CScenarioPtr scenario, CWidgetPtr zoomsContainer);
    void DisconnectSwitchingEvents();

    void HighlightRect(RectPtr rect);

    CWidgetRefPtr m_ZoomLevelsContainer;
    vector<RectPtr> m_ActiveZones;
    vector<CLaptopMGZoomLevelPtr> m_ZoomLevels;
    CWidgetRefPtr m_ZoomOutButton;
    uint m_ZoomLevelIndex;
    CLaptopMGZoomLevelPtr m_CurrentZoomLevel;
    ELaptopMGState::TYPE m_MinigameMode;

    CDebugShapesPtr m_DebugShapes;

    float m_ZoomingTime;
    CScenarioPtr m_CurrentTransitionScenario;
    CWidgetRefPtr m_HighlightWidget;

    bool m_ResizeHighligher;
};

//******************************************************************************
//      CLaptopMGElement
//******************************************************************************

class CLaptopMGElement : public CWidget
{
    RTTI_CLASS(CLaptopMGElement, CWidget);

public:
    CLaptopMGElement();

    virtual void Click(EClickType::TYPE clickType, const vec2& position);

    bool IsFound();
    
    void SetMinigame(CLaptopMinigamePtr minigame);
protected:

    bool m_Found;
    CLaptopMinigamePtr m_Minigame;
};

//******************************************************************************
//      CLaptopMGZoomLevel
//******************************************************************************

class CLaptopMGZoomLevel : public CHierarchyObject
{
    RTTI_CLASS(CLaptopMGZoomLevel, CHierarchyObject);

public:
    CLaptopMGZoomLevel();

    CWidgetPtr GetImageContainer();
    const vec2& GetImageDivision();
    // This scenario will be played when player zooms into another zoom level from THIS zoom level
    CScenarioPtr GetTransitionDisappearScenario();
    // This scenario will be played when player zooms out from another zoom level to THIS zoom level
    CScenarioPtr GetTransitionAppearScenario();
    const vec2& GetZoomScale();
    const vec2& GetZoomImgPosition();

    void SetZoomScale(vec2& zoomScale);
    void SetZoomImgPosition(vec2& pos);

    void EnableZoomLevel(bool on);
    bool IsClear();
    void SetMinigame(CLaptopMinigamePtr minigame);
protected:

    vec2 m_ImgDivision;
    CWidgetRefPtr m_ImageContainer;
    // More info about scenarios at getter functions
    CScenarioRefPtr m_TransitionDisappearScenario;
    CScenarioRefPtr m_TransitionAppearScenario;
    vec2 m_ZoomScale;
    vec2 m_ZoomImgPosition;
    vector<CLaptopMGElementRefPtr> m_ActiveAreas;
};

SK_LEAVE_NAMESPACE
#endif
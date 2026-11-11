//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Modern main-menu cards and the small tween helper used by the
//          console open/close transitions in CBasePanel.
//
//=============================================================================//

#ifndef MODERNMENU_H
#define MODERNMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/EditablePanel.h"
#include "vgui/VGUI.h"
#include "vgui/IScheme.h"
#include "Color.h"

//-----------------------------------------------------------------------------
// Purpose: Simple time based tween. Animates a value from its current value
//          to a target value with an ease-out curve.
//-----------------------------------------------------------------------------
class CModernMenuTween
{
public:
	CModernMenuTween();

	// Starts animating toward flTarget.
	void Start( float flNow, float flDuration, float flDelay, float flTarget );

	// Stops immediately and forces a value.
	void Stop( float flValue );

	bool IsActive() const { return m_bActive; }
	float GetValue() const { return m_flValue; }

	// Advances the animation and returns the current value.
	float Tick( float flNow );

private:
	float m_flValue;
	float m_flStart;
	float m_flTarget;
	float m_flStartTime;
	float m_flDuration;
	float m_flDelay;
	bool  m_bActive;
};

//-----------------------------------------------------------------------------
// Purpose: A single translucent information card (news / player profile).
//          Value 1 = fully visible, 0 = slid out to the right and transparent.
//-----------------------------------------------------------------------------
class CModernMenuCard : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CModernMenuCard, vgui::EditablePanel );

public:
	CModernMenuCard( vgui::Panel *parent, const char *name );
	virtual ~CModernMenuCard();

	void SetCardTitle( const char *pszTitle );
	void SetCardSubtitle( const char *pszSubtitle );
	void SetBodyLine( int iIndex, const char *pszText );

	void SetBasePos( int x, int y );
	void SetSlideDistance( int nPixels ) { m_iSlideDistance = nPixels; }

	// Animates to the requested state.
	void AnimateTo( bool bVisible, float flNow, float flDuration = 0.18f, float flDelay = 0.0f );

	// Jumps to the requested state without animating.
	void SnapTo( bool bVisible );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void OnThink();
	virtual void Paint();

private:
	void UpdatePosition();

	CModernMenuTween m_Tween;

	int m_iBaseX;
	int m_iBaseY;
	int m_iSlideDistance;

	int m_iBodyLineCount;
	Color m_AccentColor;
	Color m_BgColor;
	Color m_TitleColor;
	Color m_TextColor;

	vgui::HFont m_hTitleFont;
	vgui::HFont m_hSubtitleFont;
	vgui::HFont m_hBodyFont;

	wchar_t m_wszTitle[128];
	wchar_t m_wszSubtitle[128];
	wchar_t m_wszBody[3][256];
};

#endif // MODERNMENU_H

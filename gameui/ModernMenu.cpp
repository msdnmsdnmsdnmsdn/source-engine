//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Modern main-menu cards and the small tween helper used by the
//          console open/close transitions in CBasePanel.
//
//=============================================================================//

#include "ModernMenu.h"

#include "EngineInterface.h"

#include "tier3/tier3.h"
#include "tier1/strtools.h"

#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Controls.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Writes a possibly localized string into a wide buffer.
//-----------------------------------------------------------------------------
static void SetLocalizedWide( const char *pszText, wchar_t *pOut, int nOutBytes )
{
	if ( !pOut || nOutBytes <= 0 )
		return;

	if ( !pszText || !pszText[0] )
	{
		pOut[0] = 0;
		return;
	}

	if ( pszText[0] == '#' && g_pVGuiLocalize )
	{
		wchar_t *pLocalized = g_pVGuiLocalize->Find( pszText );
		if ( pLocalized )
		{
			V_wcsncpy( pOut, pLocalized, nOutBytes );
			return;
		}
	}

	ILocalize::ConvertANSIToUnicode( pszText, pOut, nOutBytes );
}

//-----------------------------------------------------------------------------
// Purpose: Draws one line of text.
//-----------------------------------------------------------------------------
static void DrawCardText( vgui::ISurface *pSurface, vgui::HFont hFont, Color col, int x, int y, const wchar_t *pText )
{
	if ( !pSurface || !hFont || !pText || !pText[0] )
		return;

	pSurface->DrawSetTextFont( hFont );
	pSurface->DrawSetTextColor( col );
	pSurface->DrawSetTextPos( x, y );
	pSurface->DrawPrintText( pText, V_wcslen( pText ) );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CModernMenuTween::CModernMenuTween()
{
	m_flValue = 0.0f;
	m_flStart = 0.0f;
	m_flTarget = 0.0f;
	m_flStartTime = 0.0f;
	m_flDuration = 0.0f;
	m_flDelay = 0.0f;
	m_bActive = false;
}

//-----------------------------------------------------------------------------
// Purpose: Starts animating from the current value toward flTarget.
//-----------------------------------------------------------------------------
void CModernMenuTween::Start( float flNow, float flDuration, float flDelay, float flTarget )
{
	m_flStart = m_flValue;
	m_flTarget = flTarget;
	m_flStartTime = flNow;
	m_flDuration = flDuration;
	m_flDelay = flDelay;
	m_bActive = true;
}

//-----------------------------------------------------------------------------
// Purpose: Stops the animation and forces a value.
//-----------------------------------------------------------------------------
void CModernMenuTween::Stop( float flValue )
{
	m_flValue = flValue;
	m_flStart = flValue;
	m_flTarget = flValue;
	m_bActive = false;
}

//-----------------------------------------------------------------------------
// Purpose: Advances the animation.
//-----------------------------------------------------------------------------
float CModernMenuTween::Tick( float flNow )
{
	if ( !m_bActive )
		return m_flValue;

	float flT = flNow - m_flStartTime - m_flDelay;
	if ( flT <= 0.0f )
		return m_flValue;

	if ( m_flDuration <= 0.0f || flT >= m_flDuration )
	{
		m_flValue = m_flTarget;
		m_bActive = false;
		return m_flValue;
	}

	float flP = flT / m_flDuration;

	// ease-out cubic
	flP = 1.0f - ( 1.0f - flP ) * ( 1.0f - flP ) * ( 1.0f - flP );

	m_flValue = m_flStart + ( m_flTarget - m_flStart ) * flP;
	return m_flValue;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CModernMenuCard::CModernMenuCard( vgui::Panel *parent, const char *name )
	: BaseClass( parent, name )
{
	m_iBaseX = 0;
	m_iBaseY = 0;
	m_iSlideDistance = 24;

	m_iBodyLineCount = 0;
	m_AccentColor = Color( 77, 216, 232, 255 );
	m_BgColor = Color( 22, 22, 28, 217 );
	m_TitleColor = Color( 242, 242, 245, 255 );
	m_TextColor = Color( 160, 160, 172, 255 );

	m_hTitleFont = 0;
	m_hSubtitleFont = 0;
	m_hBodyFont = 0;

	m_wszTitle[0] = 0;
	m_wszSubtitle[0] = 0;
	for ( int i = 0; i < 3; ++i )
	{
		m_wszBody[i][0] = 0;
	}

	SetPaintBackgroundEnabled( false );
	SetMouseInputEnabled( false );
	SetVisible( false );
	SetAlpha( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CModernMenuCard::~CModernMenuCard()
{
}

//-----------------------------------------------------------------------------
// Purpose: Sets the card title.
//-----------------------------------------------------------------------------
void CModernMenuCard::SetCardTitle( const char *pszTitle )
{
	SetLocalizedWide( pszTitle, m_wszTitle, sizeof( m_wszTitle ) );
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the card subtitle.
//-----------------------------------------------------------------------------
void CModernMenuCard::SetCardSubtitle( const char *pszSubtitle )
{
	SetLocalizedWide( pszSubtitle, m_wszSubtitle, sizeof( m_wszSubtitle ) );
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Sets one body line (max 3).
//-----------------------------------------------------------------------------
void CModernMenuCard::SetBodyLine( int iIndex, const char *pszText )
{
	if ( iIndex < 0 || iIndex >= 3 )
		return;

	SetLocalizedWide( pszText, m_wszBody[iIndex], sizeof( m_wszBody[iIndex] ) );

	if ( iIndex >= m_iBodyLineCount )
	{
		m_iBodyLineCount = iIndex + 1;
	}

	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the position the card returns to when fully visible.
//-----------------------------------------------------------------------------
void CModernMenuCard::SetBasePos( int x, int y )
{
	m_iBaseX = x;
	m_iBaseY = y;
	UpdatePosition();
}

//-----------------------------------------------------------------------------
// Purpose: Animates to visible/hidden.
//-----------------------------------------------------------------------------
void CModernMenuCard::AnimateTo( bool bVisible, float flNow, float flDuration, float flDelay )
{
	float flTarget = bVisible ? 1.0f : 0.0f;

	if ( !bVisible && m_Tween.GetValue() <= 0.001f && !m_Tween.IsActive() )
	{
		SetVisible( false );
		return;
	}

	SetVisible( true );
	m_Tween.Start( flNow, flDuration, flDelay, flTarget );
}

//-----------------------------------------------------------------------------
// Purpose: Jumps to visible/hidden without animating.
//-----------------------------------------------------------------------------
void CModernMenuCard::SnapTo( bool bVisible )
{
	m_Tween.Stop( bVisible ? 1.0f : 0.0f );
	UpdatePosition();
	SetVisible( bVisible );
}

//-----------------------------------------------------------------------------
// Purpose: Applies the current tween value to position and alpha.
//-----------------------------------------------------------------------------
void CModernMenuCard::UpdatePosition()
{
	float flValue = m_Tween.GetValue();

	int x = m_iBaseX + (int)( ( 1.0f - flValue ) * m_iSlideDistance );
	SetPos( x, m_iBaseY );
	SetAlpha( (int)( 255.0f * flValue ) );
}

//-----------------------------------------------------------------------------
// Purpose: Loads scheme colors and fonts.
//-----------------------------------------------------------------------------
void CModernMenuCard::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_BgColor = pScheme->GetColor( "ModernMenu.CardBg", Color( 22, 22, 28, 217 ) );
	m_TitleColor = pScheme->GetColor( "ModernMenu.CardTitle", Color( 242, 242, 245, 255 ) );
	m_TextColor = pScheme->GetColor( "ModernMenu.CardText", Color( 160, 160, 172, 255 ) );
	m_AccentColor = pScheme->GetColor( "ModernMenu.Accent", Color( 77, 216, 232, 255 ) );

	m_hTitleFont = pScheme->GetFont( "ModernMenu.CardTitleFont", true );
	if ( !m_hTitleFont )
	{
		m_hTitleFont = pScheme->GetFont( "Default", true );
	}

	m_hSubtitleFont = pScheme->GetFont( "ModernMenu.CardSubtitleFont", true );
	if ( !m_hSubtitleFont )
	{
		m_hSubtitleFont = m_hTitleFont;
	}

	m_hBodyFont = pScheme->GetFont( "ModernMenu.CardBodyFont", true );
	if ( !m_hBodyFont )
	{
		m_hBodyFont = pScheme->GetFont( "Default", true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Advances the tween.
//-----------------------------------------------------------------------------
void CModernMenuCard::OnThink()
{
	BaseClass::OnThink();

	if ( m_Tween.IsActive() )
	{
		m_Tween.Tick( engine->Time() );
		UpdatePosition();

		if ( !m_Tween.IsActive() && m_Tween.GetValue() <= 0.001f )
		{
			SetVisible( false );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draws the card.
//-----------------------------------------------------------------------------
void CModernMenuCard::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	vgui::ISurface *pSurface = vgui::surface();
	if ( !pSurface )
		return;

	// background
	pSurface->DrawSetColor( m_BgColor );
	pSurface->DrawFilledRect( 0, 0, wide, tall );

	// accent bar on the left edge
	pSurface->DrawSetColor( m_AccentColor );
	pSurface->DrawFilledRect( 0, 0, 3, tall );

	int nPad = vgui::scheme()->GetProportionalScaledValue( 20 );
	int nTitleY = vgui::scheme()->GetProportionalScaledValue( 14 );
	int nSubtitleY = vgui::scheme()->GetProportionalScaledValue( 44 );
	int nBodyY = vgui::scheme()->GetProportionalScaledValue( 68 );
	int nBodyStep = vgui::scheme()->GetProportionalScaledValue( 20 );

	DrawCardText( pSurface, m_hTitleFont, m_TitleColor, nPad, nTitleY, m_wszTitle );
	DrawCardText( pSurface, m_hSubtitleFont, m_TextColor, nPad, nSubtitleY, m_wszSubtitle );

	int y = nBodyY;
	for ( int i = 0; i < m_iBodyLineCount; ++i )
	{
		DrawCardText( pSurface, m_hBodyFont, m_TextColor, nPad, y, m_wszBody[i] );
		y += nBodyStep;
	}
}

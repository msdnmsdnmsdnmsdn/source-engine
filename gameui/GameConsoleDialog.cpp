//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "GameConsoleDialog.h"
#include "GameUI_Interface.h"
#include "vgui/IInput.h"
#include "vgui/ISurface.h"
#include "vgui/KeyCode.h"
#include "LoadingDialog.h"
#include "IGameUIFuncs.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameConsoleDialog::CGameConsoleDialog() : BaseClass( NULL, "GameConsole", false )
{
	if( NeedProportional() ) SetProportional(true);
	AddActionSignalTarget( this );

	m_eFadeState = CONSOLE_FADE_NONE;
	m_flFadeStartTime = 0.0f;
	m_flFadeInDelay = 0.0f;

	// We drive the fade ourselves so the console can coordinate with the menu.
	DisableFadeEffect();
	SetFadeEffectDisableOverride( true );
}

//-----------------------------------------------------------------------------
// Purpose: Shows the console and fades it in.
//-----------------------------------------------------------------------------
void CGameConsoleDialog::Activate()
{
	BaseClass::Activate();

	// In the pause menu the console appears alongside the menu, so it can
	// start fading immediately. On the main menu the cards slide out first,
	// so wait for them to finish before fading the console in.
	m_flFadeInDelay = GameUI().IsInLevel() ? 0.06f : 0.16f;
	m_flFadeStartTime = engine->Time();
	m_eFadeState = CONSOLE_FADE_IN;

	SetAlpha( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Fades the console out and hides it when done.
//-----------------------------------------------------------------------------
void CGameConsoleDialog::Hide()
{
	if ( m_eFadeState == CONSOLE_FADE_OUT )
		return;

	if ( !IsVisible() || GetAlpha() <= 0 )
	{
		m_eFadeState = CONSOLE_FADE_NONE;
		SetAlpha( 255 );
		BaseClass::Hide();
		return;
	}

	m_flFadeStartTime = engine->Time();
	m_eFadeState = CONSOLE_FADE_OUT;
}

//-----------------------------------------------------------------------------
// Purpose: True while the console is fading out.
//-----------------------------------------------------------------------------
bool CGameConsoleDialog::IsClosing() const
{
	return m_eFadeState == CONSOLE_FADE_OUT;
}

//-----------------------------------------------------------------------------
// Purpose: Advances the fade animation.
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnThink()
{
	BaseClass::OnThink();

	float flNow = engine->Time();

	if ( m_eFadeState == CONSOLE_FADE_IN )
	{
		const float flDuration = 0.26f;

		float flT = ( flNow - m_flFadeStartTime - m_flFadeInDelay ) / flDuration;
		if ( flT < 0.0f )
		{
			SetAlpha( 0 );
			return;
		}

		if ( flT >= 1.0f )
		{
			SetAlpha( 255 );
			m_eFadeState = CONSOLE_FADE_NONE;
			return;
		}

		float flP = 1.0f - ( 1.0f - flT ) * ( 1.0f - flT ) * ( 1.0f - flT );
		SetAlpha( (int)( 255.0f * flP ) );
	}
	else if ( m_eFadeState == CONSOLE_FADE_OUT )
	{
		const float flDuration = 0.20f;

		float flT = ( flNow - m_flFadeStartTime ) / flDuration;
		if ( flT >= 1.0f )
		{
			m_eFadeState = CONSOLE_FADE_NONE;
			SetAlpha( 255 );
			BaseClass::Hide();
			return;
		}

		float flP = 1.0f - flT;
		SetAlpha( (int)( 255.0f * flP * flP ) );
	}
}


//-----------------------------------------------------------------------------
// Purpose: generic vgui command handler
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnCommand(const char *command)
{
	if ( !Q_stricmp( command, "Close" ) )
	{
		if ( GameUI().IsInBackgroundLevel() )
		{
			// Tell the engine we've hid the console, so that it unpauses the game
			// even though we're still sitting at the menu.
			engine->ClientCmd_Unrestricted( "unpause" );
		}
	}

	BaseClass::OnCommand(command);
}


//-----------------------------------------------------------------------------
// HACK: Allow F key bindings to operate even when typing in the text entry field
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnKeyCodeTyped(KeyCode code)
{
	BaseClass::OnKeyCodeTyped(code);

	// check for processing
	if ( m_pConsolePanel->TextEntryHasFocus() )
	{
		// HACK: Allow F key bindings to operate even here
		if ( code >= KEY_F1 && code <= KEY_F12 )
		{
			// See if there is a binding for the FKey
			const char *binding = gameuifuncs->GetBindingForButtonCode( code );
			if ( binding && binding[0] )
			{
				// submit the entry as a console commmand
				char szCommand[256];
				Q_strncpy( szCommand, binding, sizeof( szCommand ) );
				engine->ClientCmd_Unrestricted( szCommand );
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Submits a command
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnCommandSubmitted( const char *pCommand )
{
	engine->ClientCmd_Unrestricted( pCommand );
}


//-----------------------------------------------------------------------------
// Submits a command
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnClosedByHittingTilde()
{
	if ( !LoadingDialog() )
	{
		GameUI().HideGameUI();
	}
	else
	{
		vgui::surface()->RestrictPaintToSinglePanel( LoadingDialog()->GetVPanel() );
	}
}

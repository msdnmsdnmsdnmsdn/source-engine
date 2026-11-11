//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef GAMECONSOLEDIALOG_H
#define GAMECONSOLEDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/consoledialog.h"
#include <Color.h>
#include "utlvector.h"
#include "EngineInterface.h"
#include "vgui_controls/Frame.h"


//-----------------------------------------------------------------------------
// Purpose: Game/dev console dialog
//-----------------------------------------------------------------------------
class CGameConsoleDialog : public vgui::CConsoleDialog
{
	DECLARE_CLASS_SIMPLE( CGameConsoleDialog, vgui::CConsoleDialog );

public:
	CGameConsoleDialog();

	// Fade the console in when it is activated and out before hiding it.
	virtual void Activate();
	virtual void Hide();

	// True while the console is fading out.
	bool IsClosing() const;

private:
	enum ConsoleFadeState_t
	{
		CONSOLE_FADE_NONE = 0,
		CONSOLE_FADE_IN,
		CONSOLE_FADE_OUT,
	};

	MESSAGE_FUNC( OnClosedByHittingTilde, "ClosedByHittingTilde" );
	MESSAGE_FUNC_CHARPTR( OnCommandSubmitted, "CommandSubmitted", command );

	virtual void OnKeyCodeTyped( vgui::KeyCode code );
	virtual void OnCommand( const char *command );
	virtual void OnThink();

	ConsoleFadeState_t m_eFadeState;
	float m_flFadeStartTime;
	float m_flFadeInDelay;
};


#endif // GAMECONSOLEDIALOG_H

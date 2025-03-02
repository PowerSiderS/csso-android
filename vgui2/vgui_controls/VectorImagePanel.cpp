//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include <vgui_controls/Panel.h>
#include <vgui_controls/VectorImagePanel.h>
#include <vgui/ISurface.h>
#include <bitmap/bitmap.h>
#include <KeyValues.h>
#include "filesystem.h"
#include "VGuiMatSurface/IMatSystemSurface.h"

#include "lunasvg/lunasvg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace lunasvg;

DECLARE_BUILD_FACTORY( VectorImagePanel );

//-----------------------------------------------------------------------------
// Purpose: Check box image
//-----------------------------------------------------------------------------
VectorImagePanel::VectorImagePanel( Panel *parent, const char *name ): Panel( parent, name )
{
	m_nTextureId = -1;
}

VectorImagePanel::~VectorImagePanel()
{
	DestroyTexture();
}

void VectorImagePanel::DestroyTexture()
{
	if ( m_nTextureId != -1 )
	{
		vgui::surface()->DestroyTextureID( m_nTextureId );
		m_nTextureId = -1;
	}
}

void VectorImagePanel::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	const char *szSVGPath = inResourceData->GetString( "image" );
	if ( szSVGPath )
	{
		char szFullPath[MAX_PATH];
		g_pFullFileSystem->RelativePathToFullPath( szSVGPath, "MOD", szFullPath, sizeof( szFullPath ) );

		std::unique_ptr<Document> document = Document::loadFromFile( szFullPath ); // load the svg

		if ( !document )
		{
			Warning( "VectorImagePanel: %s load failed.\n", szSVGPath );
			DestroyTexture();
			return;
		}

		int renderWide, renderTall;
		GetSize( renderWide, renderTall );
		Bitmap bitmap = document->renderToBitmap( renderWide, renderTall ); // render the svg

		if ( !bitmap.valid() )
		{
			Warning( "VectorImagePanel: %s render failed.\n", szSVGPath );
			DestroyTexture();
			return;
		}

		if ( m_nTextureId == -1 )
		{
			m_nTextureId = vgui::surface()->CreateNewTextureID( true );
		}

		vgui::surface()->DrawSetTextureRGBA( m_nTextureId, bitmap.data(), bitmap.width(), bitmap.height(), 1, true );
	}
}

void VectorImagePanel::Paint()
{
    if ( m_nTextureId == -1 )
		return;

	int wide, tall;
	vgui::surface()->DrawGetTextureSize( m_nTextureId, wide, tall );

	vgui::surface()->DrawSetTexture( m_nTextureId );
	vgui::surface()->DrawSetColor( GetFgColor() );
	g_pMatSystemSurface->DisableClipping( true );
	vgui::surface()->DrawTexturedRect( 0, 0, wide, tall );
	g_pMatSystemSurface->DisableClipping( false );
}
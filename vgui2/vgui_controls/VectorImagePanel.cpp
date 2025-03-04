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

#define NANOSVG_IMPLEMENTATION	// Expands implementation
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/nanosvg.h"
#include "nanosvg/nanosvgrast.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

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

		int w, h;
		// Load SVG
		NSVGimage* image;
		image = nsvgParseFromFile( szFullPath, "px", inResourceData->GetInt( "dpi", 96 ) );

		w = image->width;
		h = image->height;
		// Create rasterizer (can be used to render multiple images).
		struct NSVGrasterizer* rast = nsvgCreateRasterizer();
		// Allocate memory for image
		unsigned char* img = (unsigned char*) malloc( w*h * 4 );
		// Rasterize
		nsvgRasterize( rast, image, 0, 0, 1, img, w, h, w * 4 );
		// Clean memory
		nsvgDeleteRasterizer( rast );

		if ( m_nTextureId == -1 )
		{
			m_nTextureId = vgui::surface()->CreateNewTextureID( true );
		}

		vgui::surface()->DrawSetTextureRGBA( m_nTextureId, img, w, h, 1, true );
	}
}

void VectorImagePanel::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	vgui::surface()->DrawSetTexture( m_nTextureId );
	vgui::surface()->DrawSetColor( GetFgColor() );
	g_pMatSystemSurface->DisableClipping( true );
	vgui::surface()->DrawTexturedRect( 0, 0, wide, tall );
	g_pMatSystemSurface->DisableClipping( false );
}

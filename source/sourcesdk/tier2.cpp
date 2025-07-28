//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//

#include <tier2/tier2.h>
#include "tier0/dbg.h"
#include "filesystem.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/IColorCorrection.h"
#include "materialsystem/idebugtextureinfo.h"
#include "materialsystem/ivballoctracker.h"
#include "inputsystem/iinputsystem.h"
#include "networksystem/inetworksystem.h"
#include "p4lib/ip4.h"
#include "mdllib/mdllib.h"
#include "filesystem/IQueuedLoader.h"
#include "Platform.hpp"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// These tier2 libraries must be set by any users of this library.
// They can be set by calling ConnectTier2Libraries or InitDefaultFileSystem.
// It is hoped that setting this, and using this library will be the common mechanism for
// allowing link libraries to access tier2 library interfaces
//-----------------------------------------------------------------------------
#if ARCHITECTURE_IS_X86
IFileSystem *g_pFullFileSystem = 0;
IMaterialSystem *materials = 0;
IMaterialSystem *g_pMaterialSystem = 0;
IInputSystem *g_pInputSystem = 0;
INetworkSystem *g_pNetworkSystem = 0;
IMaterialSystemHardwareConfig *g_pMaterialSystemHardwareConfig = 0;
IDebugTextureInfo *g_pMaterialSystemDebugTextureInfo = 0;
IVBAllocTracker *g_VBAllocTracker = 0;
IColorCorrectionSystem *colorcorrection = 0;
IMdlLib *mdllib = 0;
IQueuedLoader *g_pQueuedLoader = 0;
#endif
IP4 *p4 = 0;


//-----------------------------------------------------------------------------
// Call this to connect to all tier 2 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier2Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount )
{
	// Don't connect twice..
	Assert( !g_pFullFileSystem && !materials && !g_pInputSystem && !g_pNetworkSystem && 
		!p4 && !mdllib && !g_pMaterialSystemDebugTextureInfo && !g_VBAllocTracker &&
		!g_pMaterialSystemHardwareConfig && !g_pQueuedLoader );

	for ( int i = 0; i < nFactoryCount; ++i )
	{
		if ( !g_pFullFileSystem )
		{
			g_pFullFileSystem = ( IFileSystem * )pFactoryList[i]( FILESYSTEM_INTERFACE_VERSION, NULL );
		}
		if ( !materials )
		{
			if (pFactoryList[i])
			{
				if (!g_pFullFileSystem)
				{
					auto iface = pFactoryList[i](FILESYSTEM_INTERFACE_VERSION, NULL);
					if (iface)
					{
						g_pFullFileSystem = (IFileSystem *)iface;
					}
				}
			}
		}
		if (!g_pInputSystem && pFactoryList[i])
		{
			void *iface = pFactoryList[i](INPUTSYSTEM_INTERFACE_VERSION, NULL);
			if (iface)
			{
				g_pInputSystem = (IInputSystem *)iface;
			}
		}
		if (!g_pNetworkSystem && pFactoryList[i])
		{
			void *iface = pFactoryList[i](NETWORKSYSTEM_INTERFACE_VERSION, NULL);
			if (iface)
			{
				g_pNetworkSystem = (INetworkSystem *)iface;
			}
		}
		if ( !g_pMaterialSystemHardwareConfig && pFactoryList[i])
		{
			void *iface = pFactoryList[i](MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, NULL);
			if (iface)
			{
				g_pMaterialSystemHardwareConfig = (IMaterialSystemHardwareConfig *)iface;
			}
		}
		if (!g_pMaterialSystemDebugTextureInfo && pFactoryList[i])
		{
			void *iface = pFactoryList[i](DEBUG_TEXTURE_INFO_VERSION, 0);
			if (iface)
			{
				g_pMaterialSystemDebugTextureInfo = (IDebugTextureInfo *)iface;
			}
		}
		if (!g_VBAllocTracker && pFactoryList[i])
		{
			void *iface = pFactoryList[i](VB_ALLOC_TRACKER_INTERFACE_VERSION, 0);
			if (iface)
			{
				g_VBAllocTracker = (IVBAllocTracker*)iface;
			}
		}
		if (!colorcorrection && pFactoryList[i])
		{
			void *iface = pFactoryList[i](COLORCORRECTION_INTERFACE_VERSION, NULL);
			if (iface)
			{
				colorcorrection = (IColorCorrectionSystem*)iface;
			}
		}
		if (!p4 && pFactoryList[i])
		{
			void *iface = pFactoryList[i](P4_INTERFACE_VERSION, NULL);
			if (iface)
			{
				p4 = (IP4*)iface;
			}
		}
		if (!mdllib && pFactoryList[i])
		{
			void *iface = pFactoryList[i](MDLLIB_INTERFACE_VERSION, NULL);
			if (iface)
			{
				mdllib = (IMdlLib *)iface;
			}
		}
		if (!g_pQueuedLoader && pFactoryList[i])
		{
			void *iface = pFactoryList[i](QUEUEDLOADER_INTERFACE_VERSION, NULL);
			if (iface)
			{
				g_pQueuedLoader = (IQueuedLoader *)iface;
			}
		}
	}
}

void DisconnectTier2Libraries()
{

	g_pFullFileSystem = 0;
	materials = g_pMaterialSystem = 0;
	g_pMaterialSystemHardwareConfig = 0;
	g_pMaterialSystemDebugTextureInfo = 0;
	g_pInputSystem = 0;
	g_pNetworkSystem = 0;
	colorcorrection = 0;
	p4 = 0;
	mdllib = 0;
	g_pQueuedLoader = 0;
}
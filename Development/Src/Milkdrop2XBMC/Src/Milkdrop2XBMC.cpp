#include <windows.h>
#include <io.h>
#include <vector>
#include "../../vis_milk2/vis_milk2/plugin.h"


#define TARGET_WINDOWS
#include "addons/include/xbmc_vis_dll.h"
//#include "xbmc_addon_cpp_dll.h"


CPlugin g_plugin;
bool IsInitialized = false;

// settings vector
//StructSetting** g_structSettings;

extern "C" ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
	if (!props)
		return ADDON_STATUS_UNKNOWN;

	VIS_PROPS* visprops = (VIS_PROPS*)props;

	swprintf(g_plugin.m_szPluginsDirPath, L"%hs\\", visprops->presets);

	g_plugin.PluginPreInitialize(0,0);
	g_plugin.PluginInitialize((LPDIRECT3DDEVICE9)visprops->device, visprops->x, visprops->y, visprops->width, visprops->height, visprops->pixelRatio);
	IsInitialized = true;
	return ADDON_STATUS_NEED_SETTINGS;
//  return ADDON_STATUS_NEED_SAVEDSETTINGS; // We need some settings to be saved later before we quit this plugin
}

extern "C" void Start( int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName )
{
}

extern "C" void ADDON_Stop()
{
	if( IsInitialized )
	{
		g_plugin.PluginQuit();
		IsInitialized = false;
	}
}

unsigned char waves[2][576];

extern "C" void AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
{
	int ipos=0;
	while (ipos < 576)
	{
		for (int i=0; i < iAudioDataLength; i+=2)
		{
			waves[0][ipos] = char (pAudioData[i] * 255.0f);
			waves[1][ipos] = char (pAudioData[i+1]  * 255.0f);
			ipos++;
			if (ipos >= 576) break;
		}
	}
}

extern "C" void Render()
{
	g_plugin.PluginRender(waves[0], waves[1]);
}

extern "C" void GetInfo(VIS_INFO* pInfo)

{
	pInfo->bWantsFreq = false;
	pInfo->iSyncDelay = 0;
}

extern "C"   bool OnAction(long action, const void *param)
{
/*
	bool handled = true;
	if( action == VIS_ACTION_UPDATE_TRACK )
	{
		VisTrack* visTrack = (VisTrack*) param;
		g_Vortex->UpdateTrack( visTrack );
	}
	else if( action == VIS_ACTION_UPDATE_ALBUMART )
	{
		g_Vortex->UpdateAlbumArt( ( char* ) param );
	}
	else if (action == VIS_ACTION_NEXT_PRESET)
	{
		g_Vortex->LoadNextPreset();
	}
	else if (action == VIS_ACTION_PREV_PRESET)
	{
		g_Vortex->LoadPreviousPreset();
	}
	else if (action == VIS_ACTION_LOAD_PRESET && param)
	{
		g_Vortex->LoadPreset( (*(int *)param) );
	}
	else if (action == VIS_ACTION_LOCK_PRESET)
	{
		g_Vortex->GetUserSettings().PresetLocked = !g_Vortex->GetUserSettings().PresetLocked;
	}
	else if (action == VIS_ACTION_RANDOM_PRESET)
	{
		g_Vortex->LoadRandomPreset();
	}
	else
	{
		handled = false;
	}

	return handled;
	*/
	return false;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to XBMC for display
//-----------------------------------------------------------------------------
extern "C" unsigned int GetPresets(char ***presets)
{
	if( !presets || !IsInitialized || !g_plugin.m_bPresetListReady )
	{
		return 0;
	}

	int NumPresets = g_plugin.m_nPresets;
//	*presets = g_plugin.m_presets;//m_pPresetAddr;
//	return g_plugin->m_nPresets;
	return 0;
	}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
extern "C" unsigned GetPreset()
{
	return 0;
}

//-- IsLocked -----------------------------------------------------------------
// Returns true if preset is locked
//-----------------------------------------------------------------------------
extern "C" bool IsLocked()
{
	return false;
}

//-- Destroy-------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{
	ADDON_Stop();
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" bool ADDON_HasSettings()
{
	return true;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_GetStatus()
{
	return ADDON_STATUS_OK;
}

extern "C" unsigned int ADDON_GetSettings(ADDON_StructSetting*** sSet)
{
	return 0;
}

extern "C" void ADDON_FreeSettings()
{

}

extern "C" ADDON_STATUS ADDON_SetSetting(const char* id, const void* value)
{
	/*
	if ( !id || !value || g_Vortex == NULL )
		return ADDON_STATUS_UNKNOWN;

	// Set the settings got from XBMC
	UserSettings& userSettings = g_Vortex->GetUserSettings();

	if (strcmp(id, "###GetSavedSettings") == 0) // We have some settings to be saved in the settings.xml file
	{
//		if (strcmp((char*)value, "0") == 0)
//		{
//			strcpy((char*)id, "lastpresetfolder");
//			strcpy((char*)value, g_plugin->m_szPresetDir);
//		}
//		if (strcmp((char*)value, "1") == 0)
//		{
//			strcpy((char*)id, "lastlockedstatus");
//			strcpy((char*)value, (g_plugin->m_bHoldPreset ? "true" : "false"));
//		}
//		if (strcmp((char*)value, "2") == 0)
//		{
//			strcpy((char*)id, "lastpresetidx");
//			sprintf ((char*)value, "%i", g_plugin->m_nCurrentPreset);
//		}
//		if (strcmp((char*)value, "3") == 0)
//		{
//			strcpy((char*)id, "###End");
//		}
		return ADDON_STATUS_OK;
	}

	if (strcmpi(id, "Use Preset") == 0)
	{
		OnAction(34, &value);
	}
	else if (strcmpi(id, "RandomPresets") == 0)
	{
		userSettings.RandomPresetsEnabled = *(bool*)value == 1;
	}
	else if (strcmpi(id, "TimeBetweenPresets") == 0)
	{
		userSettings.TimeBetweenPresets = (float)(*(int*)value * 5 + 5);
	}
	else if (strcmpi(id, "AdditionalRandomTime") == 0)
	{
		userSettings.TimeBetweenPresetsRand = (float)(*(int*)value * 5 ); 
	}
	else if (strcmpi(id, "EnableTransitions") == 0)
	{
		userSettings.TransitionsEnabled = *(bool*)value == 1;
	}
	else if (strcmpi(id, "StopFirstPreset") == 0)
	{
		userSettings.StopFirstPreset = *(bool*)value == 1;
	}
	else if (strcmpi(id, "EnableAnnouncements") == 0)
	{
		userSettings.EnableAnnouncements = *(bool*)value == 1;
	}
	else if (strcmpi(id, "ShowFPS") == 0)
	{
		userSettings.ShowFPS = *(bool*)value == 1;
	}
	else if (strcmpi(id, "ShowDebugConsole") == 0)
	{
		userSettings.ShowDebugConsole = *(bool*)value == 1;
	}
	else if (strcmpi(id, "ShowAudioAnalysis") == 0)
	{
		userSettings.ShowAudioAnalysis = *(bool*)value == 1;
	}
 	else
 		return ADDON_STATUS_UNKNOWN;
		*/
	return ADDON_STATUS_OK;
}

//-- GetSubModules ------------------------------------------------------------
// Return any sub modules supported by this vis
//-----------------------------------------------------------------------------
extern "C"   unsigned int GetSubModules(char ***presets)
{
  return 0; // this vis supports 0 sub modules
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
//-----------------------------------------------------------------------------
extern "C" void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}
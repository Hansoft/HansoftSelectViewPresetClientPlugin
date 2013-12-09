
#include "../../Projects/HansoftSDK_7_105/HPMSdkCpp.h"

#include <tchar.h>
#include <conio.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <memory>

#define mod_export __declspec(dllexport)

using namespace std;
using namespace HPMSdk;

class CHansoftSelectViewPreset_ClientPlugin : public HPMSdkCallbacks
{
public:

	struct CDynamicHelper
	{
		HPMNotificationSubscription m_RightClickSubscription;
		HPMNotificationSubscription m_DynamicUpdateSubscription;
	};

	shared_ptr<CDynamicHelper> m_pDynamicHelper;
	HPMSdkSession *m_pSession;
	HPMString m_IntegrationIdentifier;
	HPMUniqueID m_LastSelectedProject;


	CHansoftSelectViewPreset_ClientPlugin(const void *_pClientData)
	{
		m_pSession = nullptr;

		try
		{
			m_pSession = HPMSdkSession::SessionOpen(hpm_str(""), 0, hpm_str(""), hpm_str(""), hpm_str(""), this, NULL, true, EHPMSdkDebugMode_Off, _pClientData, 0, hpm_str(""), HPMSystemString(), NULL);
			m_IntegrationIdentifier = hpm_str("com.hansoft.selectviewpreset.clientplugin");
		}
		catch (HPMSdkException &_Exception)
		{
			HPMString ErrorStr = _Exception.GetAsString();

			wstringstream Stream;
			Stream << hpm_str("Error initializing Select View Preset Client Plugin:\r\n\r\n");
			Stream << ErrorStr;
			MessageBox(NULL, Stream.str().c_str(), hpm_str("Select View Preset Client Plugin Error"), MB_OK|MB_ICONERROR);
		}
		catch (HPMSdkCppException &_Exception)
		{
			wstringstream Stream; 
			Stream << hpm_str("Error initializing Select View Preset Client Plugin:\r\n\r\n");
			Stream << _Exception.what();
			MessageBox(NULL, Stream.str().c_str(), hpm_str("Select View Preset Client Plugin Error"), MB_OK|MB_ICONERROR);
		}
	}

	~CHansoftSelectViewPreset_ClientPlugin()
	{ 
		m_pDynamicHelper.reset();
		if (m_pSession)
		{
			HPMSdkSession::SessionDestroy(m_pSession);
			m_pSession = nullptr;
		}
	}

	virtual void On_ProcessError(EHPMError _Error)
	{
	}

	virtual void On_Callback(const HPMChangeCallbackData_ClientSyncDone &_Data)
	{

		try
		{
			m_pDynamicHelper = shared_ptr<CDynamicHelper>(new CDynamicHelper());
			m_pDynamicHelper->m_RightClickSubscription = m_pSession->GlobalRegisterForRightClickNotifications(NULL);
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

	bool UserInGroup(HPMUniqueID userID, HPMUniqueID groupId)
	{
		bool retValue = false;
		HPMResourceEnum resourcesInGroup = m_pSession->ResourceGroupGetResources(groupId);
		for (unsigned i=0; i < resourcesInGroup.m_Resources.size(); i+=1)
			if (resourcesInGroup.m_Resources[i].m_ID == userID.m_ID)
				return true;
		return retValue;
	}

	bool PresetApplicable(HPMProjectViewPreset preset)
	{
		bool retValue = false;
		HPMUniqueID loggedInUser = m_pSession->ResourceGetLoggedIn();
		for (unsigned i=0; i<preset.m_Resources.size(); i+=1)
		{
			HPMResourceDefinition resourceDef = preset.m_Resources[i];
			if (resourceDef.m_GroupingType == EHPMResourceGroupingType_AllProjectMembers)
				return true;
			else if (resourceDef.m_GroupingType == EHPMResourceGroupingType_ResourceGroup && UserInGroup(loggedInUser, resourceDef.m_ID))
				return true;
			else if (resourceDef.m_GroupingType == EHPMResourceGroupingType_Resource && resourceDef.m_ID.m_ID == loggedInUser.m_ID)
				return true;
		}
		return retValue;
	}

	virtual void On_Callback(const HPMChangeCallbackData_RightClickDisplayTaskMenu &_Data)
	{
		try
		{
			HPMDataHistoryGetHistoryParameters p;
			m_LastSelectedProject = _Data.m_ProjectID;
			m_pSession->GlobalAddRightClickMenuItem
				(
				_Data.m_RightClickContext
				, hpm_str(""), m_IntegrationIdentifier + hpm_str(".taskmenu.selectviewpresetclientplugin.root")
				, m_pSession->LocalizationCreateUntranslatedStringFromString(hpm_str("Select view preset"))
				, NULL
				)
				;
			std::vector<HPMProjectViewPreset> presets = m_pSession->ProjectGetViewPresets(m_LastSelectedProject).m_Presets;
			for (unsigned i=0; i<presets.size(); i+=1)
			{
				if (PresetApplicable(presets[i]))
					m_pSession->GlobalAddRightClickMenuItem
						(
						_Data.m_RightClickContext
						, m_IntegrationIdentifier + hpm_str(".taskmenu.selectviewpresetclientplugin.root")
						, m_IntegrationIdentifier + hpm_str(".taskmenu.selectviewpresetclientplugin." + presets[i].m_Name)
						, m_pSession->LocalizationCreateUntranslatedStringFromString(presets[i].m_Name)
						, NULL
						)
						;
			}
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

	virtual void On_Callback(const HPMChangeCallbackData_RightClickMenuItemSelected &_Data)
	{
		try
		{

			std::vector<HPMProjectViewPreset> presets = m_pSession->ProjectGetViewPresets(m_LastSelectedProject).m_Presets;
			for (unsigned i=0; i<presets.size(); i+=1)
			{
				if (_Data.m_UniqueName  == m_IntegrationIdentifier + hpm_str(".taskmenu.selectviewpresetclientplugin.") + presets[i].m_Name)
				{
					m_pSession->ProjectViewPresetApply(m_LastSelectedProject, m_pSession->ResourceGetLoggedIn(), presets[i]);
				}
			}
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

};

CHansoftSelectViewPreset_ClientPlugin *g_pClientPlugin;

extern "C" mod_export void DHPMSdkCallingConvention HPMClientSDKCreate(const void *_pClientData)
{
	g_pClientPlugin = new CHansoftSelectViewPreset_ClientPlugin(_pClientData);
}

extern "C" mod_export void DHPMSdkCallingConvention HPMClientSDKDestroy()
{
	delete g_pClientPlugin;
}

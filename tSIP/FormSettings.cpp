//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include "FormAccount.h"
#include "FormHotkeys.h"
#include "FormPhones.h"
#include "AudioDevicesList.h"
#include "ProgrammableButtons.h"
#include "FormLuaScript.h"
#include "UaMain.h"
#include "Paths.h"
#include "Registry.hpp"
#include "Branding.h"
#include <FileCtrl.hpp>
#include <assert.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;

namespace
{
	int audioCodecsEnabledDraggedIndex = -1;

	/** \brief Common combobox index -> output audio module name function
	*/
	const char* GetOutputModuleForCbIndex(int id)
	{
		switch (id)
		{
			case 0:
				return UaConf::modPortaudio;
			case 1:
				return UaConf::modWinwave;
			case 2:
				return UaConf::modNullaudio;
			default:
				assert(!"Unhandled module index!");
				return UaConf::modPortaudio;
		}
	}

	int GetOutputModuleCbIndex(const char* name)
	{
		if (strcmp(name, UaConf::modPortaudio) == 0)
			return 0;
		else if (strcmp(name, UaConf::modWinwave) == 0)
			return 1;
		else if (strcmp(name, UaConf::modNullaudio) == 0)
			return 2;
		assert(!"Unhandled module name!");
		return 0;
	}
}

inline void strncpyz(char* dst, const char* src, int dstsize)
{
	strncpy(dst, src, dstsize);
	dst[dstsize-1] = '\0';
}

//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner), lastTab(NULL)
{
	tabs.push_back(tsGeneral);
	tabs.push_back(tsNetwork);
	tabs.push_back(tsAccount);
	tabs.push_back(tsSpeedDial);
	tabs.push_back(tsCalls);
	tabs.push_back(tsDisplay);
	tabs.push_back(tsLocking);
	tabs.push_back(tsRing);
	tabs.push_back(tsAudioIO);
	tabs.push_back(tsAudioProcessing);
	tabs.push_back(tsRecording);
	tabs.push_back(tsCodecs);
	tabs.push_back(tsIntegration);
	tabs.push_back(tsHotkeys);
	tabs.push_back(tsContacts);
	tabs.push_back(tsHistory);
	tabs.push_back(tsPhones);
	tabs.push_back(tsTrayNotifier);
	tabs.push_back(tsScripts);
	tabs.push_back(tsLogging);

	pcGeneral->ActivePage = tsGeneral;
	frmHotkeys = new TfrmHotkeys(tsHotkeys);
	frmHotkeys->Parent = tsHotkeys;
	frmHotkeys->Visible = true;
	if (Branding::recording == false)
	{
    	tsRecording->TabVisible = false;
	}
	frmPhones = new TfrmPhones(tsPhones);
	frmPhones->Parent = tsPhones;
	frmPhones->Visible = true;

	// fill number of columns combobox and widths vleditor according to limit in settings
	cbSpeedDialSize->Items->Clear();
	//vleSpeedDialColWidths->RowCount = ProgrammableButtons::EXT_CONSOLE_COLUMNS
	for (int i=1; i<=ProgrammableButtons::EXT_CONSOLE_COLUMNS; i++)
	{
		AnsiString desc;
		if (i == 1)
		{
			desc = "1 column";
		}
		else
		{
			desc.sprintf("%d columns", i);
		}
		cbSpeedDialSize->Items->Add(desc);
		vleSpeedDialColWidths->Values[i] = 100 + i;
	}

	for (int i=0; i<pcGeneral->PageCount; i++)
	{
		pcGeneral->Pages[i]->TabVisible = false;
	}
	// make "Accounts" selected and visible
	tvSelector->Items->Item[tsAccount->PageIndex]->Selected = true;
}
//---------------------------------------------------------------------------

void TfrmSettings::FillDevList(TComboBox *target, int module, bool out, AnsiString selected)
{
	target->Items->Clear();
	std::vector<AnsiString> *v = NULL;
	if (module == 0)
	{
		if (out)
			v = &AudioDevicesList::Instance().portaudioDevsOut;
		else
			v = &AudioDevicesList::Instance().portaudioDevsIn;
	}
	else if (module == 1)
	{
		if (out)
			v = &AudioDevicesList::Instance().winwaveDevsOut;
		else
			v = &AudioDevicesList::Instance().winwaveDevsIn;
	}
	else
	{
		return;
    }
	assert(v);
	for (int i=0; i<v->size(); i++)
	{
		AnsiString dev = v->at(i);
		target->Items->Add(dev);
		if (dev == selected)
		{
        	target->ItemIndex = i;
		}
	}
}

void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
	AudioDevicesList::Instance().Refresh();

	tmpSettings = appSettings;

	if (!strcmp(tmpSettings.uaConf.audioCfgSrc.mod, UaConf::modPortaudio))
	{
		cbSoundInputMod->ItemIndex = 0;
	}
	else if (!strcmp(tmpSettings.uaConf.audioCfgSrc.mod, UaConf::modWinwave))
	{
		cbSoundInputMod->ItemIndex = 1;
	}
	else if (!strcmp(tmpSettings.uaConf.audioCfgSrc.mod, UaConf::modAufile))
	{
		cbSoundInputMod->ItemIndex = 2;
	}
	else if (!strcmp(tmpSettings.uaConf.audioCfgSrc.mod, UaConf::modNullaudio))
	{
		cbSoundInputMod->ItemIndex = 3;
	}
	else
	{
		assert(!"Unhandled audio module type!");
		cbSoundInputMod->ItemIndex = 0;
	}
	cbSoundInputModChange(NULL);

	cbSoundOutputMod->ItemIndex = GetOutputModuleCbIndex(tmpSettings.uaConf.audioCfgPlay.mod);
	cbSoundOutputModChange(NULL);

	edSoundInputWave->Text = tmpSettings.uaConf.audioCfgSrc.wavefile;

	cbSoundAlertOutputMod->ItemIndex = GetOutputModuleCbIndex(tmpSettings.uaConf.audioCfgAlert.mod);
	cbSoundAlertOutputModChange(NULL);

	cbSoundRingOutputMod->ItemIndex = GetOutputModuleCbIndex(tmpSettings.uaConf.audioCfgRing.mod);
	cbSoundRingOutputModChange(NULL);

	cbSoundOutputIntercomMod->ItemIndex = GetOutputModuleCbIndex(tmpSettings.uaConf.audioCfgPlayIntercom.mod);
	cbSoundOutputIntercomModChange(NULL);

	cbAec->ItemIndex = tmpSettings.uaConf.aec;
	edLocalAddress->Text = tmpSettings.uaConf.local.c_str();
	edIfName->Text = tmpSettings.uaConf.ifname.c_str();
	edRtpPortMin->Text = tmpSettings.uaConf.avt.portMin;
	edRtpPortMax->Text = tmpSettings.uaConf.avt.portMax;
	edJbufDelayMin->Text = tmpSettings.uaConf.avt.jbufDelayMin;
	edJbufDelayMax->Text = tmpSettings.uaConf.avt.jbufDelayMax;
	edRtpTimeout->Text = tmpSettings.uaConf.avt.rtpTimeout;

	for (int i=0; i<tmpSettings.uaConf.accounts.size(); i++)
	{
		frmAccount = new TfrmAccount(NULL, i, tmpSettings.uaConf.accounts[i]);
		frmAccount->Parent = frmSettings->scrbAccounts;
		frmAccount->Visible = true;
		// force realign
		frmAccount->Align = alBottom;
		frmAccount->Align = alTop;
		frmAccountVector.push_back(frmAccount);
	}

	edGuiScaling->Text = tmpSettings.gui.scalingPct;

	edMainWindowHeight->Text = tmpSettings.frmMain.iHeight;

	chbAlwaysOnTop->Checked = tmpSettings.frmMain.bAlwaysOnTop;
	chbStartMinimizedToTray->Checked = tmpSettings.frmMain.bStartMinimizedToTray;

	chbTrayNotifierShowOnIncoming->Checked = tmpSettings.frmTrayNotifier.showOnIncoming;
	chbSkipTrayNotifierIfMainWindowVisible->Checked = tmpSettings.frmTrayNotifier.skipIfMainWindowVisible;
	chbTrayNotifierShowOnOutgoing->Checked = tmpSettings.frmTrayNotifier.showOnOutgoing;
	edTrayNotifierBackgroundImage->Text = tmpSettings.frmTrayNotifier.backgroundImage;
	edTrayNotifierGuiScaling->Text = tmpSettings.frmTrayNotifier.scalingPct;
	chbTrayNotifierHideWhenAnsweringCall->Checked = tmpSettings.frmTrayNotifier.hideWhenAnsweringCall;
	chbTrayNotifierHideWhenAnsweringCallAutomatically->Checked = tmpSettings.frmTrayNotifier.hideWhenAnsweringCallAutomatically;
	chbFrmMainUseCustomApplicationTitle->Checked = tmpSettings.frmMain.bUseCustomApplicationTitle;
	edFrmMainCustomApplicationTitle->Text = tmpSettings.frmMain.customApplicationTitle;
	chbFrmMainUseCustomCaption->Checked = tmpSettings.frmMain.bUseCustomCaption;
	edFrmMainCustomCaption->Text = tmpSettings.frmMain.customCaption;

	chbShowSpeedDialOnly->Checked = tmpSettings.frmMain.bSpeedDialOnly;
	chbSpeedDialPopupMenu->Checked = tmpSettings.frmMain.bSpeedDialPopupMenu;
	chbSpeedDialIgnorePresenceNote->Checked = tmpSettings.frmMain.bSpeedDialIgnorePresenceNote;
	chbSpeedDialIgnoreDialogInfoRemoteIdentity->Checked = tmpSettings.frmMain.bSpeedDialIgnoreDialogInfoRemoteIdentity;
	chbSpeedDialKeepPreviousDialogInfoRemoteIdentityIfMissing->Checked = tmpSettings.frmMain.bSpeedDialKeepPreviousDialogInfoRemoteIdentityIfMissing;
	chbSpeedDialIgnoreOrClearDialogInfoRemoteIdentityIfTerminated->Checked = tmpSettings.frmMain.bSpeedDialIgnoreOrClearDialogInfoRemoteIdentityIfTerminated;

	chbKioskMode->Checked = tmpSettings.frmMain.bKioskMode;
	chbHideSpeedDialToggleButton->Checked = tmpSettings.frmMain.bHideSpeedDialToggleButton;
	chbHideMouseCursor->Checked = tmpSettings.frmMain.bHideMouseCursor;

	chbFrmMainShowWhenAnsweringCall->Checked = tmpSettings.frmMain.bShowWhenAnsweringCall;
	chbFrmMainShowWhenMakingCall->Checked = tmpSettings.frmMain.bShowWhenMakingCall;

	cbSpeedDialSize->ItemIndex = tmpSettings.frmMain.iSpeedDialSize;
	for (int i=0; i<tmpSettings.frmMain.speedDialWidth.size(); i++)
	{
		vleSpeedDialColWidths->Values[i+1] = tmpSettings.frmMain.speedDialWidth[i];
	}
	chbXBtnMinimize->Checked = tmpSettings.frmMain.bXBtnMinimize;
	chbRestoreMainWindowOnIncomingCall->Checked = tmpSettings.frmMain.bRestoreOnIncomingCall;
    chbSingleInstance->Checked = tmpSettings.frmMain.bSingleInstance;

	chbLogToFile->Checked = tmpSettings.Logging.bLogToFile;
	chbLogFlush->Checked = tmpSettings.Logging.bFlush;
	chbLogMessages->Checked = tmpSettings.uaConf.logMessages;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cmbMaxUiLogLines->Items->Strings[i]) >= tmpSettings.Logging.iMaxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
	}
	cbLogMaxFileSize->Text = tmpSettings.Logging.iMaxFileSize;
	cbLogRotate->ItemIndex = tmpSettings.Logging.iLogRotate;

	chbUserOnlyClip->Checked = tmpSettings.Display.bUserOnlyClip;
	chbDecodeUtfDisplayToAnsi->Checked = tmpSettings.Display.bDecodeUtfDisplayToAnsi;
	chbUsePAssertedIdentity->Checked = tmpSettings.Display.bUsePAssertedIdentity;
	edDialpadBackgroundImage->Text = tmpSettings.frmMain.dialpadBackgroundImage;
	edMainIconFile->Text = tmpSettings.frmMain.mainIcon;
	edTrayNotificationImage->Text = tmpSettings.frmMain.trayNotificationImage;
	chbNoBeepOnEnterKey->Checked = tmpSettings.frmMain.bNoBeepOnEnterKey;
	chbHideSettings->Checked = tmpSettings.frmMain.bHideSettings;
	chbHideViewMenu->Checked = tmpSettings.frmMain.bHideView;
	chbHideToolsMenu->Checked = tmpSettings.frmMain.bHideTools;
	chbHideHelpMenu->Checked = tmpSettings.frmMain.bHideHelp;

	chbCustomUserAgent->Checked = tmpSettings.uaConf.customUserAgent;
	edUserAgent->Text = tmpSettings.uaConf.userAgent.c_str();

	chAddFilterWMCopyData->Checked = tmpSettings.Integration.bAddFilterWMCopyData;
	edProtocolName->Text = tmpSettings.Integration.asProtocol;
	chbDoNotUseSipPrefixForDirectIpCalls->Checked = tmpSettings.Integration.bDoNotUseSipPrefixForDirectIpCalls;

	cbSipAccessUrlMode->ItemIndex = tmpSettings.SipAccessUrl.accessMode;

	lboxAudioCodecsAvailable->Clear();
	std::vector<AnsiString> codecs;
	if (Ua::Instance().GetAudioCodecList(codecs) == 0)
	{
		for (unsigned int i=0; i<codecs.size(); i++)
		{
			lboxAudioCodecsAvailable->Items->Add(codecs[i]);
		}
	}
	lboxAudioCodecsEnabled->Clear();
	if (tmpSettings.uaConf.accounts.size() > 0)
	{
        /** \todo multiple accounts */
		for (unsigned int i=0; i<tmpSettings.uaConf.accounts[0].audio_codecs.size(); i++)
		{
			AnsiString name = tmpSettings.uaConf.accounts[0].audio_codecs[i].c_str();
			for (int codec = 0; codec < lboxAudioCodecsAvailable->Items->Count; codec++)
			{
				if (lboxAudioCodecsAvailable->Items->Strings[codec] == name)
				{
					lboxAudioCodecsAvailable->Items->Delete(codec);
					lboxAudioCodecsEnabled->Items->Add(name);
					break;
				}
			}
		}
	}

	chbRecordingEnabled->Checked = tmpSettings.uaConf.recording.enabled;
	cbRecDirType->ItemIndex = tmpSettings.uaConf.recording.recDir;
	cbRecDirTypeChange(NULL);	
	this->edCustomRecDir->Text = tmpSettings.uaConf.recording.customRecDir.c_str();
	cbRecordingChannels->ItemIndex = tmpSettings.uaConf.recording.channels - 1;
	cbRecordingChannelsChange(NULL);
	if (tmpSettings.uaConf.recording.side < cbRecordedSide->Items->Count)
	{
		cbRecordedSide->ItemIndex = tmpSettings.uaConf.recording.side;
    }
	cbRecordingStart->ItemIndex = tmpSettings.uaConf.recording.recStart;

	chbAutoAnswer->Checked = tmpSettings.uaConf.autoAnswer;
	edAutoAnswerCode->Text = tmpSettings.uaConf.autoAnswerCode;
	edAutoAnswerDelayMin->Text = tmpSettings.uaConf.autoAnswerDelayMin;
	edAutoAnswerDelayMax->Text = tmpSettings.uaConf.autoAnswerDelayMax;

	chbAutoAnswerCallInfo->Checked = tmpSettings.uaConf.autoAnswerCallInfo;
	edAutoAnswerCallInfoDelayMin->Text = tmpSettings.uaConf.autoAnswerCallInfoDelayMin;

	chbAnswerOnEventTalk->Checked = tmpSettings.uaConf.answerOnEventTalk;

	chbHandleOodRefer->Checked = tmpSettings.uaConf.handleOodRefer;

	memoCallsExtraHeaderLines->Text = tmpSettings.Calls.extraHeaderLines;
	chbDisconnectCallOnAudioError->Checked = tmpSettings.Calls.bDisconnectCallOnAudioError;

	edWebRtcAecMsInSndCardBuf->Text = tmpSettings.uaConf.webrtcAec.msInSndCardBuf;
	edWebRtcAecSkew->Text = tmpSettings.uaConf.webrtcAec.skew;

	chbAudioPreprocessingTxEnabled->Checked = tmpSettings.uaConf.audioPreprocTx.enabled;
	chbAudioPreprocessingTxDenoiseEnabled->Checked = tmpSettings.uaConf.audioPreprocTx.denoiseEnabled;
	chbAudioPreprocessingTxAgcEnabled->Checked = tmpSettings.uaConf.audioPreprocTx.agcEnabled;
	chbAudioPreprocessingTxVadEnabled->Checked = tmpSettings.uaConf.audioPreprocTx.vadEnabled;
	chbAudioPreprocessingTxDereverbEnabled->Checked = tmpSettings.uaConf.audioPreprocTx.dereverbEnabled;
	edAudioPreprocessingTxAgcLevel->Text = tmpSettings.uaConf.audioPreprocTx.agcLevel;

	AudioPreprocessingUpdate();

	edRingDefault->Text = tmpSettings.Ring.defaultRing;
	edRingBellcoreDr1->Text = tmpSettings.Ring.bellcore[0];
	edRingBellcoreDr2->Text = tmpSettings.Ring.bellcore[1];
	edRingBellcoreDr3->Text = tmpSettings.Ring.bellcore[2];
	edRingBellcoreDr4->Text = tmpSettings.Ring.bellcore[3];
	edRingBellcoreDr5->Text = tmpSettings.Ring.bellcore[4];
	edRingBellcoreDr6->Text = tmpSettings.Ring.bellcore[5];
	edRingBellcoreDr7->Text = tmpSettings.Ring.bellcore[6];
	edRingBellcoreDr8->Text = tmpSettings.Ring.bellcore[7];

	chbContactPopupShowOnIncoming->Checked = tmpSettings.frmContactPopup.showOnIncoming;
	chbContactPopupShowOnOutgoing->Checked = tmpSettings.frmContactPopup.showOnOutgoing;

	edContactHttpQuery->Text = tmpSettings.HttpQuery.url;
	cbHttpQueryOpenMode->ItemIndex = tmpSettings.HttpQuery.openMode;

	chbContactFilterUsingNote->Checked = tmpSettings.Contacts.filterUsingNote;

	chbHistoryNoStoreToFile->Checked = tmpSettings.History.bNoStoreToFile;
	chbHistoryUsePaiForDisplayIfAvailable->Checked = tmpSettings.History.bUsePaiForDisplayIfAvailable;
	chbHistoryUsePaiForDialIfAvailable->Checked = tmpSettings.History.bUsePaiForDialIfAvailable;

    edScriptOnMakeCallFile->Text = tmpSettings.Scripts.onMakeCall;
	edScriptOnCallStateChangeFile->Text = tmpSettings.Scripts.onCallState;
	edScriptOnStreamingStateChangeFile->Text = tmpSettings.Scripts.onStreamingState;
	edScriptOnRegistrationStateChangeFile->Text = tmpSettings.Scripts.onRegistrationState;
	edScriptOnTimerFile->Text = tmpSettings.Scripts.onTimer;
	edScriptTimer->Text = tmpSettings.Scripts.timer;
	edScriptOnStartupFile->Text = tmpSettings.Scripts.onStartup;
	edScriptOnDialogInfoFile->Text = tmpSettings.Scripts.onDialogInfo;
	edScriptOnDialFile->Text = tmpSettings.Scripts.onDial;
	edScriptOnProgrammableButtonFile->Text = tmpSettings.Scripts.onProgrammableButton;
	edScriptOnAudioErrorFile->Text = tmpSettings.Scripts.onAudioDeviceError;

	chbShowSettingsIfAnyAccountSettingsIsHidden->Checked = tmpSettings.frmMain.bShowSettingsIfAccountSettingIsHidden;

	frmHotkeys->SetCfg(&tmpSettings.hotKeyConf);

	frmPhones->SetCfg(&tmpSettings.phoneConf);
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	tmpSettings.gui.scalingPct = StrToIntDef(edGuiScaling->Text, 100);
	if (tmpSettings.gui.scalingPct < tmpSettings.gui.SCALING_MIN ||
		tmpSettings.gui.scalingPct > tmpSettings.gui.SCALING_MAX)
	{
		tmpSettings.gui.scalingPct = 100;
	}

	int iHeight = StrToIntDef(edMainWindowHeight->Text, tmpSettings.frmMain.iHeight);
	if (iHeight >= Settings::_frmMain::MIN_HEIGHT)
	{
		tmpSettings.frmMain.iHeight = iHeight;
	}

	tmpSettings.frmMain.bAlwaysOnTop = chbAlwaysOnTop->Checked;
	tmpSettings.frmMain.bStartMinimizedToTray = chbStartMinimizedToTray->Checked;
	tmpSettings.frmTrayNotifier.showOnIncoming = chbTrayNotifierShowOnIncoming->Checked;
	tmpSettings.frmTrayNotifier.skipIfMainWindowVisible = chbSkipTrayNotifierIfMainWindowVisible->Checked;
	tmpSettings.frmTrayNotifier.showOnOutgoing = this->chbTrayNotifierShowOnOutgoing->Checked;
	tmpSettings.frmTrayNotifier.backgroundImage = edTrayNotifierBackgroundImage->Text;
	tmpSettings.frmTrayNotifier.scalingPct = StrToIntDef(edTrayNotifierGuiScaling->Text, tmpSettings.frmTrayNotifier.scalingPct);
	if (tmpSettings.frmTrayNotifier.scalingPct < Settings::_frmTrayNotifier::SCALING_MIN ||
		tmpSettings.frmTrayNotifier.scalingPct > Settings::_frmTrayNotifier::SCALING_MAX)
	{
		tmpSettings.frmTrayNotifier.scalingPct = Settings::_frmTrayNotifier::SCALING_DEF;
	}
	tmpSettings.frmTrayNotifier.hideWhenAnsweringCall = chbTrayNotifierHideWhenAnsweringCall->Checked;
	tmpSettings.frmTrayNotifier.hideWhenAnsweringCallAutomatically = chbTrayNotifierHideWhenAnsweringCallAutomatically->Checked;
	tmpSettings.frmMain.bUseCustomApplicationTitle = chbFrmMainUseCustomApplicationTitle->Checked;
	tmpSettings.frmMain.customApplicationTitle = edFrmMainCustomApplicationTitle->Text;
	tmpSettings.frmMain.bUseCustomCaption = chbFrmMainUseCustomCaption->Checked;
	tmpSettings.frmMain.customCaption = edFrmMainCustomCaption->Text;

	tmpSettings.frmMain.bSpeedDialOnly = chbShowSpeedDialOnly->Checked;
	tmpSettings.frmMain.bSpeedDialPopupMenu = chbSpeedDialPopupMenu->Checked;
	tmpSettings.frmMain.bSpeedDialIgnorePresenceNote = chbSpeedDialIgnorePresenceNote->Checked;
	tmpSettings.frmMain.bSpeedDialIgnoreDialogInfoRemoteIdentity = chbSpeedDialIgnoreDialogInfoRemoteIdentity->Checked;
	tmpSettings.frmMain.bSpeedDialKeepPreviousDialogInfoRemoteIdentityIfMissing = chbSpeedDialKeepPreviousDialogInfoRemoteIdentityIfMissing->Checked;
	tmpSettings.frmMain.bSpeedDialIgnoreOrClearDialogInfoRemoteIdentityIfTerminated = chbSpeedDialIgnoreOrClearDialogInfoRemoteIdentityIfTerminated->Checked;

	tmpSettings.frmMain.bKioskMode = chbKioskMode->Checked;
	tmpSettings.frmMain.bHideSpeedDialToggleButton = chbHideSpeedDialToggleButton->Checked;
	tmpSettings.frmMain.bHideMouseCursor = chbHideMouseCursor->Checked;

	tmpSettings.frmMain.bShowWhenAnsweringCall = chbFrmMainShowWhenAnsweringCall->Checked;
	tmpSettings.frmMain.bShowWhenMakingCall = chbFrmMainShowWhenMakingCall->Checked;

	tmpSettings.frmMain.iSpeedDialSize = cbSpeedDialSize->ItemIndex;

	for (int i=0; i<tmpSettings.frmMain.speedDialWidth.size(); i++)
	{
		tmpSettings.frmMain.speedDialWidth[i] = StrToIntDef(vleSpeedDialColWidths->Values[i+1], tmpSettings.frmMain.speedDialWidth[i]);
	}

	tmpSettings.frmMain.bXBtnMinimize = chbXBtnMinimize->Checked;
	tmpSettings.frmMain.bRestoreOnIncomingCall = chbRestoreMainWindowOnIncomingCall->Checked;
	tmpSettings.frmMain.bSingleInstance = chbSingleInstance->Checked;

	tmpSettings.Logging.bLogToFile = chbLogToFile->Checked;
	tmpSettings.Logging.bFlush = chbLogFlush->Checked;
	tmpSettings.Logging.iMaxFileSize = StrToIntDef(cbLogMaxFileSize->Text, tmpSettings.Logging.iMaxFileSize);
	if (tmpSettings.Logging.iMaxFileSize < Settings::_Logging::MIN_MAX_FILE_SIZE || tmpSettings.Logging.iMaxFileSize > Settings::_Logging::MIN_MAX_FILE_SIZE)
	{
		tmpSettings.Logging.iMaxFileSize = Settings::_Logging::DEF_MAX_FILE_SIZE;
	}
	tmpSettings.Logging.iLogRotate = cbLogRotate->ItemIndex;
	tmpSettings.Logging.iMaxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
	tmpSettings.uaConf.logMessages = chbLogMessages->Checked;

	tmpSettings.Display.bUserOnlyClip = chbUserOnlyClip->Checked;
	tmpSettings.Display.bDecodeUtfDisplayToAnsi = chbDecodeUtfDisplayToAnsi->Checked;
	tmpSettings.Display.bUsePAssertedIdentity = chbUsePAssertedIdentity->Checked;

	tmpSettings.frmMain.dialpadBackgroundImage = edDialpadBackgroundImage->Text;
	tmpSettings.frmMain.mainIcon = edMainIconFile->Text;
	tmpSettings.frmMain.trayNotificationImage = edTrayNotificationImage->Text;
	tmpSettings.frmMain.bNoBeepOnEnterKey = chbNoBeepOnEnterKey->Checked;
	tmpSettings.frmMain.bHideSettings = chbHideSettings->Checked;
	tmpSettings.frmMain.bHideView = chbHideViewMenu->Checked;
	tmpSettings.frmMain.bHideTools = chbHideToolsMenu->Checked;
	tmpSettings.frmMain.bHideHelp = chbHideHelpMenu->Checked;

	tmpSettings.uaConf.customUserAgent = chbCustomUserAgent->Checked;
	tmpSettings.uaConf.userAgent = edUserAgent->Text.c_str();

	tmpSettings.Integration.bAddFilterWMCopyData = chAddFilterWMCopyData->Checked;
	tmpSettings.Integration.asProtocol = edProtocolName->Text;
	tmpSettings.Integration.bDoNotUseSipPrefixForDirectIpCalls = chbDoNotUseSipPrefixForDirectIpCalls->Checked;

	tmpSettings.SipAccessUrl.accessMode = static_cast<Settings::_SipAccessUrl::AccessMode>(cbSipAccessUrlMode->ItemIndex);

	const char* ptr = NULL;
	if (cbSoundInputMod->ItemIndex == 0)
	{
		ptr = UaConf::modPortaudio;
	}
	else if (cbSoundInputMod->ItemIndex == 1)
	{
		ptr = UaConf::modWinwave;
	}
	else if (cbSoundInputMod->ItemIndex == 2)
	{
    	ptr = UaConf::modAufile;
	}
	else
	{
        ptr = UaConf::modNullaudio;
    }
	strncpyz(tmpSettings.uaConf.audioCfgSrc.mod, ptr, sizeof(tmpSettings.uaConf.audioCfgSrc.mod));
	strncpyz(tmpSettings.uaConf.audioCfgSrc.dev, cbSoundInputDev->Text.c_str(), sizeof(tmpSettings.uaConf.audioCfgSrc.dev));
	strncpyz(tmpSettings.uaConf.audioCfgSrc.wavefile, edSoundInputWave->Text.c_str(), sizeof(tmpSettings.uaConf.audioCfgSrc.wavefile));

	ptr = GetOutputModuleForCbIndex(cbSoundOutputMod->ItemIndex);
	strncpyz(tmpSettings.uaConf.audioCfgPlay.mod, ptr, sizeof(tmpSettings.uaConf.audioCfgPlay.mod));
	strncpyz(tmpSettings.uaConf.audioCfgPlay.dev, cbSoundOutputDev->Text.c_str(), sizeof(tmpSettings.uaConf.audioCfgPlay.dev));

	ptr = GetOutputModuleForCbIndex(cbSoundAlertOutputMod->ItemIndex);
	strncpyz(tmpSettings.uaConf.audioCfgAlert.mod, ptr, sizeof(tmpSettings.uaConf.audioCfgAlert.mod));
	strncpyz(tmpSettings.uaConf.audioCfgAlert.dev, cbSoundAlertOutputDev->Text.c_str(), sizeof(tmpSettings.uaConf.audioCfgAlert.dev));

	ptr = GetOutputModuleForCbIndex(cbSoundRingOutputMod->ItemIndex);
	strncpyz(tmpSettings.uaConf.audioCfgRing.mod, ptr, sizeof(tmpSettings.uaConf.audioCfgRing.mod));
	strncpyz(tmpSettings.uaConf.audioCfgRing.dev, cbSoundRingOutputDev->Text.c_str(), sizeof(tmpSettings.uaConf.audioCfgRing.dev));

	ptr = GetOutputModuleForCbIndex(cbSoundOutputIntercomMod->ItemIndex);
	strncpyz(tmpSettings.uaConf.audioCfgPlayIntercom.mod, ptr, sizeof(tmpSettings.uaConf.audioCfgPlayIntercom.mod));
	strncpyz(tmpSettings.uaConf.audioCfgPlayIntercom.dev, cbSoundOutputIntercomDev->Text.c_str(), sizeof(tmpSettings.uaConf.audioCfgPlayIntercom.dev));


	tmpSettings.uaConf.aec = (UaConf::Aec)cbAec->ItemIndex;

	tmpSettings.uaConf.local = edLocalAddress->Text.c_str();
	tmpSettings.uaConf.ifname = edIfName->Text.c_str();
	tmpSettings.uaConf.avt.portMin = StrToIntDef(edRtpPortMin->Text, 0);
	tmpSettings.uaConf.avt.portMax = StrToIntDef(edRtpPortMax->Text, 0);
	if (tmpSettings.uaConf.avt.ValidatePorts())
	{
		ShowMessage("Invalid RTP ports configuration!");
		return;
	}
	tmpSettings.uaConf.avt.jbufDelayMin = StrToIntDef(edJbufDelayMin->Text, 0);
	tmpSettings.uaConf.avt.jbufDelayMax = StrToIntDef(edJbufDelayMax->Text, 0);
	if (tmpSettings.uaConf.avt.ValidateJbufDelay())
	{
		ShowMessage("Invalid jitter buffer delay configuration!");
		return;
	}
	tmpSettings.uaConf.avt.rtpTimeout = StrToIntDef(edRtpTimeout->Text, -1);
	if (tmpSettings.uaConf.avt.ValidateRtpTimeout())
	{
		ShowMessage("Invalid RTP timeout configuration!");
		return;
	}

	tmpSettings.uaConf.accounts[0].audio_codecs.clear();
	for (int i=0; i<lboxAudioCodecsEnabled->Items->Count; i++)
	{
		tmpSettings.uaConf.accounts[0].audio_codecs.push_back(lboxAudioCodecsEnabled->Items->Strings[i].c_str());
	}

	tmpSettings.uaConf.recording.enabled = chbRecordingEnabled->Checked;
	tmpSettings.uaConf.recording.recDir = static_cast<UaConf::RecordingCfg::RecDir>(cbRecDirType->ItemIndex);
	tmpSettings.uaConf.recording.customRecDir = edCustomRecDir->Text.c_str();
	tmpSettings.uaConf.recording.channels = cbRecordingChannels->ItemIndex + 1;
	tmpSettings.uaConf.recording.side = cbRecordedSide->ItemIndex;
	tmpSettings.uaConf.recording.recStart = static_cast<UaConf::RecordingCfg::RecStart>(cbRecordingStart->ItemIndex);

   	tmpSettings.uaConf.autoAnswer = chbAutoAnswer->Checked;
	tmpSettings.uaConf.autoAnswerCode = StrToIntDef(edAutoAnswerCode->Text, 200);
	tmpSettings.uaConf.autoAnswerDelayMin = StrToIntDef(edAutoAnswerDelayMin->Text, 0);
	tmpSettings.uaConf.autoAnswerDelayMax = StrToIntDef(edAutoAnswerDelayMax->Text, 0);
	if (tmpSettings.uaConf.autoAnswerDelayMin > tmpSettings.uaConf.autoAnswerDelayMax)
	{
		tmpSettings.uaConf.autoAnswerDelayMin = 0;
		tmpSettings.uaConf.autoAnswerDelayMax = 0;		
	}

	tmpSettings.uaConf.autoAnswerCallInfo = chbAutoAnswerCallInfo->Checked;
	int val = StrToIntDef(edAutoAnswerCallInfoDelayMin->Text, 0);
	if (val < 0)
		val = 0;
	tmpSettings.uaConf.autoAnswerCallInfoDelayMin = val;

	tmpSettings.uaConf.answerOnEventTalk = chbAnswerOnEventTalk->Checked;

	tmpSettings.uaConf.handleOodRefer = chbHandleOodRefer->Checked;

	tmpSettings.Calls.extraHeaderLines = memoCallsExtraHeaderLines->Text.Trim();
	tmpSettings.Calls.bDisconnectCallOnAudioError = chbDisconnectCallOnAudioError->Checked;

	tmpSettings.uaConf.webrtcAec.msInSndCardBuf = StrToIntDef(edWebRtcAecMsInSndCardBuf->Text, 120);
	if (tmpSettings.uaConf.webrtcAec.msInSndCardBuf < 0) {
    	tmpSettings.uaConf.webrtcAec.msInSndCardBuf = 120;
	}
	tmpSettings.uaConf.webrtcAec.skew = StrToIntDef(edWebRtcAecSkew->Text, 0);

	tmpSettings.uaConf.audioPreprocTx.enabled = chbAudioPreprocessingTxEnabled->Checked;
	tmpSettings.uaConf.audioPreprocTx.denoiseEnabled = chbAudioPreprocessingTxDenoiseEnabled->Checked;
	tmpSettings.uaConf.audioPreprocTx.agcEnabled = chbAudioPreprocessingTxAgcEnabled->Checked;
	tmpSettings.uaConf.audioPreprocTx.vadEnabled = chbAudioPreprocessingTxVadEnabled->Checked;
	tmpSettings.uaConf.audioPreprocTx.dereverbEnabled = chbAudioPreprocessingTxDereverbEnabled->Checked;
	tmpSettings.uaConf.audioPreprocTx.agcLevel = StrToIntDef(edAudioPreprocessingTxAgcLevel->Text, UaConf::AudioPreproc::AGC_LEVEL_DEFAULT);

	tmpSettings.frmContactPopup.showOnIncoming = chbContactPopupShowOnIncoming->Checked;
	tmpSettings.frmContactPopup.showOnOutgoing = chbContactPopupShowOnOutgoing->Checked;

	tmpSettings.HttpQuery.url = edContactHttpQuery->Text;
	tmpSettings.HttpQuery.openMode = static_cast<Settings::_HttpQuery::OpenMode>(cbHttpQueryOpenMode->ItemIndex);

	tmpSettings.Contacts.filterUsingNote = chbContactFilterUsingNote->Checked;

	tmpSettings.History.bNoStoreToFile = chbHistoryNoStoreToFile->Checked;
	tmpSettings.History.bUsePaiForDisplayIfAvailable = chbHistoryUsePaiForDisplayIfAvailable->Checked;
	tmpSettings.History.bUsePaiForDialIfAvailable = chbHistoryUsePaiForDialIfAvailable->Checked;

	tmpSettings.Scripts.onMakeCall = edScriptOnMakeCallFile->Text;
	tmpSettings.Scripts.onCallState = edScriptOnCallStateChangeFile->Text;
	tmpSettings.Scripts.onStreamingState = edScriptOnStreamingStateChangeFile->Text;
	tmpSettings.Scripts.onRegistrationState = edScriptOnRegistrationStateChangeFile->Text;
	tmpSettings.Scripts.onTimer = edScriptOnTimerFile->Text;
	tmpSettings.Scripts.timer = StrToIntDef(edScriptTimer->Text, -1);
	if (tmpSettings.Scripts.timer <= 0)
		tmpSettings.Scripts.timer = 1000;
	tmpSettings.Scripts.onStartup = edScriptOnStartupFile->Text;
	tmpSettings.Scripts.onDialogInfo = edScriptOnDialogInfoFile->Text;
	tmpSettings.Scripts.onDial = edScriptOnDialFile->Text;
	tmpSettings.Scripts.onProgrammableButton = edScriptOnProgrammableButtonFile->Text;
	tmpSettings.Scripts.onAudioDeviceError = edScriptOnAudioErrorFile->Text;

	tmpSettings.frmMain.bShowSettingsIfAccountSettingIsHidden = chbShowSettingsIfAnyAccountSettingsIsHidden->Checked;

	appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------



void __fastcall TfrmSettings::btnAddAccountClick(TObject *Sender)
{
	UaConf::Account acc;
	tmpSettings.uaConf.accounts.push_back(acc);
	int i = tmpSettings.uaConf.accounts.size() - 1;

	//scrbAccounts->AutoScroll = false;

	frmAccount = new TfrmAccount(NULL, i, tmpSettings.uaConf.accounts[i]);
	frmAccount->Parent = scrbAccounts;
	frmAccount->Visible = true;
	// force realign
	frmAccount->Align = alBottom;
	frmAccount->Align = alTop;

	//scrbAccounts->AutoScroll = true;

	// scroll to newly added account position (to the end)
	//scrbAccounts->ScrollBy(0, scrbAccounts->Height);
	frmAccount->SetFocus();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::FormClose(TObject *Sender, TCloseAction &Action)
{
	for (int i=0; i<frmAccountVector.size(); i++)
	{
		delete frmAccountVector[i];
	}
	frmAccountVector.clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbSoundInputModChange(TObject *Sender)
{
	switch (cbSoundInputMod->ItemIndex)
	{
		case 0:
		case 1:
			btnSelectWaveFile->Visible = false;
			edSoundInputWave->Visible = false;
			cbSoundInputDev->Visible = true;
			lblSoundInputDevice->Visible = true;
			FillDevList(cbSoundInputDev, cbSoundInputMod->ItemIndex, false,
				tmpSettings.uaConf.audioCfgSrc.dev);
			break;
		case 2:
			btnSelectWaveFile->Visible = true;
			edSoundInputWave->Visible = true;
			cbSoundInputDev->Visible = false;
			lblSoundInputDevice->Visible = true;
			break;
		case 3:	// nullaudio
			btnSelectWaveFile->Visible = false;
			edSoundInputWave->Visible = false;
			cbSoundInputDev->Visible = false;
			lblSoundInputDevice->Visible = false;
			break;
		default:
			assert(!"Unhandled cbSoundInputMod item index!");
			break;
	}
}
//---------------------------------------------------------------------------

void TfrmSettings::ChangeSoundOutputMod(TComboBox *target, TLabel *label, int moduleIndex, AnsiString selected)
{
	switch (moduleIndex)
	{
		case 0:
		case 1:
			target->Visible = true;
			label->Visible = true;
			FillDevList(target, moduleIndex, true, selected);
			break;
		case 2:	// nullaudio
			target->Visible = false;
			label->Visible = false;
			break;
		default:
			assert(!"Unhandled output module index!");
	}
}

void __fastcall TfrmSettings::cbSoundOutputModChange(TObject *Sender)
{
	ChangeSoundOutputMod(cbSoundOutputDev, lblSoundOutputDev, cbSoundOutputMod->ItemIndex, tmpSettings.uaConf.audioCfgPlay.dev);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbSoundAlertOutputModChange(TObject *Sender)
{
	ChangeSoundOutputMod(cbSoundAlertOutputDev, lblSoundAlertOutputDev, cbSoundAlertOutputMod->ItemIndex, tmpSettings.uaConf.audioCfgAlert.dev);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbSoundRingOutputModChange(TObject *Sender)
{
	ChangeSoundOutputMod(cbSoundRingOutputDev, lblSoundRingOutputDev, cbSoundRingOutputMod->ItemIndex, tmpSettings.uaConf.audioCfgRing.dev);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbSoundOutputIntercomModChange(TObject *Sender)
{
	ChangeSoundOutputMod(cbSoundOutputIntercomDev, lblSoundOutputIntercomDev, cbSoundOutputIntercomMod->ItemIndex, tmpSettings.uaConf.audioCfgPlayIntercom.dev);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnCreateIntegrationProtocolClick(TObject *Sender)
{
	TRegistry* registry = NULL;
	AnsiString proto = edProtocolName->Text.Trim();
	if (proto.Length() == 0)
	{
        MessageBox(this->Handle, "Protocol name can not be empty", this->Caption.c_str(), MB_ICONINFORMATION);
		return;
	}
	try
	{
		registry = new TRegistry();
		registry->RootKey =  HKEY_CLASSES_ROOT;

		if (registry->OpenKey(proto, true))
		{
			AnsiString val, key;
			val.sprintf("URL:%s Protocol", proto.c_str());
			registry->WriteString("", val);
			registry->WriteInteger("BrowserFlags", 8);
			registry->WriteInteger("EditFlags", 2);
			registry->WriteString("URL Protocol", "");
			registry->CloseKey();

			key.sprintf("%s\\DefaultIcon", proto.c_str());
			if (registry->OpenKey(key, true))
			{
				val.sprintf("%s,0", Application->ExeName.c_str());
				registry->WriteString("", val);
				registry->CloseKey();
			}

			key.sprintf("%s\\shell", proto.c_str());
			if (registry->OpenKey(key, true))
			{
				registry->WriteString("", "open");
				registry->CloseKey();
			}

			key.sprintf("%s\\shell\\open\\command", proto.c_str());
			if (registry->OpenKey(key, true))
			{
				val.sprintf("%s /%s=\"%1\"", Application->ExeName.c_str(), Branding::appProto.c_str());
				if (LowerCase(Paths::GetProfileDir()) != LowerCase(ExtractFileDir(Application->ExeName)))
				{
                	val.cat_printf(" /profiledir=\"%s\"", Paths::GetProfileDir().c_str());
				}
				registry->WriteString("", val);
				registry->CloseKey();
			}
			MessageBox(this->Handle, "Registry updated", this->Caption.c_str(), MB_ICONINFORMATION);
		}
	}
	catch (...)
	{
        MessageBox(this->Handle, "Failed to create registry entries", this->Caption.c_str(), MB_ICONINFORMATION);
	}
	if (registry)
	{
    	delete registry;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnRingSelectClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton*>(Sender);
	assert(btn);
	TEdit *edit = NULL;
	AnsiString *str = NULL;
	if (btn == btnRingSelectDefault)
	{
		edit = edRingDefault;
		str = &tmpSettings.Ring.defaultRing;
	}
	else if (btn == btnRingSelectBellcoreDr1)
	{
		edit = edRingBellcoreDr1;
		str = &tmpSettings.Ring.bellcore[0];
	}
	else if (btn == btnRingSelectBellcoreDr2)
	{
		edit = edRingBellcoreDr2;
		str = &tmpSettings.Ring.bellcore[1];
	}
	else if (btn == btnRingSelectBellcoreDr3)
	{
		edit = edRingBellcoreDr3;
		str = &tmpSettings.Ring.bellcore[2];
	}
	else if (btn == btnRingSelectBellcoreDr4)
	{
		edit = edRingBellcoreDr4;
		str = &tmpSettings.Ring.bellcore[3];
	}
	else if (btn == btnRingSelectBellcoreDr5)
	{
		edit = edRingBellcoreDr5;
		str = &tmpSettings.Ring.bellcore[4];
	}
	else if (btn == btnRingSelectBellcoreDr6)
	{
		edit = edRingBellcoreDr6;
		str = &tmpSettings.Ring.bellcore[5];
	}
	else if (btn == btnRingSelectBellcoreDr7)
	{
		edit = edRingBellcoreDr7;
		str = &tmpSettings.Ring.bellcore[6];
	}
	else if (btn == btnRingSelectBellcoreDr8)
	{
		edit = edRingBellcoreDr8;
		str = &tmpSettings.Ring.bellcore[7];
	}
	dlgOpenRing->InitialDir = Paths::GetProfileDir();
	dlgOpenRing->FileName = Paths::GetProfileDir() + "\\" + edit->Text;
	if (dlgOpenRing->Execute())
	{
		if (UpperCase(Paths::GetProfileDir()) != UpperCase(ExtractFileDir(dlgOpenRing->FileName)))
		{
			MessageBox(this->Handle, "Ring file was not updated.\nFor portability ring WAVE files must be placed in application directory.", this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
		*str = ExtractFileName(dlgOpenRing->FileName);
		edit->Text = *str;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		this->Close();
	}	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbRecDirTypeChange(TObject *Sender)
{
	UaConf::RecordingCfg::RecDir dirType = static_cast<UaConf::RecordingCfg::RecDir>(cbRecDirType->ItemIndex);
	if (dirType == UaConf::RecordingCfg::RecDirRelative)
	{
		lblCustomRecDir->Visible = false;
		edCustomRecDir->Visible = false;
		btnSelectCustomRecDir->Visible = false;
	}
	else
	{
		lblCustomRecDir->Visible = true;
		edCustomRecDir->Visible = true;
		btnSelectCustomRecDir->Visible = true;
	}	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbRecordingChannelsChange(TObject *Sender)
{
	if (cbRecordingChannels->ItemIndex == 0)
	{
		lblRecordedSide->Visible = true;
		cbRecordedSide->Visible = true;
	}
	else
	{
		lblRecordedSide->Visible = false;
		cbRecordedSide->Visible = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnSelectCustomRecDirClick(TObject *Sender)
{
    AnsiString asDir;
	if (DirectoryExists(edCustomRecDir->Text))
		asDir = edCustomRecDir->Text;
	if (SelectDirectory("Recordings dir:", "", asDir))
		edCustomRecDir->Text = asDir;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnSelectWaveFileClick(TObject *Sender)
{
	dlgOpenRing->InitialDir = Paths::GetProfileDir();
	if (edSoundInputWave->Text != "")
	{
		dlgOpenRing->FileName = Paths::GetProfileDir() + "\\" + edSoundInputWave->Text;
	}
	if (dlgOpenRing->Execute())
	{
		if (UpperCase(Paths::GetProfileDir()) != UpperCase(ExtractFileDir(dlgOpenRing->FileName)))
		{
			MessageBox(this->Handle, "File was not updated.\nFor portability source WAVE files must be placed in application directory.", this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
		edSoundInputWave->Text = ExtractFileName(dlgOpenRing->FileName);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::tvSelectorChange(TObject *Sender, TTreeNode *Node)
{
	if (lastTab)
	{
		lastTab->Visible = false;
	}
	if (Node->Selected)
	{
		int id = Node->Index;
		assert(id < tabs.size());
		lastTab = tabs[id];
		lastTab->Visible = true;
    }	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnSelectImageClick(
      TObject *Sender)
{
	TEdit *edit;
	if (Sender == btnSelectDialpadBackgroundImage)
	{
		edit = edDialpadBackgroundImage;
		openDialog->Filter = "Bitmaps (*.bmp)|*.bmp|All files|*.*";
	}
	else if (Sender == btnSelectMainIconFile)
	{
		edit = edMainIconFile;
		openDialog->Filter = "Icon files (*.ico)|*.ico|All files|*.*";
	}
	else if (Sender == btnSelectTrayNotificationImage)
	{
		edit = edTrayNotificationImage;
		openDialog->Filter = "Bitmaps (*.bmp)|*.bmp|All files|*.*";
	}
	else if (Sender == btnTrayNotifierBackgroundImage)
	{
		edit = edTrayNotifierBackgroundImage;
		openDialog->Filter = "Bitmaps (*.bmp)|*.bmp|All files|*.*";
    }
	else
	{
        assert(!"Unhandler sender!");
		return;
	}
	AnsiString dir = Paths::GetProfileDir() + "\\img\\";
	openDialog->InitialDir = dir;
	if (FileExists(dir + edit->Text))
		openDialog->FileName = dir + edit->Text;
	else
		openDialog->FileName = "";
	if (openDialog->Execute())
	{
    	edit->Text = ExtractFileName(openDialog->FileName);
	}
}
//---------------------------------------------------------------------------

void TfrmSettings::AudioCodecEnableSelected(void)
{
	for (int i=0; i<lboxAudioCodecsAvailable->Items->Count; i++)
	{
		if (lboxAudioCodecsAvailable->Selected[i])
		{
			lboxAudioCodecsEnabled->Items->Add(lboxAudioCodecsAvailable->Items->Strings[i]);
		}
	}
	lboxAudioCodecsAvailable->DeleteSelected();
}

void TfrmSettings::AudioCodecDisableSelected(void)
{
	for (int i=0; i<lboxAudioCodecsEnabled->Items->Count; i++)
	{
		if (lboxAudioCodecsEnabled->Selected[i])
		{
			lboxAudioCodecsAvailable->Items->Add(lboxAudioCodecsEnabled->Items->Strings[i]);
		}
	}
	lboxAudioCodecsEnabled->DeleteSelected();
}

void TfrmSettings::AudioPreprocessingUpdate(void)
{
	bool state = chbAudioPreprocessingTxEnabled->Checked;
	chbAudioPreprocessingTxDenoiseEnabled->Enabled = state;
	chbAudioPreprocessingTxAgcEnabled->Enabled = state;
	chbAudioPreprocessingTxVadEnabled->Enabled = state;
	chbAudioPreprocessingTxDereverbEnabled->Enabled = state;
	edAudioPreprocessingTxAgcLevel->Enabled = state;
	lblAudioPreprocessingTxAgcLevel->Enabled = state;
}

void __fastcall TfrmSettings::btnAudioCodecEnableClick(TObject *Sender)
{
	AudioCodecEnableSelected();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnAudioCodecDisableClick(TObject *Sender)
{
	AudioCodecDisableSelected();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::lboxAudioCodecsAvailableDblClick(TObject *Sender)
{
	AudioCodecEnableSelected();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::lboxAudioCodecsEnabledDblClick(TObject *Sender)
{
	AudioCodecDisableSelected();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::lboxAudioCodecsEnabledStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	audioCodecsEnabledDraggedIndex = lboxAudioCodecsEnabled->ItemIndex;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::lboxAudioCodecsEnabledDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	Accept = true;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::lboxAudioCodecsEnabledDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TListBox *lbox = lboxAudioCodecsEnabled;
	int dropIndex = lbox->ItemAtPos(Point(X,Y), false);
	if (dropIndex >= 0)
	{
		lbox->Items->Move(audioCodecsEnabledDraggedIndex, dropIndex);
		lbox->ItemIndex = dropIndex;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAudioPreprocessingTxEnabledClick(
      TObject *Sender)
{
	AudioPreprocessingUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnSelectedScriptClick(
      TObject *Sender)
{
	TEdit *edit = NULL;
	if (Sender == btnSelectedScriptOnMakeCallChange)
	{
        edit = edScriptOnMakeCallFile;
    }
	else if (Sender == btnSelectedScriptOnCallStateChange)
	{
		edit = edScriptOnCallStateChangeFile;
	}
	else if (Sender == btnSelectedScriptOnStreamingStateChange)
	{
		edit = edScriptOnStreamingStateChangeFile;
	}
	else if (Sender == btnSelectedScriptOnRegistrationStateChange)
	{
		edit = edScriptOnRegistrationStateChangeFile;
    }
	else if (Sender == btnSelectedScriptOnTimerChange)
	{
		edit = edScriptOnTimerFile;
	}
	else if (Sender == btnSelectedScriptOnStartupChange)
	{
		edit = edScriptOnStartupFile;
	}
	else if (Sender == btnSelectedScriptOnDialogInfoChange)
	{
        edit = edScriptOnDialogInfoFile;
	}
	else if (Sender == btnSelectedScriptOnDialChange)
	{
        edit = edScriptOnDialFile;
	}
	else if (Sender == btnSelectedScriptOnProgrammableButtonChange)
	{
    	edit = edScriptOnProgrammableButtonFile;
	}
	else if (Sender == btnSelectedScriptOnAudioErrorChange)
	{
    	edit = edScriptOnAudioErrorFile;
	}
	else
	{
		assert(0);
		return;
    }
	AnsiString dir = Paths::GetProfileDir() + "\\scripts";
	ForceDirectories(dir);
	openDialog->InitialDir = dir;
	openDialog->Filter = "Lua files (*.lua)|*.lua|All files|*.*";
	AnsiString fileName = dir + "\\" + edit->Text;
	if (FileExists(fileName))
		openDialog->FileName = fileName;
	else
		openDialog->FileName = "";
	if (openDialog->Execute())
	{
		if (UpperCase(dir) != UpperCase(ExtractFileDir(openDialog->FileName)))
		{
			MessageBox(this->Handle, "Entry was not updated.\nFor portability script files must be placed in \"script\" subdirectory.", this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
		edit->Text = ExtractFileName(openDialog->FileName);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmSettings::btnSelectedScriptEditClick(
      TObject *Sender)
{
	TEdit *edit = NULL;
	AnsiString eventName;
	if (Sender == btnSelectedScriptOnMakeCallEdit)
	{
		edit = edScriptOnMakeCallFile;
		eventName = "on_make_call";
	}
	else if (Sender == btnSelectedScriptOnCallStateEdit)
	{
		edit = edScriptOnCallStateChangeFile;
		eventName = "on_call_state";
	}
	else if (Sender == btnSelectedScriptOnStreamingStateEdit)
	{
		edit = edScriptOnStreamingStateChangeFile;
		eventName = "on_streaming_state";
	}
	else if (Sender == btnSelectedScriptOnRegistrationStateEdit)
	{
		edit = edScriptOnRegistrationStateChangeFile;
		eventName = "on_registration_state";
	}
	else if (Sender == btnSelectedScriptOnTimerEdit)
	{
		edit = edScriptOnTimerFile;
		eventName = "on_timer";
	}
	else if (Sender == btnSelectedScriptOnStartupEdit)
	{
		edit = edScriptOnStartupFile;
		eventName = "on_startup";
	}
	else if (Sender == btnSelectedScriptOnDialogInfoEdit)
	{
		edit = edScriptOnDialogInfoFile;
		eventName = "on_dialog_info";
	}
	else if (Sender == btnSelectedScriptOnDialEdit)
	{
		edit = edScriptOnDialFile;
		eventName = "on_dial";
	}
	else if (Sender == btnSelectedScriptOnProgrammableButtonEdit)
	{
		edit = edScriptOnProgrammableButtonFile;
		eventName = "on_programmable_button";
	}
	else if (Sender == btnSelectedScriptOnAudioErrorEdit)
	{
		edit = edScriptOnAudioErrorFile;
		eventName = "on_audio_error";
	}
	else
	{
		assert(0);
		return;
	}
	AnsiString dir = Paths::GetProfileDir() + "\\scripts";
	ForceDirectories(dir);
	AnsiString file = dir + "\\";
	if (edit->Text != "")
	{
		file += edit->Text;
	}
	else
	{
		AnsiString name = eventName + "_" + FormatDateTime("yyyymmdd_hhnnss_zzz", Now()) + ".lua";
		file += name;
		edit->Text = name;
	}
	if (!FileExists(file))
	{
		FILE *fp = fopen(file.c_str(), "wb");
		if (fp)
		{
			fclose(fp);
		}
		else
		{
			MessageBox(this->Handle, "Could not create file in \"script\" subdirectory.", this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
	}

	TfrmLuaScript *frmLuaScript = new TfrmLuaScript(NULL);
	frmLuaScript->Show();
	frmLuaScript->OpenFile(file);
}
//---------------------------------------------------------------------------




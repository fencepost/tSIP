/** \file
	\brief tSIP changelog
*/

/** \page changelog Changelog

Version 0.1.1:
	- lib project dependencies handled in a better way (#pragma link "re.lib",
	library search directory changed depending on current build configuration)
	- FIXED: possible Access Violation errors on quit or restart (caused by calling quit(0) from
	outside of worker thread)
	- added missing handling of transport and expires configuration parameters

Version 0.1.2
	- re/rem/baresip sources updated to version 0.4
	- winwave.c: fixes for small, non-recurrent memleaks
	- added baresip portaudio module and necessary PortAudio v19/DirectSound statically
	linked library; PortAudio is now default sound backend, WaveIn/WaveOut is left as
	an alternative
	- removed unnecessary parts from displayed call URI

Version 0.1.3
	- added speex codec module and library
	- added transmitted and received SIP message logging option
	- ua_find(): trying to match incoming requests using AOR if matching by Contact
	fails; matching by Contact only may cause interoperability problems
	(see <a href="http://www.developer.nokia.com/Community/Discussion/showthread.php?191636-Problems-with-incoming-VoIP-3.x-calls">
	Nokia: Problems with incoming VoIP 3.x calls</a>).

Version 0.1.4
	- re/rem/baresip sources updated to version 0.4.1
	- GUI: added auth username to configuration
	- GUI: fixed problem with temporary freezing when opening log window after long
	work time

Version 0.1.5
	- added baresip gsm module and statically linked gsm library
	- added module for G.726-32 codec and G.726 code from older SpanDSP version
	(copyright Sun Microsystems and Steve Underwood, public domain) 
	- added codec set configuration (enable/disable particular codecs)

Version 0.1.6
	- added local address setting (baresip "sip_listen" equivalent)

Version 0.1.7
	- updated re (0.4.5), rem (0.4.4), baresip (0.4.6) sources
	- log window: added "Save to file" to context menu and "Log to file" checkbox
	(duplicating settings)
	- added re-register button (forcing immediate re-registration)
	- eliminated GUI freezing when application was closing during registration
	that would fail due to no response from 2nd party
	- auto-answer with specified SIP code
	- added (very crude) call history
	- added redial button
	- making all: assumed that if sip: prefix is present uri domain
	does not have to be added (previously: unable to call
	i.e. sip:192.168.1.11 uri, call function was assuming that domain
	was not present if "@" in uri was not present)

Version 0.1.8
	- moved baresip code into separate static library (mostly to speed up
	often unnecessarly forced by TC++ whole project rebuilds)
	- dialpad: added A, B, C, D DTMFs
	- added Flash button (sent as DTMF event)
	- added Hold function
	- added blind transfer function

Version 0.1.9
	- Speed Dial panel with dialog-info subscription (BLF)

Version 0.1.10
	- FIXED: account configuration: password is not not required
	- FIXED: FLASH is no longer displayed as 'R' when dialing
	- FIXED: inconsistent application state when UA was restarted (configuration changed) during a call
	- making call with [Enter] in number edit field
	- auto-repetition for "backspace" button

Version 0.1.11
Added Accept header to SUBSCRIBE message.
Although it shouldn't be required (see RFC6665, 3.1.3),
Asterisk 11.9.0 is dropping these type of message with error: WARNING[14547]: chan_sip.c:27847 handle_request_subscribe: SUBSCRIBE failure: no Accept header: pvt: stateid: -1, laststate: 0, dialogver: 0, subscribecont: '', subscribeuri: ''.
Thanks to Barry Mercer for reporting.

Version 0.1.12
	- WebRTC Acoustic Echo Canceller
	- AEC configuration: none/Speex/WebRTC
	- fixed: problem with audio quality with WaveIn/WaveOut interface on some PCs (waveInUnprepareHeader misordering)

Version 0.1.13
	- separate audio module selection for audio source, output and output for ringing

Version 0.1.14
	- crude contact list / phonebook
	- json-cpp code moved into separate static library
	- fixed status text when account changed to account without registration
	- added CALL_STATE_OUTGOING, UA_EVENT_CALL_OUTGOING (feedback before contacting 2nd party on outgoing call)
	- added CALL_EVENT_TRYING, UA_EVENT_CALL_TRYING (info on receiving SIP/100)

Version 0.1.15
	- fixed audio device selection for winwave output (default device was always opened)
	- tray icon
		- File/Minimize to tray
		- status text as tray hint
		- "X" button minimizes to tray
		- settings: Start minimized to tray
	- cleanup: Hangup()/CALL_STATE_CLOSED code duplication
	- simple tray notifier window, related settings added; 

Version 0.1.16
	- replaced str_error calls in SIP replies with fixed text (when replying to re-INVITE with image media only invalid message was generated with "Unknown error" followed by empty line)
	- fixed minor built problems: reference to missing module with webrtc,
	not all projects included in main project group, missing (strangely only
	with particular TC++ copy installed) mmsystem.h include
	- fixed: no ring tones when working directory was different than application
	directory (e.g. when using start softphone01\tSIP.exe from batch file)
	- clearing BLF icon from speed dial panel when BLF subscription is disabled
	- settings: delay for the auto answer, randomized
	(for fuzzing purposes) from specified range

Version 0.1.17
	- fixed audio device enumeration in settings window
	- GUI scaling (main window)
	- intercom (separate, e.g. connected to speaker (not earphone) audio output on incoming INVITE with Call-Info with answer-after)
	- winwave: fixed handle leaks (waveInUnprepareHeader called after waveInClose, with invalid dev handle),
	problem found with MS Application Verifier
	- configurable buttons
		- type
		- caption and basic number
		- height, margin top, margin bottom
		- images selection: image hidden, images for non-BLF button type, for BLF terminated/early/confirmed state
	- setting: double width for speed dial


Version 0.1.18
	- additional configuration for BLF buttons: number dialed when button is pressed
	can be changed to different than number for subscription and depend on subscribed
	number state; this can be treated as generalized "Call pickup" function available
	e.g. on Yealink phones (to get identical behavior mark "early" for override and
	enter complete number that has to be used to pickup call from other extension on PABX)
	- new button type: unsolicited MWI (dislaying number of new and old messages on voice mail);
	image shown when there are new messages is shared with BLF "early" setting
	- new button type: MUTE

Version 0.1.19
	- browser integration
		- settings:
		- settings: WM_COPYDATA filter for elevated process (see MSDN: ChangeWindowMessageFilter) - this allows
		to pass command to softphone instance that is running with administrator privileges, most likely
		this would not be needed outside of special cases like running application under BDS 2006 debugger
		that requires it

Version 0.1.20
	- fixed command line execution when other instance was not found (accessing released memory)
	- settings: console-only mode

Version 0.1.21
	- changed registration expires in default configuration to 0; this disables registration
	by default - deregistration attempt from non-existent server (long timeout) is annoying
	and it's not very likely that default registration server address (i.e. localhost)
	would match user registrar
	- moved ring initialization to top level project (fixes issue with short
	ringtone being played even with auto-answer with zero delay)
	- separate ringtones selection for INVITEs with Alert-Info: info=<Bellcore-dr1> header (Bellcore-dr1 ... Bellcore-dr8)
	and for default ring; note: wav files have to be placed in application directory
	- new function and button type: mute ring
	- settings: action for "X" button in main window: minimize to tray or close

Version 0.1.22
	- FIXED: SIP/500 response was sent since v0.1.21 on missing Alert-Info
	- new setting for BLF button: action when button is pressed during call:
		- none (= no action, same as in previous version)
		- DTMF dialing with optional prefix to be added (prefix can be PABX transfer code
		or even "R" for FLASH if PABX supports it)
		- blind transfer using REFER (default)
	- (again) more console columns (up to 5 + 1 "basic") giving up to 55 buttons (up to 75 with reduced button height);
	minor cleanup to make adding more columns later easier (change EXT_CONSOLE_COLUMNS + add item to combobox in settings window)
	- version info: added file description (presented e.g. in Process Explorer and some firewalls)

Version 0.1.23
	- minor improvements to call history:
		- keeping only user part of URI from incoming call
		- size increased to 1000 entries
		- not skipping entries with duplicated numbers / URIs
		- storing call time
		- displaying call direction and completion
	- settings: shortcuts (local to the application)
		- hide application
		- answer/hangup call
		- equivalent of pressing configurable (console) button; for button ID check caption of button edit window
	- settings: width for console columns

Version 0.1.24
	- fixed console-only mode
	- switching to/from console-only mode does not require restart
	- note: you can use 0.0.0.0:XXXX to bind to specific port without entering any specific IP address
	- log: added timestamp to log entries with SIP sent/received messages
	- global hotkeys (hotkeys that work even when application is hidden);
	since it may be quite hard to find unique hotkey combinations use them
	sparingly - my recommendation would be using global hotkeys for show/hide app
	and answer/hangup and local hotkeys for other actions

Version 0.1.25
	- build options: instruction set changed to Pentium Pro for all projects and targets
	- TimeCounter.h: time measurement utility for basic testing
	- setting focus to call number edit box on startup and when application is called with
	global show/hide hotkey (allowing dialing number without using mouse while keeping application
	hidden normally)
	- hotkeys: new action available: redial (although same effect can be
	achieved by assigning redial function to one of the speed dial keys
	and then assigning this button as action)
	- fixed: unitialized values for new hotkey button (i.e. "global" checkbox
	often set by default)
	- fixed: halt in global buttons unregister loop
	- contacts: note field added
	- contact note popup window (auto popup on incoming and/or outgoing call if note is not empty and specified option is set);
	window position and size is stored in configuration
	- new button type: show contact note popup

Version 0.1.25.1
	- fixed regression (0.1.25): "Cannot focus a disabled or invisible window" exception when starting minimized to tray
	- Branding.[h|c] - common application name/protocol handler definition to reduce differences between branded and regular version

Version 0.1.26
	- (very basic) call recording
		- settings: on/off switch (all calls are recorder automatically when enabled)
		- writing files to \recordings subdirectory
		- filename contains date, time, direction ("1" for incoming, "0" for outgoing) and 2nd party number/URI encoded in base64
		- output format: WAV, both direction mixed into single channel, PCM 8000sps, 16bit (~55MB per hour)

Version 0.1.27
	- recording can be disabled and hidden altogether (e.g. for legal reasons) without recompiling
	by opening executable with resource editor and changing "RECORDING_ENABLED" string to any
	other value; for extra protection from user manipulation Yoda's Protector or similar tool may be used
	- settings window can be closed with Esc key
	- new hotkey action type: call to number from clipboard (select number in web browser, hit Ctrl+C and
	than assigned global shortcut to call; I'd suggest using global hotkey with Ctrl, e.g. Ctrl + Browser Forward Key for this purpose)
	- fixed: global hotkeys not working with Win Vista and earlier systems because of MOD_NOREPEAT flag used;
	added OS detection and app-based antirepeat (with 1s timeout) for systems older than Win7
	- settings: restore minimized / bring main window to front on incoming call
	- settings: show tray notifier window on outgoing call (e.g. when using global hotkeys)

Version 0.1.28
	- DLL plugin system for interfacing external devices
		- implemented interface for EX-03 USB phone (aka @phone); most likely compatible
		also with EX-02 and few other similar devices 
	- FIXED: incorrect parameter (branding module not used) in protocol handler links
	- protocol handler: added "ANSWER" parameter (next to "HANGUP")

Version 0.1.29
	- FIXED: global hotkey unregistration and phone interface shutdown with "X" button when
	this button was configured to minimize to tray; added TfrmMain::Finalize()

Version 0.1.30
	- FIXED: regression: call history not saved on system shutdown (WM_ENDSESSION handler)
	- fixed handling of missing plugin DLL
	- contacts: CSV import

Version 0.1.31
	- contact note popup and contact node editor: URLs are highlighted and opened in default browser when clicked
	- recordings: custom directory can be set for output (e.g. located on shared network drive)
	- added exe location to "About" window (click to open application directory)
	- call duration is displayed in "Connected" state

Version 0.1.32, 0.1.33
	- recording: encoding of number/URI in wav file name changed to filesystems-safe RFC 4648 version of base64
	- new button type: HTTP query - open specified URL (e.g. search engine or web-based corporate database) with
	number from last (or current) incoming call passed as parameter; by default google is used
	- settings: added RTP/RTCP local port range selection

Version 0.1.34
	- presence subscription (application/pidf+xml) sharing images with BLF (new button type)

Version 0.1.35
	- presence button shares more settings with BLF: when pressed in-call works as blind transfer or DTMF sequence with optional prefix for attended transfer
	- new button type: UNREGISTER
	- recording: option to record calls in stereo, that is each call side into separate
	wave file channel; useful e.g. for echo and delay testing; note: this doubles resulting file size (effectively 256kbps, 110MB/h)

Version 0.1.36
	- fixed regression (0.1.35) in single channel call recording
	- added baresip aufile (wave file as sound source) adapted to Win32
        - presented as another (next to WaveIO and Portaudio) sound module for audio input
		- requires S16LE, 1ch, 8kSps wave file, located in application directory
		- call is disconnected at the end of file
		- allows making call with no system sound device (MB with jack auto detection)
		- poor man's voicemail (when combining announcement with silence at the end with call recording and auto answer)
	- ACTION_SHOWWINDOW (SHOWWINDOW command line parameter) - bring application from tray
	- new setting: single instance (bring previous instance to front if already running); uses SHOWWINDOW action and
	same mechanism as click-to-call

Version 0.1.36.1
	- FIXED: problem with ring and wave source files selection (case sensitivity)
	
Version 0.1.36.2
	- updated (C) date in About window
	- FIXED: in default configuration input device type was set to wave file (while file itself was not selected
	resulting in immediate call drop)
	- FIXED: NUL (0) characters were written in log file at the end of every line; strangely neither notepad nor notepad++ don't care about it

Version 0.1.37
	- new button type: execute arbitrary application; may be used to run RecordViewer or e.g. notepad passing
	number from last call as part of command line arguments or file name to open/edit

Version 0.1.38
	- new function: RTP streaming (paging, sender only)
		- new button type
		- target address can be specified as unicast or multicast (e.g. 192.168.1.10:4000)
		- for each button that starts streaming separate wave file to stream can be specified;
		if wave file is not specified then default audio source from configuration (e.g. microphone) is used
		- note: at the moment only G.711a can be used for streaming and enabling G.711a/u in configuration is required
		- extended plugin interface while keeping it backward compatible (optional functions to be exported by dlls)
			- SetPagingTxCallback - set function to be called by dll to start paging
			- SetPagingTxState - inform dll about started/stopped paging
	- account settings: added outbound proxy (equals to baresip outbound1 parameter for account but by convention - same as with SIP server - sip: prefix should not be included)
	- minor cleanup for account configuration window with separating
	basic settings (server, UDP/TCP, user, password and registration Expires/disabling)
	from optional extra settings

Version 0.1.39
	- FIXED regression (0.1.34): not working MWI
	- handling NOTIFY with "Event: talk" (answering call remotely, intended for interoperability with some CRM using asterisk AMI to control softphone), new setting in "Call" tab

Version 0.1.40
	- configuration of programmable buttons moved to separate file (tSIP_buttons.json)
	to reduce main configuration file grow and possibly make it easier to share keys configuration
	between users or application instances
	- if new keys config file is not found application will try to create it by
	moving settings from main config file; if earlier application version would be started
	next (downgrade) it would not find new settings so manual JSON merge may be needed
	- maximum number of side-car columns increased from 5 to 12 giving up to 132 programmable buttons
	(with reduced buttons height: up to 180); note that to use full 12 columns with default column width
	screen with 1440px width would be required
	- FIXED: problem with inconsistent side-car columns order when switching number of columns without restart
	- added "Reset all" button to edit window for programmable buttons
	- minor changes to programmable button edit window: separated visual settings
	- custom background colors (including system colors) can be assigned for programmable buttons (visual grouping for BLF)
	- FIXED: if programmable button with type = Disabled was clicked there was log line: "Unhandled BTN type = some_big_number"
	- SpeedDial button type shares with BLF and Presence setting for action when pressed during call (DTMF or transfer)

Version 0.1.41
	- FIXED: with message logging: message body was not displayed (only headers) for incoming messages
	- added action (= new possible keyboard shortcut): toggle sidecar
	- modified settings window (categories tree on the left side)
	- separated network settings (bind address, RTP ports)
	- Polycom-style Call-Info: Answer-After=X in incoming invite is recognized (in addition to previous Call-Info: <uri>;answer-after=X)
	- new account setting, "answer any"; if enabled account is treated as "local account" and used
	to process any incoming message that would be otherwise answered with SIP/404 if account for which
	it was intended was not found; intented to allow using softphone for some testing purposes like
	simulating PABX that is processing any call

Version 0.1.42
	- [Display Name] is used as description line if number was not found in phonebook
	- new setting: show only user part (if present) of URI as CLIP
	- Contacts: filter matches also phone numbers / URIs
	- added filtering to call history
	- call history stores display names from incoming INVITEs
	- call history displays contact names if available, then display names and finally numbers if names are empty
	- FIXED: memleak: contact_any
	- added G.722 codec; note: WebRTC AEC is currently disabled for 16kHz sampling

Version 0.1.43
	- added jitter buffer configuration - intended for special cases like using VoIP over poor quality VPN
	- new button type: SCRIPT (Lua scripting, number preprocessing)

Version 0.1.44
	- quick fix for problem with passwords containing "@" (problem with AOR decoding solved by passing password separately when creating account)
	- codec selection (PCMA/PCMU/G.722) for paging; make sure that selected codec is enabled in configuration
	- added (optional) network adapter selection that may help with other than expected SDP c line address if there are multiple network adapters installed: adapter list dump in log on startup,
	new setting in "Network" tab - use full adapter ID, on windows with curly brackets

Version 0.1.45
	- DLL modules (to be placed in "modules" subdirectory); only codec modules are handled at
	the moment
	- G.729 codec module built with Code::Blocks/MinGW
	- changing tray icon on missed call or new voice mail image (MWI)

Version 0.1.45.1
	- call history popup menu:
		- add/edit contact
		- copy number to clipboard
		- HTTP query (same as with programmable button, opening browser searching with google for number by default)
	- contact editor can be closed using Esc
	- 3 numbers per contact; first non-empty used as default when calling using double click, use popup menu to call using specific number
	- added "Company" to contacts
	- new setting: Contacts: Filter using contact note
	- [Enter] in contacts and history filters moves focus to list

Version 0.1.45.2	
	- minor fixes to GUI scaling + button images
	- changed handling of button images larger than 16x16 - images are centered but not cropped
	- button labels are transparent and on front thus button image can be used as sort of background
Version 0.1.45.3
	- added background image selection for dialpad
Version 0.1.46
	- turned off TabStop for dialpad 0...9*# buttons
	- FIXED: memleak when restarting stack (i.e. changing configuration) or closing application while paging is running
Version 0.1.46.1
	- Branding: using application name and default name for integration protocol from resources stringtable
Version 0.1.46.2
	- FIXED: for HTTP query function full SIP URI (not number) from current incoming call was used
Version 0.1.46.3
	- additional setting for HTTP query function: starting automatically on incoming call or when incoming call is accepted

Version 0.1.47
	- plugin interface: added GetNumberDescription() function (get phone number description from
	phone book or possibly other future data bank)
	- text-to-speech plugin (talking ringtone)

Version 0.1.47.1
	- new setting in "General" tab: "Do not beep when using [Enter] in main window to dial or transfer call" - this
	beep was unintentional but I guess someone might get used to it as a confirmation signal
	- new setting: "History"/"Do not store history to file"

Version 0.1.48
	- [BREAKING] codecs settings from previous version are not preserved; PCMU + PCMA are set as default
	- [BREAKING] codecs from dynamic modules (i.e. G.729) are not enabled by default after loading module
	- new, "correct" (as intented in baresip) way of configuring audio codecs, allowing
	independently enable/disable codecs if there are multiple codecs registered from single module
	and prioritize enabled codecs
	- codecs settings are now associated with account, although since number of accounts
	is limited to 1 at the moment - separate "Codecs" tab in settings is kept
	- codec selection for RTP streaming no longer requires separately enabling specified codec in configuration
	- added L16 codec (unchanged baresip module)
	- added L16/44100 to codec selection for RTP streaming; note that with default 20ms ptime these frames would be fragmented
	- added ptime setting to RTP paging
	- RTP timestamps are starting from random value and few other minor fixes from re/rem/baresip

Version 0.1.49
	- new settings in "General" page: custom main icon file (.ico) and custom tray icon image for unanswered calls notification (.bmp, 16x16), both from /img/ subdirectory;
	this is intended to help to distinguish multiple application instances (possibly using each instance for separate account) running simultaneously
	- background image can be assigned to tray notifier
	- tray notifier settings moved to separate page
	- new settings: "Hide settings" that may be used to protect configuration from end user, in particular if combined with restricting write rights for main configuration file

Version 0.1.50
	- support for Access-URL header in incoming INVITE (http://documents.polycom.com/topics/80391) - opening page specified in SIP message
    - settings: open manually, open depending on "mode" parameter or always open automatically	
	- new button type: "Access SIP URL" for opening received URL manually

Version 0.1.51
	- RTP timeout ("Network" tab) - disconnecting call when no RTP packets are received for specified time; improving resilience to network problems in scenarios when softphone is running unattended with auto answer
	- new button types: switching audio source and audio output during call ("speakerphone")

Version 0.1.52
	- included speex audio preprocessor module from baresip (i.e. speex-based denoise for microphone path)
	- settings: AEC card renamed to "Audio Processing", including AEC and audio preprocessor settings
	- adding audio filters to chain continues if filter installation fails for one of the filters
	(WebRTC EC supports only 8kHz codecs at the moment; if e.g. G.722 was used with WebRTC EC then recording was not running)
	- no onmouseover for programmable buttons (looking bad on touch screen when mouse cursor is not visible and button is still down), replaced with onmouseup/onmousedown
	- no focus for Call/Answer and Hangup buttons; assumption: local hotkey would be better way to terminate call than tab-cycling
	- settings: disabling context menu for speed dial panels / programmable keys in order to prevent accidental editing when using with touch screen
	- added L16/48000 to codecs set
	- settings: logging: log file flushing (disabled by default to reduce disk activity)
	- settings: logging: log file size limit, 10MB by default, resetting file if exceeded
	- settings: added button to refresh DLL plugin list

Version 0.1.53
	- audio source switching works also with RTP streaming (paging)
	- Lua scripting (integrated):
		- scripts can be executed on events (on call state change, on streaming state change - previously only when button was clicked)
		- added new setting page to assign event scripts
		- ScriptExec: context map for multiple scripts running simultaneously
		- added function SwitchAudioSource (e.g. SwitchAudioSource("aufile", "test.wav"))
		- added function SendDtmf (e.g. SendDtmf("1234*#))
		- added function BlindTransfer (e.g. BlindTransfer("123"))
		- added function GetCallState (returning int as in Callback::ua_state_e enum)
		- added function IsCallIncoming()
		- added function GetCallPeer() (returning either caller or called number)
		- added function GetStreamingState() (returning int as in Callback::paging_tx_state_e enum)
		- FIXED: added missing Sleep(miliseconds) function
		- added function set to pass variables (states) between script(s) executions:
			- SetVariable("name")
			- value, isset = GetVariable("name") (returning 2 variables, isset == 0 if variable with that name is not set)
			- ClearVariable("name")
			- ClearAllVariables()
		- added GetInitialCallTarget() and SetInitialCallTarget(number) functions allowing to override number dialed by the user and use softphone for call origination
	- settings/Speed Dial: option to ignore presence note (text from server displayed as second line)

Version 0.1.54
	- FIXED: making more than one simultaneous outgoing call was possible
	- FIXED: WebRTC AEC config was not read back from setting file
	- Lua scripting: added ShellExecute() function (avoiding command line that is displayed by os.execute())
	- Lua scripting: added "on timer" script

Version 0.1.54.1
	- settings: added option to set custom User-Agent

Version 0.1.55
	- Lua scripting: added GetRegistrationState() (returning values as in Callback::reg_state_e)
	- Lua scripting: added SetTrayIcon("fileFromImgSubdir.bmp"); on my PC it seems to require 8-bit bmp files for correct transparency handling
		- note: after first function run tSIP stops updating tray icon on its own (regular icon or unanswered call/MWI notification icon)
	- Lua scripting: added "on registration state" script
	- Lua: changed path search order to avoid CodeGuard problems in debug build
	- plugin interface: plugins can now set script variables
	- Lua scripting: added "Answer()" function
	- Lua scripting: added "MessageBox" function - direct equivalent of WinAPI function with same name but with window handle parameter skipped

Version 0.1.56
	- Lua:
		- added "on startup" (running once) script selection
		- added SetButtonCaption(btnId, text)
		- added SetButtonDown(btnId, state) - set button to "pressed" state or back
		- added SetButtonImage(btnId, file.bmp) - set image for for btn #btnId
		- added PluginSendMessageText(dllName, text) - method for passing data/control from script to plugin
		- added srcType, srcTypeIsSet = GetExecSourceType()	and srcId, srcIdIsSet = GetExecSourceId():
		function set that allows reading execution
		context (i.e. how the script was started, id of the button that script was associated to);
		this allows using same script file for multiple buttons
		- added InputQuery, allowing to take text input from the user, direct equivalent of VCL function with same name:
		text, isAccepted = InputQuery(caption, prompt, defaultText)
		- added GetRecordFile() function (returning name of call recording file, empty string if there is no recording);
		valid after call is established (recording started), cleared on new call, intented to be used mostly at CALL_STATE_CLOSED state
	- added command line APP_QUIT action (can be used as "tsip /tsip=APP_QUIT" from command line or from browser
	through tsip:APP_QUIT link); this action causes previous application instance to quit and is intented
	to help with provisioning based on swapping configuration files with downloaded ones (it should be done when application is not running, otherwise configuration file would be overwritten)
	- "Number" label and edit are not displayed when editing "Script" button

Version 0.1.56.1
	- Lua updated to version 5.3.4 (support for integers and bitwise operators)
	- Lua: added "on BLF (dialog-info) state" event script; numeric ID of the contact that changed state is passed through GetExecSourceId()
	- Lua: added function returning phone number and BLF state for specified contact ID
		(to be used inside "on BLF state" event script):
		number, state = GetBlfState(contactId)
	- Lua: added "tsip_winapi" built-in module, intented to hold some functions corresponding
	directly to Windows API and act as namespace; this module need to be imported with "require" and included functions are not put into global table directly
		- FindWindow
		- SendMessage
		- MessageBox (duplicating same function in global namespace at the moment)
		- Beep (duplicating same function in global namespace at the moment)

Version 0.1.56.2
	- added option to start recording in call progress (early media / ringback) state

Version 0.1.57
	- new setting, replacing option added in 0.1.56.2 (breaking compatibility): recording start
	can be set to manual (using Lua script), on call confirmed or on call progress
	- Lua: added function to manually start call recording (if not started automatically)
		status = RecordStart(filename, channels)
	where filename = full path and name for record file to create, channels (1 or 2) = mono or stereo (independent channels for call parties) recording
	- Lua: added GetExeName() function, returning full path and application exe name
	- Lua: added GetRecordingState() function, allowing to check if recording is running; might be used
	if few buttons / scripts able to start recording are defined

Version 0.1.57.1
	- added ptime configuration (Account tab, as in baresip)
	- Lua: added ForceDirectories(dir) function, same as VCL function but returning 0 on success - to be used before
	RecordStart() if target directory does not exist yet; requires fully-qualified path,
	usage: ForceDirectories("D:\\test\\test123")
	- button edit window can be closed using Esc

Version 0.1.57.2
	- dial buttons, Dialpad/Contacts/History tabs and sidecar open buttons no longer hold the focus (thus focus is always in number edit field allowing using keyboard for dialing and DTMFs all the time)
	- digits entered in number edit during call are generating DTMFs
Version 0.1.57.3
  - removed trailing newline from text returned by strerror()
  - fixed "Unknown error" log entries after e.g. normal call clearing by second party, added some more descriptive messages
Version 0.1.58
	- reason for registration failure (e.g. 404 Not Found) is shown in status bar (might not fit with long register server name
	but should be visible if sidecar is open)
	- added "on dial" script, running when user presses one of dialpad keys or is editing main number with keyboard; purpose: generating local audio feedback for dialed digits and DTMFs
Version 0.1.58.2
	- Lua: added GetCallInitialRxInvite() function, returning full text of first incoming INVITE of incoming call; allows to extract and process custom header lines from "on call state" script, e.g. using Remote-Party-ID to show extra CLIP on one of the buttons
	- Lua: added function description = GetContactName(number) to get contact description
Version 0.1.58.3
	- settings: added option to force "Contact" user (instead of default hex string)
Version 0.1.58.4
	- Lua: added GetRxDtmf() function, returning single DTMF sign (or empty string) from reception queue; sign is removed from queue
Version 0.1.59
	- added software volume control for both directions
Version 0.1.59.1
	- FIXED: inconsistent application state when handling incoming REFER (call being transferred)
Version 0.1.59.2
	- added simplified handler for out-of-dialog REFER - this is supposed to work with ANVEO CTI; as this seems
	little dangerous this is opt-in feature with new settings in "Calls" tab added;
	node: depending on setup using "Answer any" account option might be required
Version 0.1.59.4
	- added kiosk mode ("General" tab)
	- added new button types: show settings (blocked if "Hide settings" is used), minimize, show log and exit
	- increased max value allowed for GUI scaling setting to 500%
	- fixed scaling for phonebook and call history list columns width
	- added "Hide mouse cursor" option ("General" tab)
	- added option to set 2 caption lines for programmable buttons, e.g. description + number for BLF button
Version 0.1.60
	- added separate audio output device selection for ring
	- application version is stored in configuration to help with seemless upgrade (now: copying alert sound device to ring sound device for older version)
Version 0.1.60.3
	- added "nullaudio" input (generating silence) and output (discarding samples) audio device;
	intended to be used on machines with no audio I/O, e.g. with no microphone connected and autosensing jack,
	virtual machines or when running as service
Version 0.1.60.4
	- added "Display"/"Decode Display Name (UTF) to local (ANSI) code page" parameter

Version 0.1.61
	- attempt to remove bug related to TCP transport - reported 100% CPU load on disconnection,
	simingly fixed by additional conn_close(tc, 0) in tcp.c on recv() error
	- Settings/Calls: custom/extra header lines can be added to outgoing INVITEs (e.g. Call-Info with answer-after or Answer-After to make second party answer call automatically)

Version 0.1.61
	- increased configuration value limit for jitter buffer from 200 to 1000 frames
	- Settings window position changed from poDesktopCenter (in my configuration with two monitors: 1280x800 + 1920x1080 connected with VGA
	window was placed mostly on second screen even if it was turned off at the moment) to poScreenCenter

Version 0.1.62
	- removed "tool" line from SDP - suspected to be causing "488 SDP Parameter Error In SIP Request"
	error when calling using t-mobile.de operator
	- settings: added GUI scaling for tray notifier window
	- added Lua ShowTrayNotifier(description, uri, incoming) function (mainly for testing)
	- neither Answer nor Hangup button is active by default in tray notifier window
	- FIXED: bug in limiting tray notifier window position resulting in not restoring previous position if notifier was near the screen right/down edge
	- interoperability (supervoip.pl, some other German operator): using username as default contact name if contact name is empty instead of semi-random contact name based on memory address
	- "level" label in settings disabled for audio preprocessing
	- added hints for volume reset buttons
	- settings window moved from desktop center to screen center position
	- added Settings/Delete call history menu item (deleting immediately history file content and clearing items from dial combobox)
	- Log and "About" windows can be closed with Esc
	- if call is closed from reason other than normal disconnection - call state label displays reason (e.g. "488 Not Acceptable Here", "480 Temporarily Unavailable") for 8 seconds

Version 0.1.62.1
	- added options to hide "View" and "Help" menu (limiting access for the operator together with option to hide settings menu)
	- new command line option: PROGRAMMABLE_BTN_xxx (i.e. /tsip=PROGRAMMABLE_BTN_xxx) - simulating programmable button with ID = xxx (0...100+) being pressed
	- added Lua GetUserName() function, returning user name from configuration - intended to be used to differentiate instances

Version 0.1.62.2
	- FIXED: due to hazard between callback and control queues ring audio might be started after call ended (for calls with CANCEL immediately after INVITE) and not stopped; forcing ring stop on end of call event

Version 0.1.62.3
	- new settings: "Show main form when answering call" and "Hide tray notifier when answering call"
	- added "General (2)" settings tab

Version 0.1.63
	- Lua: added ProgrammableButtonClick(buttonId) function - simulating clicking on specified button
	- numeric code is added to text (quite often meaningless "Unknown error") generated by strerror
	- registration client: reduced "failwait" saturation time, i.e. maximum time to retry registration from 1800s to 300s (time is randomized and increases exponentially up to this limit)
	- subscription client: removed RESUB_FAILC_MAX limit, added "failwait" similar to registration with 900s limit

Version 0.1.64
	- added "logrotate" mechanism; configuration: number of old files to keep (0...5)
	- added queues as new mechanism for connecting scripts and plugins (similar to variables)
	- added Lua FindWindowByCaptionAndExeName(caption, exeName) function
	- added sxmlc library source to re
	- BLF buttons can now show remote identity and call direction (number calling / number called) as 2nd line (opt-out)
	- fixed few error log lines

Version 0.1.65
	- FIXED: nullaudio audio output module not working in release build due to unitialized variable
	- FIXED: project dependency on gsm.lib not showing in IDE
	- command line: target starting with "sip:" is treated as direct IP call
	- FIXED: (repeated) delayed auto-answer on answer error
	- added options to set custom application title and main form caption
	- added speex resampler to aufile (wave file) audio input module
	- added window ("Tools" menu) for script editing / ad hoc scripting based on Scintilla (this is making script plugin obsolete)
	- script editor can be used directly from settings window (script assignment) editing existing script or creating new one
	- rewritten reading main config and button config file - using previous values as defaults when reading JSON
	- added RefreshAudioDevicesList() and GetAudioDevice() Lua functions
	- added UpdateSettings(jsonText) Lua function

Version 0.1.66
	- separated few settings into "Locking" tab (kiosk mode and other settings disabling functionality)
	- added "Hide Tools menu" option
	- fixed wrong interpretation of number of console column settings (broken in 0.1.65)
	- corrected main window height to match exactly default console buttons height (broken in 0.1.65)
	- added Lua WinAPI GetAsyncKeyState() function - allowing to e.g. modify script button action depending on Ctrl/Shift/Alt state
	- added options to record only one side of the call (local or remote)
	- extended Lua RecordStart(file, channels, side) function; third (optional) argument specifies which side(s) of the call should be recorded (0 = both/mixed, 1 = local, 2 = remote); third parameter applies only to single channel recording (channels = 1), otherwise call parties are recorded in separate left/right channels as previously
	- added command line option to specify settings/profile directory; allows sharing single exe by multiple instances or multiple users; example: tSIP.exe /profiledir="E:\zrzut\aa bb"
	- added Lua funcion: GetProfileDir()
	- added "Hide tray notifier when call is auto-answered" setting
	- FIXED: UTF8 decoding not applied to BLF remote identity display name
	- added P-Asserted-Identity handling
	- added REINVITE event handling (updating CLIP from PAI)
	- added BLF settings: "Keep previous remote identity info if remote identity is missing in notification" and "Ignore or clear remote identity if call state is set to terminated"
	- added Lua PlaySound() function (WinAPI equivalent in tsip_winapi module)
	- script window accepts file drag-and-drop
	- added "on programmable button" script event and "SetHandled" Lua function
	- added Lua GetButtonType(btnId) and GetButtonNumber(btnId) functions

Version 0.1.67
	- FIXED: missing checking if "on programmable button" event script was set (problem of 0.1.66)
	- added separate width setting for each speed dial column
	- added missing onClick handler to label2 of programmable button
	- using PAI from 200/OK reply (updating calee number/name after pickup)
	- added Help/Troubleshooting function
	- added main window height setting
	- added option allowing to hide speed dial toggle button
	- added option modifying behavior on audio error (not disconnecting call) - Call settings tab
	- added "on audio device error" script event	
	- added audio device error event to plugin interface
	- extended plugin interface with capability of running scripts
	- extended plugin interface: SetProfileDir(dir) function
	- change wav audio source: releasing input file immediately after reading
	- FIXED: error handler possibly not being called for resampled aufile
	- prevent divide by zero exception if switching to nullaudio before CONFIRMED state
	- new plugin: Text-To-Speech input
*/


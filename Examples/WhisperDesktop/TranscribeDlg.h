#pragma once
#include "AppState.h"
#include "Utils/WTL/atlddx.h"
#include "Utils/WTL/atlcrack.h"
#include "Utils/miscUtils.h"
#include "Utils/LanguageDropdown.h"
#include "Utils/TranslateCheckbox.h"
#include "Utils/PendingState.h"
#include "Utils.h"

class TranscribeDlg :
	public CDialogImpl<TranscribeDlg>,
	public CWinDataExchange<TranscribeDlg>,
	public iThreadPoolCallback
{
	AppState& appState;

public:
	static constexpr UINT IDD = IDD_TRANSCRIBE_DIALOG;
	static constexpr UINT WM_CALLBACK_STATUS = WM_APP + 1;

	TranscribeDlg( AppState& app ) : appState( app ) { }

	// Show this dialog modally, without parent.
	HRESULT show();

	BEGIN_MSG_MAP(LoadModelDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		ON_BUTTON_CLICK(IDC_CONSOLE, cbConsole.click)
		ON_BUTTON_CLICK(IDCANCEL, onClose)
		ON_BUTTON_CLICK(IDC_BACK, onBack)
		ON_BUTTON_CLICK(IDC_BROWSE_MEDIA, onBrowseMedia)
		ON_BUTTON_CLICK(IDC_BROWSE_RESULT, onBrowseOutput)
		ON_BUTTON_CLICK(IDC_TRANSCRIBE, onTranscribe)
		ON_BUTTON_CLICK(IDC_CAPTURE, onCapture)
		COMMAND_HANDLER(IDC_OUTPUT_FORMAT, CBN_SELCHANGE, onOutFormatChange)
		COMMAND_HANDLER(IDC_PATH_MEDIA, EN_CHANGE, onInputChange)
		MESSAGE_HANDLER(WM_CALLBACK_STATUS, onCallbackStatus)
		MSG_WM_CLOSE(onWmClose)
		ON_BUTTON_CLICK(IDC_BUTTON1, onSaveFile)
		ON_BUTTON_CLICK(IDC_BUTTON2, onCopy)
		ON_BUTTON_CLICK(IDC_BUTTON3, onClear);
	END_MSG_MAP()

	BEGIN_DDX_MAP(LoadModelDlg)
		DDX_CONTROL_HANDLE(IDC_MODEL_DESC, modelDesc)
		DDX_CONTROL_HANDLE(IDC_PATH_MEDIA, sourceMediaPath)
		DDX_CONTROL_HANDLE(IDC_OUTPUT_FORMAT, transcribeOutFormat)
		DDX_CONTROL_HANDLE(IDC_PATH_RESULT, transcribeOutputPath)
		DDX_CONTROL_HANDLE(IDC_BROWSE_RESULT, transcribeOutputBrowse)
		DDX_CONTROL_HANDLE(IDC_TRANSCRIBE_PROGRESS, progressBar)
		DDX_CONTROL_HANDLE(IDC_OUTPUTBOX, outputBox);
	END_DDX_MAP()

private:
	PendingState pendingState;
	void setPending( bool nowPending );
	void transcribeError( LPCTSTR text, HRESULT hr = S_FALSE );

	LRESULT OnInitDialog( UINT nMessage, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	void onClose()
	{
		ATLVERIFY( EndDialog( IDCANCEL ) );
	}
	void onBack()
	{
		ATLVERIFY( EndDialog( IDC_BACK ) );
	}

	void printModelDescription();
	CStatic modelDesc;
	ConsoleCheckbox cbConsole;

	LanguageDropdown languageSelector;
	TranslateCheckbox cbTranslate;

	CEdit sourceMediaPath;
	CEdit transcribeOutputPath;
	CButton transcribeOutputBrowse;
	CComboBox transcribeOutFormat;
	CProgressBarCtrl progressBar;
	CEdit outputBox;
	void populateOutputFormats();

	LRESULT onOutFormatChange( UINT, INT, HWND, BOOL& bHandled );
	LRESULT onInputChange( UINT, INT, HWND, BOOL& );
	void onBrowseMedia();
	void onBrowseOutput();
	void onTranscribe();
	void onCapture()
	{
		EndDialog( IDC_CAPTURE );
	}

	ThreadPoolWork work;

	enum struct eOutputFormat : uint8_t;

	struct TranscribeArgs
	{
		CString pathMedia;
		CString pathOutput;
		uint32_t language;
		bool translate;
		eOutputFormat format;
		Whisper::eResultFlags resultFlags;
		uint64_t startTime;
		int64_t mediaDuration;
		CString errorMessage;
	};
	TranscribeArgs transcribeArgs;

	void __stdcall poolCallback() noexcept override final;

	LRESULT onCallbackStatus( UINT, WPARAM wParam, LPARAM, BOOL& bHandled );

	HRESULT transcribe();
	void getThreadError();
	
	static HRESULT writeTextFile( const Whisper::sSegment* const segments, const size_t length, CStringA& file, bool timestamps );
	static HRESULT writeSubRip( const Whisper::sSegment* const segments, const size_t length, CStringA& file );
	static HRESULT writeWebVTT( const Whisper::sSegment* const segments, const size_t length, CStringA& file );

	static HRESULT __cdecl newSegmentCallbackStatic( Whisper::iContext* ctx, uint32_t n_new, void* user_data ) noexcept;
	HRESULT newSegmentCallback( Whisper::iContext* ctx, uint32_t n_new );

	static HRESULT __cdecl progressCallbackStatic( double p, Whisper::iContext* ctx, void* pv ) noexcept;
	HRESULT progressCallback( double p ) noexcept;

	void onWmClose();
	// Populate output path based on the provided input media path
	void setOutputPath( const CString& input );
	// Populate output path based on the input media path in the edit box
	void setOutputPath();

	// add on
	LRESULT setOutPut();
	CString outdata;
	void onSaveFile();
	void onCopy();
	void onClear();
};
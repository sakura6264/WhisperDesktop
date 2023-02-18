#include "stdafx.h"
#include "AppState.h"
#include "Utils/miscUtils.h"
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include "CircleIndicator.h"

namespace
{
	static const HKEY regKeyRoot = HKEY_CURRENT_USER;
	const LPCTSTR regKey = LR"(SOFTWARE\const.me\WhisperDesktop)";
	const LPCTSTR regValPath = L"modelPath";
	const LPCTSTR regValImpl = L"modelImpl";
	const LPCTSTR regValLang = L"language";
	const LPCTSTR regValLastScreen = L"screen";
	const LPCTSTR regValGpuFlags = L"gpuFlags";

	static HRESULT readString( CRegKey& k, LPCTSTR name, CString& rdi )
	{
		ULONG nChars = 0;
		LSTATUS lss = k.QueryStringValue( name, nullptr, &nChars );
		if( lss != ERROR_SUCCESS )
			return HRESULT_FROM_WIN32( lss );
		if( nChars == 0 )
		{
			rdi = L"";
			return S_FALSE;
		}

		lss = k.QueryStringValue( name, rdi.GetBufferSetLength( nChars ), &nChars );
		rdi.ReleaseBuffer();
		if( lss != ERROR_SUCCESS )
			return HRESULT_FROM_WIN32( lss );

		return S_OK;
	}

	using Whisper::eModelImplementation;
}

HRESULT AppState::startup()
{
	HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
	if( FAILED( hr ) )
	{
		reportFatalError( "CoInitializeEx failed", hr );
		return hr;
	}
	coInit = true;
	INITCOMMONCONTROLSEX init;
	init.dwSize = sizeof( init );
	init.dwICC = ICC_LINK_CLASS | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES | ICC_TAB_CLASSES;
	const BOOL icc = InitCommonControlsEx( &init );
	if( !icc )
	{
		reportFatalError( "InitCommonControlsEx failed", HRESULT_FROM_WIN32( GetLastError() ) );
		return E_FAIL;
	}

	hr = initMediaFoundation( &mediaFoundation );
	if( FAILED( hr ) )
	{
		reportFatalError( "Unable to initialize Media Foundation runtime", hr );
		return hr;
	}

	hr = console.initialize();
	if( FAILED( hr ) )
	{
		reportFatalError( "Unable to initialize logging", hr );
		return hr;
	}

	hr = CircleIndicator::registerClass();
	if( FAILED( hr ) )
	{
		reportFatalError( "Unable to register custom controls", hr );
		return hr;
	}
	appIcon.LoadIcon( IDI_WHISPERDESKTOP );
	return S_OK;
}

AppState::~AppState()
{
	if( coInit )
	{
		CoUninitialize();
		coInit = false;
	}
}

HRESULT AppState::findModelSource()
{
	{
		CAtlFile file;
		CHECK( file.Create( source.path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING ) );
		ULONGLONG len;
		CHECK( file.GetSize( len ) );
		source.sizeInBytes = len;
	}

	CString impl;
	CHECK( implParse( impl, source.impl ) );
	source.found = true;
	return S_OK;
}

HRESULT AppState::saveModelSource()
{
	return S_OK;
}

uint32_t AppState::languageRead()
{
	return UINT_MAX;
}

void AppState::languageWrite( uint32_t key ){}

CString AppState::stringLoad( LPCTSTR name )
{
	return CString();
}
void AppState::stringStore( LPCTSTR name, LPCTSTR value ){}
uint32_t AppState::dwordLoad( LPCTSTR name, uint32_t fallback )
{
	return fallback;
}
void AppState::dwordStore( LPCTSTR name, uint32_t value ){}

void AppState::lastScreenSave( HRESULT code )
{
	dwordStore( regValLastScreen, (uint32_t)code );
}

HRESULT AppState::lastScreenLoad()
{
	return (HRESULT)dwordLoad( regValLastScreen, SCREEN_TRANSCRIBE );
}

void AppState::setupIcon( CWindow* wnd )
{
	HICON ic = appIcon;
	if( nullptr != ic )
	{
		wnd->SendMessage( WM_SETICON, ICON_SMALL, (LPARAM)ic );
		wnd->SendMessage( WM_SETICON, ICON_BIG, (LPARAM)ic );
	}
}

uint32_t AppState::gpuFlagsLoad()
{
	return dwordLoad( regValGpuFlags, 0 );
}

void AppState::gpuFlagsStore( uint32_t flags ){}

bool AppState::boolLoad( LPCTSTR name )
{
	return dwordLoad( name, 0 ) != 0;
}

void AppState::boolStore( LPCTSTR name, bool val ){}
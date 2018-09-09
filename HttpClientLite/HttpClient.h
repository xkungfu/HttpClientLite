#pragma once

// STL Header
#include <map>
#include <string>
#include <optional>

// Boost Header
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

/**
 * Class to save all information about a HTML set
 */
class HTMLTag
{
public:
	std::string											m_sTagName;
	std::optional<std::string>							m_sContent;
	std::map< std::string, std::optional<std::string> >	m_mAttributes;

public:
	HTMLTag( const std::string& sTagName )
	{
		m_sTagName = sTagName;
	}

	HTMLTag( const std::string& sTagName, const std::string& sHtmlSource, size_t uBeginPos = 0 )
	{
		m_sTagName = sTagName;
		GetData( sHtmlSource, uBeginPos );
	}

	std::optional< std::pair<size_t,size_t> > GetData( const std::string& sHtmlSource, size_t uBeginPos = 0 );

	std::optional< std::pair<size_t,size_t> > FindQuoteContent( const std::string& rSourze, const char c, size_t uBeginPos = 0 );

public:
	static std::optional< std::pair< HTMLTag, std::pair<size_t,size_t> > > Construct( const std::string& sTagName, std::string& sHtmlSource, size_t uBeginPos = 0 )
	{
		HTMLTag mTag( sTagName );
		std::optional< std::pair<size_t,size_t> > pInfo = mTag.GetData( sHtmlSource, uBeginPos );
		if( pInfo )
			return std::make_pair( mTag, *pInfo );

		return std::optional< std::pair< HTMLTag, std::pair<size_t,size_t> > >();
	}
};

class HTMLParser
{
public:
	typedef std::map< std::wstring, std::optional<std::wstring> >	TAttributes;

public:
	static void FindTag( const std::wstring& rHtml, const std::wstring& rTag, const std::map< std::wstring, std::optional<std::wstring> >& rAttribute, size_t uStartPos = 0 );

	static std::pair<size_t,std::wstring> FindContentBetweenTag( const std::wstring& rHtml, const std::pair<std::wstring,std::wstring>& rTag, size_t uStartPos = 0 );

	static std::optional< std::pair<std::wstring,std::wstring> > AnalyzeLink( const std::wstring& rHtml, size_t uStartPos = 0 );
};

class HttpClient
{
public:
	boost::signals2::signal<void(const std::string&)>	m_sigErrorLog;
	boost::signals2::signal<void(const std::string&)>	m_sigInfoLog;

public:
	HttpClient();

	std::optional<std::wstring> ReadHtml( const std::string& rServer, const std::string& rPath );

	std::optional<std::wstring> ReadHtml( const std::string& sURL )
	{
		auto pLink = ParseURL( sURL );
		if( pLink )
			return ReadHtml( pLink->first, pLink->second );

		return std::optional<std::wstring>();
	}

	bool GetBinaryFile( const std::string& rServer, const std::string& rPath, const std::wstring& rFilename );

	bool GetBinaryFile( const std::string& sURL, const std::wstring& rFilename )
	{
		auto pLink = ParseURL( sURL );
		if( pLink )
			return GetBinaryFile( pLink->first, pLink->second, rFilename );

		return false;
	}

public:
	static std::optional< std::pair<std::string,std::string> > ParseURL( const std::string& sURL );
	static std::optional< std::string > GetFilename( const std::string& sURL );

protected:
	boost::asio::io_service			m_IO_service;
	boost::asio::ip::tcp::resolver	m_Resolver;

protected:
	bool SendRequest( const std::string& rServer, const std::string& rPath, boost::asio::ip::tcp::iostream& rStream );
	size_t GetHttpHeader( boost::asio::ip::tcp::iostream& rStream );
};
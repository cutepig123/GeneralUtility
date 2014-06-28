#include <oleauto.h>

#include <map>
#include <string>

#include "dllpstub.h"

// C++ interface to GNU libiconv functions
struct ICONV
{
//	ICONV dll interface
	struct Proxy;

	HMODULE BEGIN;

	HANDLE (*iconv_open)(const char *tocode, const char *fromcode);
	size_t (*iconv)(HANDLE, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
	int (*iconv_close)(HANDLE);
	int (*iconvctl)(HANDLE, int request, void *argument);
	void (*iconvlist)
	(
		int (*do_one)(unsigned int namescount, const char *const *names, void *data),
		void *data
	);
	int *_libiconv_version;

	HMODULE END;
};

// Proxy which loads libiconv DLL (iconv.dll)
extern struct ICONV::Proxy
{
//	ICONV dll proxy
	DLLPSTUB stub; // DLLPSTUB must be followed by char array naming the DLL
	LPCSTR ICONV[&((ICONV*)0)->END - &((ICONV*)0)->BEGIN];
	HMODULE handle;
	struct ICONV *operator->()
	{
		stub.Load();
		return (struct ICONV *) ICONV;
	}
} ICONV;

//	Provide CMap template with reasonable HashKey() overload for BSTR
template<> UINT AFXAPI HashKey(BSTR);

class CMarkdown
{
//	Core class
public:
	class Converter
	{
	//	ICONV wrapper
	public:
		HANDLE handle;
		Converter(const char *tocode, const char *fromcode);
		~Converter();
		size_t iconv(const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) const;
		size_t Convert(const char *, size_t, char *, size_t) const;
	};
	typedef std::map<std::wstring, std::wstring> EntityMap;
	static void Load(EntityMap &entityMap);
	void Load(EntityMap &entityMap, const Converter &);
	class FileImage;
	class File;
	// An _HSTR is a handle to a string
	// It may be any one (and only one) of the following
	//  - CHAR (use _HSTR::A)
	//  - WCHAR (use _HSTR::W)
	//  - TCHAR (use __HSTR::T)
	//  - OLECHAR (use __HSTR::B)
	// NB: Convert, Resolve, Entites, and Trim all require the _HSTR be an OLECHAR
	//      as they use the member B
	typedef union _HSTR
	{
		OLECHAR B[20];
		CHAR A[20];
		TCHAR T[20];
		WCHAR W[20];
		// Unicode(codepage) converts string using MultiByteToWideChar()
		_HSTR *Unicode(UINT); // passing 1200 makes this a NOP
		// Octets(codepage) converts string using WideCharToMultiByte()
		_HSTR *Octets(UINT); // passing 1200 makes this a NOP
		// Convert(converter) converts string using an ICONV descriptor
		_HSTR *Convert(const Converter &);
		_HSTR *Resolve(const EntityMap &);
		_HSTR *Entities();
		_HSTR *Trim(const OLECHAR *);
	} *HSTR;
	union String
	{
	//	No orthogonal string class interface, just the methods required.
	//	Based on BSTR as it supports both octet strings and wide strings.
	//	The parser will always return octet strings, which can be converted to
	//	wide strings using Unicode() or Convert(), which can be converted back
	//	to octet strings using Octets() or Convert(). Convert() is the most
	//	versatile of these methods, but requires ICONV.DLL to be in place.
	//	Use Unicode() and Octets() if you must get away without ICONV.DLL.
	private:
		String(const String &);
		String &operator=(const String &);
	public:
		HSTR H;
		BSTR B;
		LPSTR A;
		LPTSTR T;
		LPWSTR W;
		String(HSTR H = 0):H(H)
		{
		}
		void operator=(HSTR H)
		{
			SysFreeString(B);
			B = H->B;
		}
		bool operator==(const char *A)
		{
			return lstrcmpA(String::A, A) == 0;
		}
		~String()
		{
			SysFreeString(B);
		}
		class AppendStream
		{
		//	Concatenation helper
			OLECHAR B[1];
		public:
			AppendStream *Append(const OLECHAR *B, UINT b)
			{
				memcpy(AppendStream::B, B, b * sizeof(OLECHAR));
				return (class AppendStream *)(AppendStream::B + b);
			}
		} *AppendStream(UINT cchGrow)
		{
			UINT b = SysStringLen(B);
			if (!SysReAllocStringLen(&B, B, b + cchGrow))
			{
				AfxThrowMemoryException();
			}
			return (class AppendStream *)(B + b);
		}
	};
	const char *first;	// first char of current markup (valid after Move)
	const char *lower;	// beginning of enclosed text (valid after Move)
	const char *upper;	// end of enclosed text (initially beginning of file)
	const char *ahead;	// last char of file
	enum
	{
		IgnoreCase = 0x10,
		HtmlUTags = 0x20,			// check for unbalanced tags
		Html = IgnoreCase|HtmlUTags	// shortcut
	};
	CMarkdown(const char *upper, const char *ahead, unsigned flags = 0);
	operator bool();				// is node ahead?
	void Scan();					// find closing tag
	CMarkdown &Move();				// move to next node
	CMarkdown &Move(const char *);	// move to next node with given name
	bool Pull();					// pull child nodes into view
	CMarkdown &Pop();				// irreversible pull for chained calls
	bool Push();					// reverse pull
	HSTR GetTagName();				// tag name
	HSTR GetTagText();				// tag name plus attributes
	HSTR GetInnerText();			// text between enclosing tags
	HSTR GetOuterText();			// text including enclosing tags
	HSTR GetAttribute(const char *, const void * = 0); // random or enumerate
private:
	int (__cdecl *const memcmp)(const void *, const void *, size_t);
	const char *const utags;
	int FindTag(const char *, const char *);
	class Token;
};

class CMarkdown::FileImage
{
//	Map a file into process memory. Optionally convert UCS2 source to UTF8.
public:
	DWORD cbImage;
	LPVOID pImage;
	enum
	{
		Handle = 0x01,
		Octets = 0x02 + 0x04,
		Mapping = 0x40
	};
	int nByteOrder;
	FileImage(LPCTSTR, DWORD trunc = 0, int flags = 0);
	~FileImage();
	static LPVOID NTAPI MapFile(HANDLE hFile, DWORD dwSize);
	static int NTAPI GuessByteOrder(DWORD);
};

class CMarkdown::File : public CMarkdown::FileImage, public CMarkdown
{
//	Construct CMarkdown object from file.
public:
	File(LPCTSTR path, DWORD trunc = 0, unsigned flags = Octets):
	CMarkdown::FileImage(path, trunc, flags),
	CMarkdown((const char *)pImage, (const char *)pImage + cbImage, flags)
	{
	}
};

///////////////////////////////////////////////////////////////////////////
//  File:    asp.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  ASP syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ccrystaltextview.h"
#include "SyntaxColors.h"
#include "string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszHtmlKeywordList[] =
  {
    // HTML section
    _T ("DOCTYPE"),
    _T ("PUBLIC"),
    _T ("HTML"),
    // frames
    _T ("FRAMESET"),
    _T ("FRAME"),
    _T ("NOFRAMES"),
    _T ("IFRAME"),
    // HEAD section
    _T ("HEAD"),
    _T ("TITLE"),
    _T ("ISINDEX"),
    _T ("META"),
    _T ("LINK"),
    _T ("BASE"),
    _T ("BASEFONT"),
    _T ("SCRIPT"),
    _T ("NOSCRIPT"),
    _T ("STYLE"),
    // BODY section
    _T ("BODY"),
    // headings
    _T ("H1"),
    _T ("H2"),
    _T ("H3"),
    _T ("H4"),
    _T ("H5"),
    _T ("H6"),
    // lists
    _T ("UL"),
    _T ("OL"),
    _T ("DIR"),
    _T ("MENU"),
    _T ("LI"),
    _T ("DL"),
    _T ("DT"),
    _T ("DD"),
    // text containers
    _T ("P"),
    _T ("PRE"),
    _T ("BLOCKQUOTE"),
    _T ("ADDRESS"),
    // others
    _T ("DIV"),
    _T ("SPAN"),
    _T ("CENTER"),
    _T ("HR"),
    // logical markup
    _T ("EM"),
    _T ("STRONG"),
    _T ("DFN"),
    _T ("CODE"),
    _T ("SAMP"),
    _T ("KBD"),
    _T ("VAR"),
    _T ("CITE"),
    _T ("Q"),
    _T ("ACRONYM"),
    _T ("ABBR"),
    _T ("INS"),
    _T ("DEL"),
    _T ("BDO"),
    // physical markup
    _T ("TT"),
    _T ("I"),
    _T ("B"),
    _T ("U"),
    _T ("STRIKE"),
    _T ("S"),
    _T ("BIG"),
    _T ("SMALL"),
    _T ("SUB"),
    _T ("SUP"),
    // special markup
    _T ("A"),
    _T ("IMG"),
    _T ("APPLET"),
    _T ("OBJECT"),
    _T ("PARAM"),
    _T ("FONT"),
    _T ("BR"),
    _T ("MAP"),
    _T ("AREA"),
    // forms
    _T ("FORM"),
    _T ("INPUT"),
    _T ("TEXTAREA"),
    _T ("SELECT"),
    _T ("OPTION"),
    _T ("OPTGROUP"),
    _T ("BUTTON"),
    _T ("FIELDSET"),
    _T ("LEGEND"),
    _T ("LABEL"),
    // tables
    _T ("TABLE"),
    _T ("TR"),
    _T ("TH"),
    _T ("TD"),
    _T ("COLGROUP"),
    _T ("COL"),
    _T ("THEAD"),
    _T ("TBODY"),
    _T ("TFOOT"),
    _T ("CAPTION"),
    NULL
  };

static LPTSTR s_apszUser1KeywordList[] =
  {
    _T ("ABBR"),
    _T ("ACCEPT"),
    _T ("ACCEPT-CHARSET"),
    _T ("ACCESSKEY"),
    _T ("ACTION"),
    _T ("ALIGN"),
    _T ("ALINK"),
    _T ("ALT"),
    _T ("ARCHIVE"),
    _T ("AXIS"),
    _T ("BACKGROUND"),
    _T ("BGCOLOR"),
    _T ("BORDER"),
    _T ("CELLPADDING"),
    _T ("CELLSPACING"),
    _T ("CHAR"),
    _T ("CHAROFF"),
    _T ("CHARSET"),
    _T ("CHECKED"),
    _T ("CITE"),
    _T ("CLASS"),
    _T ("CLASSID"),
    _T ("CLEAR"),
    _T ("CODE"),
    _T ("CODEBASE"),
    _T ("CODETYPE"),
    _T ("COLOR"),
    _T ("COLS"),
    _T ("COLSPAN"),
    _T ("COMPACT"),
    _T ("CONTENT"),
    _T ("COORDS"),
    _T ("DATA"),
    _T ("DATAFLD"),
    _T ("DATAFORMATAS"),
    _T ("DATASRC"),
    _T ("DATETIME"),
    _T ("DECLARE"),
    _T ("DEFER"),
    _T ("DIR"),
    _T ("DISABLED"),
    _T ("ENCTYPE"),
    _T ("EVENT"),
    _T ("FACE"),
    _T ("FOR"),
    _T ("FRAME"),
    _T ("FRAMEBORDER"),
    _T ("HEADERS"),
    _T ("HEIGHT"),
    _T ("HREF"),
    _T ("HREFLANG"),
    _T ("HSPACE"),
    _T ("HTTP-EQUIV"),
    _T ("ID"),
    _T ("ISMAP"),
    _T ("LABEL"),
    _T ("LANG"),
    _T ("LANGUAGE"),
    _T ("LINK"),
    _T ("LONGDESC"),
    _T ("MARGINHEIGHT"),
    _T ("MARGINWIDTH"),
    _T ("MAXLENGTH"),
    _T ("MEDIA"),
    _T ("METHOD"),
    _T ("MULTIPLE"),
    _T ("NAME"),
    _T ("NOHREF"),
    _T ("NORESIZE"),
    _T ("NOSHADE"),
    _T ("NOWRAP"),
    _T ("ONBLUR"),
    _T ("ONCHANGE"),
    _T ("ONCLICK"),
    _T ("ONDBLCLICK"),
    _T ("ONFOCUS"),
    _T ("ONKEYDOWN"),
    _T ("ONKEYPRESS"),
    _T ("ONKEYUP"),
    _T ("ONLOAD"),
    _T ("ONMOUSEDOWN"),
    _T ("ONMOUSEMOVE"),
    _T ("ONMOUSEOUT"),
    _T ("ONMOUSEOVER"),
    _T ("ONMOUSEUP"),
    _T ("ONRESET"),
    _T ("ONSELECT"),
    _T ("ONSUBMIT"),
    _T ("ONUNLOAD"),
    _T ("PROFILE"),
    _T ("PROMPT"),
    _T ("READONLY"),
    _T ("REL"),
    _T ("REV"),
    _T ("ROWS"),
    _T ("ROWSPAN"),
    _T ("RULES"),
    _T ("SCHEME"),
    _T ("SCOPE"),
    _T ("SCROLLING"),
    _T ("SELECTED"),
    _T ("SHAPE"),
    _T ("SIZE"),
    _T ("SPAN"),
    _T ("SRC"),
    _T ("STANDBY"),
    _T ("START"),
    _T ("STYLE"),
    _T ("SUMMARY"),
    _T ("TABINDEX"),
    _T ("TARGET"),
    _T ("TEXT"),
    _T ("TITLE"),
    _T ("TYPE"),
    _T ("USEMAP"),
    _T ("VALIGN"),
    _T ("VALUE"),
    _T ("VALUETYPE"),
    _T ("VERSION"),
    _T ("VLINK"),
    _T ("VSPACE"),
    _T ("WIDTH"),
    NULL
  };

static LPTSTR s_apszUser2KeywordList[] =
  {
    _T ("nbsp"),
    _T ("quot"),
    _T ("amp"),
    _T ("lt"),
    _T ("gt"),
    _T ("copy"),
    _T ("reg"),
    _T ("acute"),
    _T ("laquo"),
    _T ("raquo"),
    _T ("iexcl"),
    _T ("iquest"),
    _T ("Agrave"),
    _T ("agrave"),
    _T ("Aacute"),
    _T ("aacute"),
    _T ("Acirc"),
    _T ("acirc"),
    _T ("Atilde"),
    _T ("atilde"),
    _T ("Auml"),
    _T ("auml"),
    _T ("Aring"),
    _T ("aring"),
    _T ("AElig"),
    _T ("aelig"),
    _T ("Ccedil"),
    _T ("ccedil"),
    _T ("ETH"),
    _T ("eth"),
    _T ("Egrave"),
    _T ("egrave"),
    _T ("Eacute"),
    _T ("eacute"),
    _T ("Ecirc"),
    _T ("ecirc"),
    _T ("Euml"),
    _T ("euml"),
    _T ("Igrave"),
    _T ("igrave"),
    _T ("Iacute"),
    _T ("iacute"),
    _T ("Icirc"),
    _T ("icirc"),
    _T ("Iuml"),
    _T ("iuml"),
    _T ("Ntilde"),
    _T ("ntilde"),
    _T ("Ograve"),
    _T ("ograve"),
    _T ("Oacute"),
    _T ("oacute"),
    _T ("Ocirc"),
    _T ("ocirc"),
    _T ("Otilde"),
    _T ("otilde"),
    _T ("Ouml"),
    _T ("ouml"),
    _T ("Oslash"),
    _T ("oslash"),
    _T ("Ugrave"),
    _T ("ugrave"),
    _T ("Uacute"),
    _T ("uacute"),
    _T ("Ucirc"),
    _T ("ucirc"),
    _T ("Uuml"),
    _T ("uuml"),
    _T ("Yacute"),
    _T ("yacute"),
    _T ("yuml"),
    _T ("THORN"),
    _T ("thorn"),
    _T ("szlig"),
    _T ("sect"),
    _T ("para"),
    _T ("micro"),
    _T ("brvbar"),
    _T ("plusmn"),
    _T ("middot"),
    _T ("uml"),
    _T ("cedil"),
    _T ("ordf"),
    _T ("ordm"),
    _T ("not"),
    _T ("shy"),
    _T ("macr"),
    _T ("deg"),
    _T ("sup1"),
    _T ("sup2"),
    _T ("sup3"),
    _T ("frac14"),
    _T ("frac12"),
    _T ("frac34"),
    _T ("times"),
    _T ("divide"),
    _T ("cent"),
    _T ("pound"),
    _T ("curren"),
    _T ("yen"),
    NULL
  };

static LPTSTR s_apszAspKeywordList[] =
  {
    _T ("If"),
    _T ("Function"),
    _T ("Else"),
    _T ("End"),
    _T ("For"),
    _T ("Next"),
    _T ("While"),
    _T ("Wend"),
    _T ("SMDoMenu"),
    _T ("GetAttrType"),
    _T ("GetAttrName"),
    _T ("GetAttrValString"),
    _T ("GetAttrValFloat"),
    _T ("GetAttrValInt"),
    _T ("GetAttrValBool"),
    _T ("GetAttrValEnumInt"),
    _T ("GetAttrValEnumString"),
    _T ("GetClassId"),
    _T ("GetGeoType"),
    _T ("SetAttrValString"),
    _T ("SetAttrValInt"),
    _T ("SetAttrValFloat"),
    _T ("SetAttrValBool"),
    _T ("SetAttrValEnumString"),
    _T ("SetAttrValEnumInt"),
    _T ("CreateVerifyItem"),
    _T ("VerifyCardinalities"),
    _T ("As"),
    _T ("Abs"),
    _T ("AppActivate"),
    _T ("Asc"),
    _T ("Atn"),
    _T ("Beep"),
    _T ("Call"),
    _T ("CDbl"),
    _T ("ChDir"),
    _T ("ChDrive"),
    _T ("CheckBox"),
    _T ("Chr"),
    _T ("CInt"),
    _T ("CLng"),
    _T ("Close"),
    _T ("Const"),
    _T ("Cos"),
    _T ("CreateObject"),
    _T ("CSng"),
    _T ("CStr"),
    _T ("CVar"),
    _T ("CurDir"),
    _T ("Date"),
    _T ("Declare"),
    _T ("Dialog"),
    _T ("Dim"),
    _T ("Dir"),
    _T ("DlgEnable"),
    _T ("DlgText"),
    _T ("DlgVisible"),
    _T ("Do"),
    _T ("Double"),
    _T ("Loop"),
    _T ("End"),
    _T ("EOF"),
    _T ("Erase"),
    _T ("Exit"),
    _T ("Exp"),
    _T ("FileCopy"),
    _T ("FileLen"),
    _T ("Fix"),
    _T ("For"),
    _T ("To"),
    _T ("Step"),
    _T ("Next"),
    _T ("Format"),
    _T ("Function"),
    _T ("GetObject"),
    _T ("Global"),
    _T ("GoSub"),
    _T ("Return"),
    _T ("GoTo"),
    _T ("Hex"),
    _T ("Hour"),
    _T ("If"),
    _T ("Then"),
    _T ("Else"),
    _T ("Input"),
    _T ("InputBox"),
    _T ("InStr"),
    _T ("Int"),
    _T ("IsDate"),
    _T ("IsEmpty"),
    _T ("IsNull"),
    _T ("IsNumeric"),
    _T ("Kill"),
    _T ("LBound"),
    _T ("LCase"),
    _T ("LCase$"),
    _T ("Left"),
    _T ("Left$"),
    _T ("Len"),
    _T ("Let"),
    _T ("Line"),
    _T ("Input#"),
    _T ("Log"),
    _T ("Long"),
    _T ("Mid"),
    _T ("Minute"),
    _T ("MkDir"),
    _T ("Month"),
    _T ("MsgBox"),
    _T ("Name"),
    _T ("Now"),
    _T ("Oct"),
    _T ("On"),
    _T ("Error"),
    _T ("Open"),
    _T ("Option"),
    _T ("Base"),
    _T ("Print"),
    _T ("Print"),
    _T ("Rem"),
    _T ("Right"),
    _T ("RmDir"),
    _T ("Rnd"),
    _T ("Second"),
    _T ("Seek"),
    _T ("Seek"),
    _T ("Select Case"),
    _T ("SendKeys"),
    _T ("Set"),
    _T ("Shell"),
    _T ("Sin"),
    _T ("Space"),
    _T ("Sqr"),
    _T ("Static"),
    _T ("Stop"),
    _T ("Str"),
    _T ("StrComp"),
    _T ("String"),
    _T ("StringFunction"),
    _T ("Sub"),
    _T ("Tan"),
    _T ("Text"),
    _T ("TextBox"),
    _T ("Time"),
    _T ("TimeSerial"),
    _T ("TimeValue"),
    _T ("Trim"),
    _T ("LTrim"),
    _T ("RTrim"),
    _T ("Type"),
    _T ("UBound"),
    _T ("UCase"),
    _T ("Val"),
    _T ("VarType"),
    _T ("While"),
    _T ("Wend"),
    _T ("With"),
    _T ("Write"),
    _T ("Year"),
    NULL
  };

static BOOL
IsXKeyword (LPTSTR apszKeywords[], LPCTSTR pszChars, int nLength)
{
  for (int L = 0; apszKeywords[L] != NULL; L++)
    {
      if (_tcsnicmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsHtmlKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszHtmlKeywordList, pszChars, nLength);
}

static BOOL
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszUser1KeywordList, pszChars, nLength);
}

static BOOL
IsUser2Keyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszUser2KeywordList, pszChars, nLength);
}

static BOOL
IsAspKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszAspKeywordList, pszChars, nLength);
}

static BOOL
IsAspNumber (LPCTSTR pszChars, int nLength)
{
  if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
    {
      for (int I = 2; I < nLength; I++)
        {
          if (_istdigit (pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
                (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
            continue;
          return FALSE;
        }
      return TRUE;
    }
  if (!_istdigit (pszChars[0]))
    return FALSE;
  for (int I = 1; I < nLength; I++)
    {
      if (!_istdigit (pszChars[I]) && pszChars[I] != '+' &&
            pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
            pszChars[I] != 'E')
        return FALSE;
    }
  return TRUE;
}

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010
#define COOKIE_USER1            0x0020
#define COOKIE_EXT_USER1        0x0040

DWORD CCrystalTextView::
ParseLineAsp (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT|COOKIE_EXT_USER1);

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~(COOKIE_EXT_COMMENT|COOKIE_EXT_USER1)) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bDecIndex = FALSE;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = ::CharNext(pszChars+I) - pszChars)
    {
      if (I == nPrevI)
        {
          // CharNext did not advance, so we're at the end of the string
          // and we already handled this character, so stop
          break;
        }

      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos = nPrevI;
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
            }
          else if (dwCookie & COOKIE_EXT_USER1)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.')
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                }
              else
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_OPERATOR);
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  goto out;
                }
            }
          bRedefineBlock = FALSE;
          bDecIndex = FALSE;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength)
        break;

      if (dwCookie & COOKIE_COMMENT)
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Extended comment <!--....-->
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if (!(dwCookie & COOKIE_EXT_USER1))
            {
              if (I > 1 && pszChars[I] == '>' && pszChars[nPrevI] == '-' && *::CharPrev(pszChars, pszChars + nPrevI) == '-')
                {
                  dwCookie &= ~COOKIE_EXT_COMMENT;
                  bRedefineBlock = TRUE;
                }
            }
          continue;
        }

      if ((dwCookie & COOKIE_EXT_USER1) && pszChars[I] == '\'')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Extended comment <?....?>
      if (dwCookie & COOKIE_EXT_USER1)
        {
          if (I > 0 && pszChars[I] == '>' && (pszChars[nPrevI] == '?' || pszChars[nPrevI] == '%'))
            {
              dwCookie &= ~COOKIE_EXT_USER1;
              bRedefineBlock = TRUE;
              continue;
            }
        }

      //  Normal text
      if ((dwCookie & (COOKIE_PREPROCESSOR|COOKIE_EXT_USER1)) && pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }

      if ((dwCookie & (COOKIE_PREPROCESSOR|COOKIE_EXT_USER1)) && pszChars[I] == '\'')
        {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if (!(dwCookie & COOKIE_EXT_USER1))
        {
          if (!(dwCookie & COOKIE_EXT_USER1) && I < nLength - 3 && pszChars[I] == '<' && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-')
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              I += 3;
              dwCookie |= COOKIE_EXT_COMMENT;
              dwCookie &= ~COOKIE_PREPROCESSOR;
              continue;
            }
        }

      if (bFirstChar)
        {
          if (!xisspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      //  User1 start: <?
      if (I < nLength && pszChars[I] == '<' && I < nLength - 1 && (pszChars[I + 1] == '?' || pszChars[I + 1] == '%'))
        {
          DEFINE_BLOCK (I, COLORINDEX_NORMALTEXT);
          dwCookie |= COOKIE_EXT_USER1;
          nIdentBegin = -1;
          continue;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (dwCookie & COOKIE_PREPROCESSOR)
                {
                  if (IsHtmlKeyword (pszChars + nIdentBegin, I - nIdentBegin) && (pszChars[nIdentBegin - 1] == _T ('<') || pszChars[nIdentBegin - 1] == _T ('/')))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                    }
                  else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                    }
                  else if (IsAspNumber (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                    }
                  else
                    {
                      goto next;
                    }
                }
              else if (dwCookie & COOKIE_EXT_USER1)
                {
                  if (IsAspKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                    }
                  else if (IsAspNumber (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                    }
                  else
                    {
                      bool bFunction = FALSE;

                      for (int j = I; j < nLength; j++)
                        {
                          if (!xisspace (pszChars[j]))
                            {
                              if (pszChars[j] == '(')
                                {
                                  bFunction = TRUE;
                                }
                              break;
                            }
                        }
                      if (bFunction)
                        {
                          DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                        }
                      else
                        {
                          goto next;
                        }
                    }
                }
              else if (dwCookie & COOKIE_USER1)
                {
                  if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
                    }
                  else
                    {
                      goto next;
                    }
                }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
next:
              ;
            }

          //  Preprocessor start: < or bracket
          if (!(dwCookie & COOKIE_EXT_USER1) && I < nLength && pszChars[I] == '<' && !(I < nLength - 3 && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-'))
            {
              DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
              DEFINE_BLOCK (I + 1, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_PREPROCESSOR;
              nIdentBegin = -1;
              continue;
            }

          //  User1 end: ?>
          if (dwCookie & COOKIE_EXT_USER1)
            {
              if (I > 0 && pszChars[I] == '>' && (pszChars[nPrevI] == '?' || pszChars[nPrevI] == '%'))
                {
                  dwCookie &= ~COOKIE_EXT_USER1;
                  nIdentBegin = -1;
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  continue;
                }
            }

          //  Preprocessor end: > or bracket
          if (dwCookie & COOKIE_PREPROCESSOR)
            {
              if (pszChars[I] == '>')
                {
                  dwCookie &= ~COOKIE_PREPROCESSOR;
                  nIdentBegin = -1;
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  continue;
                }
            }

          //  Preprocessor start: &
          if (!(dwCookie & COOKIE_EXT_USER1) && pszChars[I] == '&')
            {
              dwCookie |= COOKIE_USER1;
              nIdentBegin = -1;
              continue;
            }

          //  Preprocessor end: ;
          if (dwCookie & COOKIE_USER1)
            {
              if (pszChars[I] == ';')
                {
                  dwCookie &= ~COOKIE_USER1;
                  nIdentBegin = -1;
                  continue;
                }
            }

        }
    }

  if (nIdentBegin >= 0 && (dwCookie & COOKIE_PREPROCESSOR))
    {
      if (IsHtmlKeyword (pszChars + nIdentBegin, I - nIdentBegin) && (pszChars[nIdentBegin - 1] == _T ('<') || pszChars[nIdentBegin - 1] == _T ('/')))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
      else if (IsAspNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = TRUE;
                    }
                  break;
                }
            }
          if (bFunction)
            {
              DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }
  else if (nIdentBegin >= 0 && (dwCookie & COOKIE_EXT_USER1))
    {
      if (IsAspKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsAspNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = TRUE;
                    }
                  break;
                }
            }
          if (bFunction)
            {
              DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }

  dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_PREPROCESSOR | COOKIE_EXT_USER1);
  return dwCookie;
}

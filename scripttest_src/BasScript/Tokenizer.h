#if !defined(AFX_TOKENIZER_H__20050805_FACF_B73F_147C_0080AD509054__INCLUDED_)
#define AFX_TOKENIZER_H__20050805_FACF_B73F_147C_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define MAX_KEYWORD_LEN 32
#define MAX_STRING_LEN  300


typedef enum 
{
   OP_END = 0,
   OP_EOL,
   OP_INDENTIFIER,
   OP_OPERATOR,
   OP_COMMA,
   OP_DOT,
   OP_COLON,
   OP_SEMICOLON,
   OP_CHAR,
   OP_PAREN_L,
   OP_PAREN_R,
   OP_BRACE_L,
   OP_BRACE_R,
   OP_INTEGER,
   OP_NUMBER,
   OP_STRING,
   OP_EQUAL,
   OP_NOT,
   OP_PLUS,
   OP_MINUS,
   OP_PLUSEQUAL,
   OP_MINUSEQUAL,
   OP_DIV,
   OP_MUL,
   OP_OR,
   OP_AND,
   OP_XOR,
   OP_INC,
   OP_DEC,
   //
   OP_IF,
   OP_ELSE,
   OP_FUNCTION,
   OP_SWITCH,
   OP_CASE,
   OP_DEFAULT,
   OP_WHILE,
   OP_FOR,
   OP_BREAK,
   OP_FOREACH,
   OP_VAR,
   OP_RETURN,
   OP_USES,
} OPTYPE;


struct TOKEN
{
   TOKEN(OPTYPE t)
   {
      type = t;
      value[0] = '\0';
      pstr = NULL;
   }
   TOKEN(OPTYPE t, LPCWSTR p)
   {
      type = t;
      wcscpy(value, p);
      pstr = NULL;
   }
   TOKEN(OPTYPE t, WCHAR c)
   {
      type = t;
      value[0] = c;
      value[1] = '\0';
      pstr = NULL;
   }
   TOKEN(const TOKEN& t)
   {
      pstr = NULL;
      Copy(t);
   }
   ~TOKEN()
   {
      if( pstr != NULL ) free(pstr);
   }
   operator OPTYPE() const
   {
      return type;
   }
   operator LPCWSTR() const
   {
      return value;
   }
   const TOKEN* operator =(const TOKEN& t)
   {
      Copy(t);
      return this;
   }
   bool operator ==(LPCWSTR pstr) const
   {
      return wcscmp(value, pstr) == 0;
   }
   void Copy(const TOKEN& t)
   {
      if( pstr != NULL ) free(pstr); pstr = NULL;
      type = t.type;
      wcscpy(value, t.value);
      if( t.pstr == NULL ) pstr = NULL; else {
         pstr = (LPWSTR) malloc((wcslen(t.pstr) + 1) * sizeof(WCHAR));
         wcscpy(pstr, t.pstr);
      }
   }
   OPTYPE type;
   WCHAR value[MAX_KEYWORD_LEN];
   LPWSTR pstr;
};


class CTokenizer
{
public:
   LPCWSTR m_pstr;
   int m_iLineNo;

   CTokenizer() : m_pstr(NULL), m_iLineNo(1)
   {
   }
   void Assign(LPCWSTR pstr, int iLineNo)
   {
      m_pstr = pstr;
      m_iLineNo = iLineNo;
   }

   const TOKEN GetToken()
   {
resume:
      while( *m_pstr && (*m_pstr == ' ' || *m_pstr == '\t' || *m_pstr == '\r') ) m_pstr++;
      if( *m_pstr == '\0' ) return OP_END;
      switch( *m_pstr ) {
      case '\n':
         m_pstr++;
         m_iLineNo++;
         goto resume;
      case ',':
         return m_pstr++, TOKEN(OP_COMMA);
      case '.':
         return m_pstr++, TOKEN(OP_DOT);
      case ':':
         return m_pstr++, TOKEN(OP_COLON);
      case ';':
         return m_pstr++, TOKEN(OP_SEMICOLON);
      case '(':
         return m_pstr++, TOKEN(OP_PAREN_L);
      case ')':
         return m_pstr++, TOKEN(OP_PAREN_R);
      case '{':
         return m_pstr++, TOKEN(OP_BRACE_L);
      case '}':
         return m_pstr++, TOKEN(OP_BRACE_R);
      case '-':
         m_pstr++;
         if( *m_pstr == '-' ) return m_pstr++, TOKEN(OP_DEC);
         if( *m_pstr == '=' ) return m_pstr++, TOKEN(OP_MINUSEQUAL);
         return TOKEN(OP_MINUS);
      case '+':
         m_pstr++;
         if( *m_pstr == '+' ) return m_pstr++, TOKEN(OP_INC);
         if( *m_pstr == '=' ) return m_pstr++, TOKEN(OP_PLUSEQUAL);
         return TOKEN(OP_PLUS);
      case '&':
         m_pstr++;
         if( *m_pstr == '&' ) m_pstr++;
         return TOKEN(OP_AND);
      case '|':
         m_pstr++;
         if( *m_pstr == '|' ) m_pstr++;
         return TOKEN(OP_OR);
      case '~':
         m_pstr++;
         return TOKEN(OP_XOR);
      case '*':
         m_pstr++;
         return TOKEN(OP_MUL);
      case '/':
         m_pstr++;
         if( *m_pstr == '/' ) {
            while( *m_pstr != '\0' && *m_pstr != '\n' ) m_pstr++;
            goto resume;
         }
         if( *m_pstr == '*' ) {
            m_pstr++;
            while( *m_pstr != '\0' && !(*m_pstr == '*' && *(m_pstr + 1) == '/') ) m_pstr++;
            if( *m_pstr != '\0' ) m_pstr += 2;
            goto resume;
         }
         return TOKEN(OP_DIV);
      case '=':
      case '<':
      case '>':
      case '!':
         {
            TOKEN t(OP_OPERATOR);
            LPWSTR p = t.value;
            *p++ = *m_pstr++;
            if( *m_pstr == '<' || *m_pstr == '>' || *m_pstr == '=' ) *p++ = *m_pstr++;
            *p = '\0';
            if( t == L"!" ) return t.type = OP_NOT, t;
            if( t == L"=" ) return t.type = OP_EQUAL, t;
            return t;
         }
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         {
            TOKEN t(OP_INTEGER);
            LPWSTR p = t.value;
            int cCount = 0;
            while( *m_pstr && *m_pstr >= '0' && *m_pstr <= '9' ) {
               *p++ = *m_pstr++;
               if( *m_pstr == '.' ) t.type = OP_NUMBER, *p++ = *m_pstr++;
               if( ++cCount >= MAX_KEYWORD_LEN ) return TOKEN(OP_END);
            }
            *p = '\0';
            return t;
         }
      case '\"':
         WCHAR chDelim = *m_pstr;
         m_pstr++; // skip quote
         TOKEN t(OP_STRING);
         LPWSTR p = t.pstr = (LPWSTR) malloc((MAX_STRING_LEN + 1) * sizeof(WCHAR));
         int cCount = 0;
         while( *m_pstr && *m_pstr != chDelim && *m_pstr != '\n' ) {
            if( *m_pstr == '\\' ) {
               switch( *++m_pstr ) {
               case '"': *p++ = '\"'; m_pstr++; break;
               case 'n': *p++ = '\n'; m_pstr++; break;
               case 'r': *p++ = '\r'; m_pstr++; break;
               case 't': *p++ = '\t'; m_pstr++; break;
               default:
                  *p++ = *m_pstr++;
                  break;
               }
            }
            else {
               *p++ = *m_pstr++;
            }
            if( ++cCount >= MAX_STRING_LEN ) return TOKEN(OP_END);
         }
         *p = '\0';
         if( *m_pstr != chDelim ) return TOKEN(OP_END);
         m_pstr++; // skip quote
         return t;
      }

      TOKEN t(OP_INDENTIFIER);
      LPWSTR p = t.value;
      int cCount = 0;
      while( *m_pstr && (isalnum(*m_pstr) || *m_pstr == '_') ) {
         *p++ = *m_pstr++;
         if( ++cCount >= MAX_KEYWORD_LEN ) return TOKEN(OP_END);
      }
      *p = '\0';

      typedef struct
      {
         LPWSTR pstrName;
         OPTYPE Op;
      } KEYWORD;
      static KEYWORD Keywords[] = 
      {
         { L"break",    OP_BREAK },
         { L"case",     OP_CASE },
         { L"default",  OP_DEFAULT },
         { L"else",     OP_ELSE },
         { L"for",      OP_FOR },
         { L"foreach",  OP_FOREACH },
         { L"function", OP_FUNCTION },
         { L"if",       OP_IF },
         { L"return",   OP_RETURN },
         { L"switch",   OP_SWITCH },
         { L"uses",     OP_USES },
         { L"var",      OP_VAR },
         { L"while",    OP_WHILE },
         { NULL,        OP_END }
      };
      for( KEYWORD* k = Keywords; k->Op != OP_END; k++ ) {
         int iCmp = wcscmp(k->pstrName, t);
         if( iCmp > 0 ) break;
         if( iCmp == 0 ) {
            t.type = k->Op;
            return t;
         }
      }

      if( wcslen(t.value) == 0 ) {
         return TOKEN(OP_CHAR, *m_pstr++);
      }
      return t;
   }
   TOKEN PeekToken()
   {
      // TODO: Optimize with a cache of last token!
      LPCWSTR pstrOld = m_pstr; // backup old text position
      TOKEN t = GetToken();
      m_pstr = pstrOld;
      return t;
   }
   void SkipTo(TOKEN& t, OPTYPE Stop)
   {
      int iBrace = 0;
      int iParen = 0;
      for( ; ; ) {
         if( t == OP_BRACE_R ) iBrace--;
         else if( t == OP_PAREN_R ) iParen--;
         if( t == Stop && iBrace <= 0 && iParen <= 0 ) {
            t = GetToken();
            return;
         }
         if( t == OP_END ) return;
         else if( t == OP_BRACE_L ) iBrace++;
         else if( t == OP_PAREN_L ) iParen++;
         t = GetToken();
      }
   }
};


#endif // !defined(AFX_TOKENIZER_H__20050805_FACF_B73F_147C_0080AD509054__INCLUDED_)

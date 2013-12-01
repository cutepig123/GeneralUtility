
#include "stdafx.h"

#include "ScriptEngine.h"

#include <comdef.h>


void CScriptEngine::__level6(VARIANT* pVar)
{
   switch( t ) {
   case OP_PAREN_L:
      {
         t = tok.GetToken();
         _variant_t var;
         __level1(&var);
         if( t != OP_PAREN_R ) throw EXCEP_PAREN;
         *pVar = var.Detach();
      }
      break;
   case OP_STRING:
      {
         _variant_t var = t.pstr;
         *pVar = var.Detach();
      }
      break;
   case OP_INTEGER:
      {
         _variant_t var = t.value;
         var.ChangeType(VT_I4);
         *pVar = var.Detach();
      }
      break;
   case OP_NUMBER:
      {
         _variant_t var = t.value;
         _com_util::CheckError(::VariantChangeTypeEx(&var, &var, 1033, 0, VT_R8));
         *pVar = var.Detach();
      }
      break;
   case OP_INDENTIFIER:
      {
         __var_retrieve(pVar);
         switch( t ) {
         case OP_DOT:
            {
               t = tok.GetToken();
               if( t != OP_INDENTIFIER ) throw EXCEP_NAMEEXPECTED;
               if( t == L"toString" ) _com_util::CheckError(::VariantChangeType(pVar, pVar, 0, VT_BSTR));
               else if( t == L"toDate" ) _com_util::CheckError(::VariantChangeType(pVar, pVar, 0, VT_DATE));
               else if( t == L"toInteger" ) _com_util::CheckError(::VariantChangeType(pVar, pVar, 0, VT_I4));
               else if( t == L"toNumber" ) _com_util::CheckError(::VariantChangeTypeEx(pVar, pVar, 1033, 0, VT_R8));
               else throw EXCEP_NOTFOUND;
               if( tok.GetToken() != OP_PAREN_L ) throw EXCEP_PAREN;
               if( tok.GetToken() != OP_PAREN_R ) throw EXCEP_PAREN;
            }
            break;
         case OP_INC:
         case OP_DEC:
            {
               _variant_t one(1L);
               if( t == OP_INC ) _com_util::CheckError(::VarAdd(pVar, &one, pVar));
               else _com_util::CheckError(::VarSub(pVar, &one, pVar));
            }
            break;
         default:
            return;
         }
      }
      break;
   }
   t = tok.GetToken();
}

void CScriptEngine::__level5(VARIANT* pVar)
{
   bool bUnary = false;
   if( t == OP_PLUS || t == OP_MINUS ) {
      bUnary = true;
      t = tok.GetToken();
   }
   __level6(pVar);
   if( bUnary )  _com_util::CheckError(::VarNeg(pVar, pVar));
}

void CScriptEngine::__level4(VARIANT* pVar)
{
   __level5(pVar);
   while( t == OP_NOT ) {
      _com_util::CheckError(::VarNot(pVar, pVar));
      t = tok.GetToken();
   }
}

void CScriptEngine::__level3(VARIANT* pVar)
{
   __level4(pVar);
   while( t == OP_MUL || t == OP_DIV ) {
      TOKEN tOp = t;
      t = tok.GetToken();
      _variant_t var;
      __level4(&var);
      switch( tOp ) {
      case OP_MUL:
         _com_util::CheckError(::VarMul(pVar, &var, pVar));
         break;
      case OP_DIV:
         _com_util::CheckError(::VarDiv(pVar, &var, pVar));
         break;
      }
   }
}

void CScriptEngine::__level2(VARIANT* pVar)
{
   __level3(pVar);
   while( t == OP_PLUS || t == OP_MINUS ) {
      TOKEN tOp = t;
      t = tok.GetToken();
      _variant_t var;
      __level3(&var);
      switch( tOp ) {
      case OP_PLUS:
         var.ChangeType(pVar->vt);
         _com_util::CheckError(::VarAdd(pVar, &var, pVar));
         break;
      case OP_MINUS:
         var.ChangeType(pVar->vt);
         _com_util::CheckError(::VarSub(pVar, &var, pVar));
         break;
      }
   }
}

void CScriptEngine::__level1(VARIANT* pVar)
{
   __level2(pVar);
   while( t == OP_OPERATOR ) {
      TOKEN tOp = t;
      t = tok.GetToken();
      _variant_t var;
      __level2(&var);
      HRESULT Hr = ::VarCmp(pVar, &var, LOCALE_USER_DEFAULT, 0);
      if( tOp == L"==" ) var = Hr == VARCMP_EQ;
      else if( tOp == L"!=" ) var = Hr != VARCMP_EQ;
      else if( tOp == L"<" ) var = Hr == VARCMP_LT;
      else if( tOp == L">" ) var = Hr == VARCMP_GT;
      else if( tOp == L"<=" ) var = Hr == VARCMP_LT || Hr == VARCMP_EQ;
      else if( tOp == L">=" ) var = Hr == VARCMP_GT || Hr == VARCMP_EQ;
      ::VariantCopy(pVar, &var);
   }
}

void CScriptEngine::__level0(VARIANT* pVar)
{
   __level1(pVar);
   while( t == OP_AND || t == OP_OR || t == OP_XOR ) {
      TOKEN tOp = t;
      t = tok.GetToken();
      _variant_t var;
      __level1(&var);
      switch( tOp ) {
      case OP_AND:
          _com_util::CheckError(::VarAnd(pVar, &var, pVar));
         break;
      case OP_OR:
          _com_util::CheckError(::VarOr(pVar, &var, pVar));
         break;
      case OP_XOR:
          _com_util::CheckError(::VarXor(pVar, &var, pVar));
         break;
      }
   }
}

void CScriptEngine::__eval(VARIANT* pVar)
{
   __level0(pVar);
   while( t == OP_COMMA ) {
      t = tok.GetToken();
      __level0(pVar);
   }
}

int CScriptEngine::__pushstack()
{
   STACKLEVEL& stack = m_Stack[m_nStackLevel];
   stack.iVarIndex = m_nVars;
   stack.pstrCode = tok.m_pstr;
   stack.iLineNo = tok.m_iLineNo;
   stack.t = t.type;
   ::VariantInit(&stack.vRet);
   if( ++m_nStackLevel >= MAX_STACKDEPTH ) throw EXCEP_STACKOVERFLOW;
   return m_nStackLevel;
}

void CScriptEngine::__popstack(int iStack)
{
   STACKLEVEL& stack = m_Stack[iStack - 1];
   int i;
   for( i = m_nVars - 1; i >= stack.iVarIndex; --i ) ::VariantClear(&m_vars[i].vValue);
   for( i = m_nStackLevel - 1; i >= iStack - 1; --i ) ::VariantClear(&m_Stack[i].vRet);
   tok.Assign(stack.pstrCode, stack.iLineNo);
   m_nVars = stack.iVarIndex;   
   t = TOKEN(stack.t);
   m_nStackLevel = iStack - 1;
}

void CScriptEngine::__block()
{
   if( t == OP_SEMICOLON ) return;
   if( t != OP_BRACE_L ) {
      __statement();
      t = tok.GetToken();
   }
   else {
      int iStack = __pushstack();
      __exec(OP_BRACE_R);
      __popstack(iStack);
      tok.SkipTo(t, OP_BRACE_R);
   }
   if( t == OP_SEMICOLON ) t = tok.GetToken();
}

void CScriptEngine::__var_assign()
{
   TOKEN tName = t;
   _variant_t res;
   TOKEN tOp = tok.PeekToken();
   switch( tOp ) {
   case OP_PAREN_L:
      {
         _variant_t v;
         __var_retrieve(&v);
      }
      return;
   case OP_EQUAL:
   case OP_PLUSEQUAL:
   case OP_MINUSEQUAL:
      tok.GetToken();
      t = tok.GetToken();
      __eval(&res);
      break;
   case OP_INC:
   case OP_DEC:
      __eval(&res);
      break;
   case OP_DOT:
      break;
   default:
      throw EXCEP_SYNTAXERROR;
   }  
   int i;
   for(  i = m_nVars - 1; i >= 0; --i ) {
      if( tName == m_vars[i].szName ) {
         switch( tOp ) {
         case OP_INC:
         case OP_DEC:
         case OP_EQUAL:      _com_util::CheckError(::VariantCopy(&m_vars[i].vValue, &res)); break;
         case OP_PLUSEQUAL:  _com_util::CheckError(::VarAdd(&m_vars[i].vValue, &res, &m_vars[i].vValue)); break;
         case OP_MINUSEQUAL: _com_util::CheckError(::VarSub(&m_vars[i].vValue, &res, &m_vars[i].vValue)); break;
         default: throw EXCEP_SYNTAXERROR;
         }         
         return;
      }
   }
   for( i = 0; i < m_nObjects; i++ ) {
      if( wcslen(m_Objects[i].szName) == 0 ) {
         if( m_Objects[i].pCallback->SetProperty(tName, res) ) return;
      }
   }
   for( i = 0; i < m_nObjects; i++ ) {
      if( tName == m_Objects[i].szName ) {
         if( tok.GetToken() != OP_DOT ) throw EXCEP_NAMEEXPECTED;
         t = tok.GetToken();
         tName = t;
         if( tok.PeekToken() == OP_EQUAL ) {
            tok.GetToken();
            t = tok.GetToken();
            __eval(&res);
         }
         if( !m_Objects[i].pCallback->SetProperty(tName, res) ) throw EXCEP_NOTFOUND;
         return;
      }
   }
   throw EXCEP_NOTFOUND;
}

void CScriptEngine::__var_retrieve(VARIANT* pVar)
{
   int i;
   for( i = m_nVars - 1; i >= 0; --i ) {
      if( t == m_vars[i].szName ) {
         ::VariantCopy(pVar, &m_vars[i].vValue);
         t = tok.GetToken();
         return;
      }
   }
   for( i = 0; i < m_nObjects; i++ ) {
      if( t == m_Objects[i].szName ) {
         if( pVar->vt == VT_UNKNOWN ) pVar->vt = VT_ERROR;
         if( tok.GetToken() != OP_DOT ) throw EXCEP_NAMEEXPECTED;
         TOKEN tName = tok.GetToken();
         t = tok.GetToken();
         if( t == OP_PAREN_L ) {
            t = tok.GetToken();
            int nArgs = MAX_ARGUMENTS;
            _variant_t args[MAX_ARGUMENTS];
            __var_getargs(args, nArgs);
            if( !m_Objects[i].pCallback->Method(tName, args, nArgs, pVar) ) throw EXCEP_NOTFOUND;
         }
         else {
            if( !m_Objects[i].pCallback->GetProperty(tName, pVar) ) throw EXCEP_NOTFOUND;
         }
         if( pVar->vt == VT_ERROR ) throw EXCEP_ERROR;
         return;
      }
   }
   TOKEN tName = t;
   t = tok.GetToken();
   if( t == OP_PAREN_L ) 
   {
      t = tok.GetToken();
      int nArgs = MAX_ARGUMENTS;
      _variant_t args[MAX_ARGUMENTS];
      __var_getargs(args, nArgs);
      for( int i = 0; i < m_nFunctions; i++ ) {
         if( tName == m_Functions[i].szName ) {
            int iStack = __pushstack();
            tok.Assign(m_Functions[i].pstrCode, m_Functions[i].iLineNo);
            int i = 0;
            t = tok.GetToken();
            while( t == OP_INDENTIFIER ) {
               _variant_t v;
               if( i < nArgs ) v = args[i++];
               __var_define(t, v);
               t = tok.GetToken();
               if( t == OP_COMMA ) t = tok.GetToken();
               else if( t != OP_PAREN_R ) throw EXCEP_PAREN;
            }
            if( t != OP_PAREN_R ) throw EXCEP_PAREN;
            if( tok.GetToken() != OP_BRACE_L ) throw EXCEP_PAREN;
            try {
               __exec(OP_BRACE_R);
            }
            catch( EXCEP e ) {
               if( e != EXCEP_RETURN ) throw e;
               else ::VariantCopy(pVar, &m_Stack[iStack - 1].vRet);
            }
            __popstack(iStack);
            return;
         }
      }
      for( i = 0; i < m_nObjects; i++ ) {
         if( wcslen(m_Objects[i].szName) == 0 ) {
            if( m_Objects[i].pCallback->Method(tName, args, nArgs, pVar) ) {
               if( pVar->vt == VT_ERROR ) throw EXCEP_ERROR;
               return;
            }
         }
      }
   }
   else
   {
      for( i = 0; i < m_nObjects; i++ ) {
         if( wcslen(m_Objects[i].szName) == 0 ) {
            if( m_Objects[i].pCallback->GetProperty(tName, pVar) ) {
               if( pVar->vt == VT_ERROR || pVar->vt == VT_UNKNOWN ) throw EXCEP_ERROR;
               return;
            }
         }
      }
   }
   throw EXCEP_NOTFOUND;
}

void CScriptEngine::__break()
{
   throw EXCEP_BREAK;
}

void CScriptEngine::__return()
{
   if( m_nStackLevel == 0 ) throw EXCEP_SCOPE;
   STACKLEVEL& stack = m_Stack[m_nStackLevel - 1];
   t = tok.GetToken();
   __eval(&stack.vRet);
   if( t != OP_SEMICOLON ) throw EXCEP_SYNTAXERROR;
   for( int i = 0; i < m_nStackLevel - 1; i++ ) ::VariantCopy(&m_Stack[i].vRet, &stack.vRet);
   throw EXCEP_RETURN;
}

void CScriptEngine::__func_define()
{
   if( m_nStackLevel != 0 ) throw EXCEP_SCOPE;
   TOKEN tName = tok.GetToken();
   if( tName != OP_INDENTIFIER ) throw EXCEP_NAMEEXPECTED;
   for( int i = 0; i < m_nFunctions; i++ ) {
      if( t == m_Functions[i].szName ) throw EXCEP_DUPLICATE;
   }
   FUNCTIONDEF& func = m_Functions[m_nFunctions];
   wcscpy(func.szName, tName);
   if( tok.GetToken() != OP_PAREN_L ) throw EXCEP_PAREN;
   func.pstrCode = tok.m_pstr;
   func.iLineNo = tok.m_iLineNo;
   tok.SkipTo(t, OP_PAREN_R);
   if( t != OP_BRACE_L ) throw EXCEP_PAREN;
   tok.SkipTo(t, OP_BRACE_R);
   if( ++m_nFunctions >= MAX_FUNCTIONS ) throw EXCEP_CODEOVERFLOW;
}

void CScriptEngine::__var_getargs(VARIANT* pArgs, int& nArgs)
{
   int nMaxArgs = nArgs;
   nArgs = 0;
   while( t != OP_PAREN_R ) {
      if( nArgs >= nMaxArgs ) throw EXCEP_PARAMOVERFLOW;
      __level0(&pArgs[nArgs++]);
      if( t == OP_COMMA ) t = tok.GetToken();
      else if( t != OP_PAREN_R ) throw EXCEP_PAREN;
   }
   if( t != OP_PAREN_R ) throw EXCEP_PAREN;
   t = tok.GetToken();
}

void CScriptEngine::__var_define()
{
   do {
      TOKEN tName = tok.GetToken();
      if( tName != OP_INDENTIFIER ) throw EXCEP_NAMEEXPECTED;
      int iNameIndex = m_nStackLevel == 0 ? 0 : m_Stack[m_nStackLevel - 1].iVarIndex + 1;
      while( iNameIndex < m_nVars ) {
         if( tName == m_vars[iNameIndex++].szName ) throw EXCEP_DUPLICATE;
      }
      VARIABLE& var = m_vars[m_nVars];
      _variant_t v;
      __var_define(tName, v);
      t = tok.GetToken();
      if( t == OP_EQUAL ) {
         t = tok.GetToken();
         __level0(&var.vValue);
      }
   } while( t == OP_COMMA );
   if( t != OP_SEMICOLON ) throw EXCEP_SYNTAXERROR;
   t = tok.GetToken();
}

void CScriptEngine::__var_define(LPCWSTR pstrName, VARIANT& vValue)
{
   VARIABLE& var = m_vars[m_nVars];
   wcscpy(var.szName, pstrName);
   ::VariantInit(&var.vValue);
   ::VariantCopy(&var.vValue, &vValue);
   if( ++m_nVars >= MAX_VARIABLES ) throw EXCEP_CODEOVERFLOW;
}

void CScriptEngine::__forloop()
{
   if( tok.GetToken() != OP_PAREN_L ) throw EXCEP_PAREN;
   int iStack = __pushstack();
   t = tok.GetToken();
   if( t == OP_VAR ) __var_define(); else __var_assign();
   LPCWSTR pstrEval = tok.m_pstr;
   for( ; ; ) {
      t = tok.GetToken();
      _variant_t eval;
      __eval(&eval);
      if( eval != _variant_t(true) ) break;
      if( t != OP_SEMICOLON ) throw EXCEP_SYNTAXERROR;
      LPCWSTR pstrContinue = tok.m_pstr;
      tok.SkipTo(t, OP_PAREN_R);
      try {
         __block();
      }
      catch( EXCEP e ) {
         if( e == EXCEP_BREAK ) break;
         else throw e;
      }
      tok.m_pstr = pstrContinue;
      t = tok.GetToken();
      if( t != OP_PAREN_R ) __var_assign();
      if( t != OP_PAREN_R ) throw EXCEP_PAREN;
      tok.m_pstr = pstrEval;
   }
   __popstack(iStack);
   tok.SkipTo(t, OP_BRACE_R);
}

void CScriptEngine::__whileloop()
{
   t = tok.GetToken();
   if( t != OP_PAREN_L ) throw EXCEP_PAREN;
   int iStack = __pushstack();
   for( LPCWSTR pstrContinue = tok.m_pstr; ; tok.m_pstr = pstrContinue ) {
      t = tok.GetToken();
      _variant_t eval;
      __eval(&eval);
      if( eval != _variant_t(true) ) break;
      if( t != OP_PAREN_R ) throw EXCEP_PAREN;
      t = tok.GetToken();
      try {
         __block();
      }
      catch( EXCEP e ) {
         if( e == EXCEP_BREAK ) break;
         else throw e;
      }
   }
   __popstack(iStack);
   tok.SkipTo(t, OP_BRACE_R);
}

void CScriptEngine::__ifcondit()
{
   if( tok.GetToken() != OP_PAREN_L ) throw EXCEP_PAREN;
   t = tok.GetToken();
   _variant_t eval;
   __eval(&eval);
   t = tok.GetToken();
   if( eval == _variant_t(true) ) {
      __block();
      while( t == OP_ELSE ) {
         t = tok.GetToken();
         if( t == OP_IF ) {
            t = tok.GetToken();
            tok.SkipTo(t, OP_PAREN_R);
         }
         __skipblock();
      }
   }
   else {
      __skipblock();
      while( t == OP_ELSE ) {
         t = tok.GetToken();
         if( t == OP_IF ) __ifcondit();
         else __block();
      }
   }
}

void CScriptEngine::__switchcondit()
{
   if( tok.GetToken() != OP_PAREN_L ) throw EXCEP_PAREN;
   t = tok.GetToken();
   _variant_t eval;
   __eval(&eval);
   if( t != OP_PAREN_R ) throw EXCEP_PAREN;   
   int iStack = __pushstack();
   if( tok.GetToken() != OP_BRACE_L ) throw EXCEP_PAREN;
   t = tok.GetToken();
   while( t == OP_CASE ) {
      t = tok.GetToken();
      _variant_t val;
      __eval(&val);
      if( t != OP_COLON ) throw EXCEP_SYNTAXERROR;
      if( eval == val ) {
         try {
            __exec(OP_BRACE_R);
         }
         catch( EXCEP e ) {
            if( e != EXCEP_BREAK ) throw e;
         }
         break;
      }
      else {
         while( t != OP_CASE && t != OP_DEFAULT && t != OP_BRACE_R && t != OP_END ) __skipblock();
         if( t == OP_DEFAULT ) {
            if( tok.GetToken() != OP_COLON ) throw EXCEP_SYNTAXERROR;
            t = tok.GetToken();
            __block();
            break;
         }
      }
   }
   __popstack(iStack);
   tok.SkipTo(t, OP_BRACE_R);
   return;
}

void CScriptEngine::__skipblock()
{
   if( t == OP_BRACE_L ) tok.SkipTo(t, OP_BRACE_R);
   else tok.SkipTo(t, OP_SEMICOLON);
}

void CScriptEngine::__statement()
{
   switch( t ) {
   case OP_VAR:
      __var_define();
      break;
   case OP_INDENTIFIER:
      __var_assign();
      break;
   case OP_FUNCTION:
      __func_define();
      break;
   case OP_IF:
      __ifcondit();
      break;
   case OP_FOR:
      __forloop();
      break;
   case OP_WHILE:
      __whileloop();
      break;
   case OP_SWITCH:
      __switchcondit();
      break;
   case OP_BRACE_L:
      __block();
      break;
   case OP_BREAK:
      __break();
      break;
   case OP_RETURN:
      __return();
      return;
   case OP_SEMICOLON:
      t = tok.GetToken();
      break;
   default:
      throw EXCEP_KEYWORD;
   }
}

void CScriptEngine::__exec(OPTYPE Stop)
{
   t = tok.GetToken();
   while( t != OP_END && t != Stop ) {
      __statement();
      if( m_bCancelled ) throw EXCEP_CANCELLED;
   }
}

void CScriptEngine::__run(LPCWSTR pstrCode)
{
   try {
      tok.Assign(pstrCode, 1);
      __exec(OP_END);
   }
   catch( EXCEP e ) {
      _InterpretError(e);
   }
   catch( _com_error c ) {
      _InterpretError(EXCEP_TYPEMISMATCH);
   }
}


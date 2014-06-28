Option Explicit
''
' This script gets the status of the translations.
'
' Copyright (C) 2007 by Tim Gerundt
' Released under the "GNU General Public License"
'
' ID line follows -- this is updated by SVN
' $Id: GetTranslationsStatus.vbs 7302 2010-10-15 07:18:48Z gerundt $

Const ForReading = 1
Const SvnWebUrlLanguages = "http://winmerge.svn.sourceforge.net/viewvc/winmerge/trunk/Src/Languages/"

Dim oFSO, bRunFromCmd

Set oFSO = CreateObject("Scripting.FileSystemObject")

bRunFromCmd = False
If LCase(oFSO.GetFileName(Wscript.FullName)) = "cscript.exe" Then
  bRunFromCmd = True
End If

Call Main

''
' ...
Sub Main
  Dim oTranslationsStatus
  Dim oLanguages, sLanguage
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  InfoBox "Creating translations status files...", 3
  
  Set oTranslationsStatus = CreateObject("Scripting.Dictionary")
  
  If (bRunFromCmd = True) Then 'If run from command line...
    Wscript.Echo "English"
  End If
  oTranslationsStatus.Add "English", GetTranslationsStatusFromPoFile("English.pot")
  
  Set oLanguages = GetLanguages
  For Each sLanguage In oLanguages.Keys 'For all languages...
    If (bRunFromCmd = True) Then 'If run from command line...
      Wscript.Echo sLanguage
    End If
    oTranslationsStatus.Add sLanguage, GetTranslationsStatusFromPoFile(oLanguages(sLanguage))
  Next
  
  CreateTranslationsStatusHtmlFile "TranslationsStatus.html", oTranslationsStatus
  
  CreateTranslationsStatusWikiFile "TranslationsStatus.wiki", oTranslationsStatus
  
  CreateTranslationsStatusXmlFile "TranslationsStatus.xml", oTranslationsStatus
  
  CreateTranslatorsListFile "../../Docs/Users/Translators.html", oTranslationsStatus
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox "Translations status files created, after " & Seconds & " second(s).", 10
End Sub

''
' ...
Function GetLanguages()
  Dim oLanguages, oFile
  
  Set oLanguages = CreateObject("Scripting.Dictionary")
  
  For Each oFile In oFSO.GetFolder(".").Files 'For all files in the current folder...
    If (LCase(oFSO.GetExtensionName(oFile.Name)) = "po") Then 'If a PO file...
      oLanguages.Add oFSO.GetBaseName(oFile.Name), oFile.Path
    End If
  Next
  Set GetLanguages = oLanguages
End Function

''
' ...
Class CStatus
  Public Count, Translated, Untranslated, Fuzzy
  Public PoRevisionDate, PotCreationDate
  Public Translators
  
  Private Sub Class_Initialize
    Count = 0
    Translated = 0
    Untranslated = 0
    Fuzzy = 0
    PoRevisionDate = ""
    PotCreationDate = ""
    Set Translators = CreateObject("Scripting.Dictionary")
  End Sub
  
  Public Sub AddTranslator(ByVal sTranslator, ByVal bMaintainer)
    Dim Translator
    
    Set Translator = New CTranslator
    Translator.Maintainer = bMaintainer
    Translator.Mail = Trim(GetRegExpSubMatch(sTranslator, "<(.*)>"))
    If (Translator.Mail <> "") Then 'If mail address exists...
      Translator.Name = Trim(GetRegExpSubMatch(sTranslator, "(.*) <.*>"))
    Else 'If mail address NOT exists...
      Translator.Name = sTranslator
    End If
    Translators.Add Translators.Count, Translator
  End Sub
End Class

''
' ...
Class CTranslator
  Public Name, Mail, Maintainer
End Class

''
' ...
Function GetTranslationsStatusFromPoFile(ByVal sPoPath)
  Dim oStatus, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgId, sMsgStr, bFuzzy, bMaintainer
  Dim reMsgId, reMsgStr, reMsgContinued
  
  Set reMsgId = New RegExp
  reMsgId.Pattern = "^msgid ""(.*)""$"
  reMsgId.IgnoreCase = True
  
  Set reMsgStr = New RegExp
  reMsgStr.Pattern = "^msgstr ""(.*)""$"
  reMsgStr.IgnoreCase = True
  
  Set reMsgContinued = New RegExp
  reMsgContinued.Pattern = "^""(.*)""$"
  reMsgContinued.IgnoreCase = True
  
  Set oStatus = New CStatus
  If (oFSO.FileExists(sPoPath) = True) Then 'If the PO file exists...
    iMsgStarted = 0
    sMsgId = ""
    sMsgStr = ""
    bFuzzy = False
    bMaintainer = False
    Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      If (sLine <> "") Then 'If NOT empty line...
        If (Left(sLine, 1) <> "#") Then 'If NOT comment line...
          If reMsgId.Test(sLine) Then 'If "msgid"...
            iMsgStarted = 1
            Set oMatch = reMsgId.Execute(sLine)(0)
            sMsgId = oMatch.SubMatches(0)
          ElseIf reMsgStr.Test(sLine) Then 'If "msgstr"...
            iMsgStarted = 2
            Set oMatch = reMsgStr.Execute(sLine)(0)
            sMsgStr = oMatch.SubMatches(0)
          ElseIf reMsgContinued.Test(sLine) Then 'If "msgid" or "msgstr" continued...
            Set oMatch = reMsgContinued.Execute(sLine)(0)
            If (iMsgStarted = 1) Then
              sMsgId = sMsgId & oMatch.SubMatches(0)
            ElseIf (iMsgStarted = 2) Then
              sMsgStr = sMsgStr & oMatch.SubMatches(0)
            End If
          End If
        Else 'If comment line...
          iMsgStarted = -1
          If (Left(sLine, 2) = "#,") Then 'If "Reference" line...
            If (InStr(sLine, "fuzzy") > 0) Then 'If "fuzzy"...
              bFuzzy = True
            End If
          ElseIf (sLine = "# Maintainer:") Then 'If maintainer list starts...
            bMaintainer = True
          ElseIf (sLine = "# Translators:") Then 'If translators list starts...
            bMaintainer = False
          ElseIf (Left(sLine, 4) = "# * ") Then 'If translator/maintainer...
            oStatus.AddTranslator Trim(Mid(sLine, 5)), bMaintainer
          End If
        End If
      Else 'If empty line
        iMsgStarted = 0
      End If
      
      If (iMsgStarted = 0) Then 'If NOT inside a translation...
        If (sMsgId <> "") Then
          oStatus.Count = oStatus.Count + 1
          If (bFuzzy = False) Then 'If NOT a fuzzy translation...
            If (sMsgStr <> "") Then
              oStatus.Translated = oStatus.Translated + 1
            Else
              oStatus.Untranslated = oStatus.Untranslated + 1
            End If
          Else 'If a fuzzy translation...
            oStatus.Fuzzy = oStatus.Fuzzy + 1
          End If
        ElseIf(sMsgStr <> "") Then
          oStatus.PoRevisionDate = GetRegExpSubMatch(sMsgStr, "PO-Revision-Date: ([0-9 :\+\-]+)")
          oStatus.PotCreationDate = GetRegExpSubMatch(sMsgStr, "POT-Creation-Date: ([0-9 :\+\-]+)")
        End If
        sMsgId = ""
        sMsgStr = ""
        bFuzzy = False
      End If
    Loop
    oTextFile.Close
  End If
  Set GetTranslationsStatusFromPoFile = oStatus
End Function

''
' ...
Sub CreateTranslationsStatusHtmlFile(ByVal sHtmlPath, ByVal oTranslationsStatus)
  Dim oHtmlFile, sLanguage, oLanguageStatus, i
  Dim sMaintainerStart, sMaintainerEnd
  
  Set oHtmlFile = oFSO.CreateTextFile(sHtmlPath, True)
  
  oHtmlFile.WriteLine "<!DOCTYPE HTML PUBLIC ""-//W3C//DTD HTML 4.01 Transitional//EN"""
  oHtmlFile.WriteLine "  ""http://www.w3.org/TR/html4/loose.dtd"">"
  oHtmlFile.WriteLine "<html>"
  oHtmlFile.WriteLine "<head>"
  oHtmlFile.WriteLine "  <title>Translations Status</title>"
  oHtmlFile.WriteLine "  <meta http-equiv=""content-type"" content=""text/html; charset=ISO-8859-1"">"
  oHtmlFile.WriteLine "  <style type=""text/css"">"
  oHtmlFile.WriteLine "  <!--"
  oHtmlFile.WriteLine "    body {"
  oHtmlFile.WriteLine "      font-family: Verdana,Helvetica,Arial,sans-serif;"
  oHtmlFile.WriteLine "      font-size: small;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    code,pre {"
  oHtmlFile.WriteLine "      font-family: ""Courier New"",Courier,monospace;"
  oHtmlFile.WriteLine "      font-size: 1em;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    .status {"
  oHtmlFile.WriteLine "      border-collapse: collapse;"
  oHtmlFile.WriteLine "      border: 1px solid #aaaaaa;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    .status th {"
  oHtmlFile.WriteLine "      padding: 3px;"
  oHtmlFile.WriteLine "      background: #f2f2f2;"
  oHtmlFile.WriteLine "      border: 1px solid #aaaaaa;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    .status td {"
  oHtmlFile.WriteLine "      padding: 3px;"
  oHtmlFile.WriteLine "      background: #f9f9f9;"
  oHtmlFile.WriteLine "      border: 1px solid #aaaaaa;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    .left { text-align: left; }"
  oHtmlFile.WriteLine "    .center { text-align: center; }"
  oHtmlFile.WriteLine "    .right { text-align: right; }"
  oHtmlFile.WriteLine "  -->"
  oHtmlFile.WriteLine "  </style>"
  oHtmlFile.WriteLine "</head>"
  oHtmlFile.WriteLine "<body>"
  oHtmlFile.WriteLine "<h1>Translations Status</h1>"
  oHtmlFile.WriteLine "<p>Status from <strong>" & GetCreationDate() & "</strong>:</p>"
  oHtmlFile.WriteLine "<table class=""status"">"
  oHtmlFile.WriteLine "  <tr>"
  oHtmlFile.WriteLine "    <th class=""left"">Language</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Total</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Translated</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Fuzzy</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Untranslated</th>"
  oHtmlFile.WriteLine "    <th class=""center"">Last Update</th>"
  oHtmlFile.WriteLine "  </tr>"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oHtmlFile.WriteLine "  <tr>"
      oHtmlFile.WriteLine "    <td class=""left"">" & sLanguage & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus.Count & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus.Translated & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus.Fuzzy & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus.Untranslated & "</td>"
      oHtmlFile.WriteLine "    <td class=""center"">" & Left(oLanguageStatus.PoRevisionDate, 10) & "</td>"
      oHtmlFile.WriteLine "  </tr>"
    End If
  Next
  Set oLanguageStatus = oTranslationsStatus("English")
  oHtmlFile.WriteLine "  <tr>"
  oHtmlFile.WriteLine "    <td class=""left"">English</td>"
  oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus.Count & "</td>"
  oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus.Count & "</td>"
  oHtmlFile.WriteLine "    <td class=""right"">0</td>"
  oHtmlFile.WriteLine "    <td class=""right"">0</td>"
  oHtmlFile.WriteLine "    <td class=""center"">" & Left(oLanguageStatus.PotCreationDate, 10) & "</td>"
  oHtmlFile.WriteLine "  </tr>"
  oHtmlFile.WriteLine "</table>"
  oHtmlFile.WriteLine "<h2>Translators</h2>"
  oHtmlFile.WriteLine "<table class=""status"">"
  oHtmlFile.WriteLine "  <tr>"
  oHtmlFile.WriteLine "    <th class=""left"">Language</th>"
  oHtmlFile.WriteLine "    <th class=""left"">Translator(s)</th>"
  oHtmlFile.WriteLine "  </tr>"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oHtmlFile.WriteLine "  <tr>"
      oHtmlFile.WriteLine "    <td>" & sLanguage & "</td>"
      oHtmlFile.WriteLine "    <td>"
      For i = 0 To oLanguageStatus.Translators.Count - 1 'For all translators...
        sMaintainerStart = ""
        sMaintainerEnd = ""
        If (oLanguageStatus.Translators(i).Maintainer = True) Then 'If maintainer...
          sMaintainerStart = "<strong title=""Maintainer"">"
          sMaintainerEnd = "</strong>"
        End If
        If (oLanguageStatus.Translators(i).Mail <> "") Then 'If mail address exists...
          oHtmlFile.WriteLine "      " & sMaintainerStart & "<a href=""mailto:" & oLanguageStatus.Translators(i).Mail & """>" & oLanguageStatus.Translators(i).Name & "</a>" & sMaintainerEnd & "<br>"
        Else 'If mail address NOT exists...
          oHtmlFile.WriteLine "      " & sMaintainerStart & oLanguageStatus.Translators(i).Name & sMaintainerEnd & "<br>"
        End If
      Next
      oHtmlFile.WriteLine "    </td>"
      oHtmlFile.WriteLine "  </tr>"
    End If
  Next
  oHtmlFile.WriteLine "</table>"
  oHtmlFile.WriteLine "</body>"
  oHtmlFile.WriteLine "</html>"
  oHtmlFile.Close
End Sub

''
' ...
Sub CreateTranslationsStatusWikiFile(ByVal sWikiPath, ByVal oTranslationsStatus)
  Dim oWikiFile, sLanguage, oLanguageStatus
  
  Set oWikiFile = oFSO.CreateTextFile(sWikiPath, True)
  
  oWikiFile.WriteLine "== Translations Status =="
  oWikiFile.WriteLine "Status from '''" & GetCreationDate() & "''':"
  oWikiFile.WriteLine "{| class=""wikitable"" border=""1"""
  oWikiFile.WriteLine "! Language"
  oWikiFile.WriteLine "! Total"
  oWikiFile.WriteLine "! Translated"
  oWikiFile.WriteLine "! Fuzzy"
  oWikiFile.WriteLine "! Untranslated"
  oWikiFile.WriteLine "! Last Update"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oWikiFile.WriteLine "|-"
      oWikiFile.WriteLine "|align=""left""| " & sLanguage
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus.Count
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus.Translated
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus.Fuzzy
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus.Untranslated
      oWikiFile.WriteLine "|align=""center""| " & Left(oLanguageStatus.PoRevisionDate, 10)
    End If
  Next
  Set oLanguageStatus = oTranslationsStatus("English")
  oWikiFile.WriteLine "|-"
  oWikiFile.WriteLine "|align=""left""| English"
  oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus.Count
  oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus.Count
  oWikiFile.WriteLine "|align=""right""| 0"
  oWikiFile.WriteLine "|align=""right""| 0"
  oWikiFile.WriteLine "|align=""center""| " & Left(oLanguageStatus.PotCreationDate, 10)
  oWikiFile.WriteLine "|}"
  oWikiFile.Close
End Sub

''
' ...
Sub CreateTranslationsStatusXmlFile(ByVal sXmlPath, ByVal oTranslationsStatus)
  Dim oXmlFile, sLanguage, oLanguageStatus, i
  
  Set oXmlFile = oFSO.CreateTextFile(sXmlPath, True)
  
  oXmlFile.WriteLine "<translations>"
  oXmlFile.WriteLine "  <update>" & GetCreationDate() & "</update>"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oXmlFile.WriteLine "  <translation>"
      oXmlFile.WriteLine "    <language>" & sLanguage & "</language>"
      oXmlFile.WriteLine "    <file>" & sLanguage & ".po</file>"
      oXmlFile.WriteLine "    <update>" & Left(oLanguageStatus.PoRevisionDate, 10) & "</update>"
      oXmlFile.WriteLine "    <strings>"
      oXmlFile.WriteLine "      <count>" & oLanguageStatus.Count & "</count>"
      oXmlFile.WriteLine "      <translated>" & oLanguageStatus.Translated & "</translated>"
      oXmlFile.WriteLine "      <fuzzy>" & oLanguageStatus.Fuzzy & "</fuzzy>"
      oXmlFile.WriteLine "      <untranslated>" & oLanguageStatus.Untranslated & "</untranslated>"
      oXmlFile.WriteLine "    </strings>"
      oXmlFile.WriteLine "    <translators>"
      For i = 0 To oLanguageStatus.Translators.Count - 1 'For all translators...
        If (oLanguageStatus.Translators(i).Maintainer = True) Then 'If maintainer...
          oXmlFile.WriteLine "      <translator maintainer=""1"">"
        Else 'If NOT maintainer...
          oXmlFile.WriteLine "      <translator>"
        End If
        oXmlFile.WriteLine "        <name>" & oLanguageStatus.Translators(i).Name & "</name>"
        If (oLanguageStatus.Translators(i).Mail <> "") Then 'If mail address exists...
          oXmlFile.WriteLine "        <mail>" & oLanguageStatus.Translators(i).Mail & "</mail>"
        End If
        oXmlFile.WriteLine "      </translator>"
      Next
      oXmlFile.WriteLine "    </translators>"
      oXmlFile.WriteLine "  </translation>"
    End If
  Next
  Set oLanguageStatus = oTranslationsStatus("English")
  oXmlFile.WriteLine "  <translation>"
  oXmlFile.WriteLine "    <language>English</language>"
  oXmlFile.WriteLine "    <file>English.pot</file>"
  oXmlFile.WriteLine "    <update>" & Left(oLanguageStatus.PotCreationDate, 10) & "</update>"
  oXmlFile.WriteLine "    <strings>"
  oXmlFile.WriteLine "      <count>" & oLanguageStatus.Count & "</count>"
  oXmlFile.WriteLine "      <translated>" & oLanguageStatus.Count & "</translated>"
  oXmlFile.WriteLine "      <fuzzy>0</fuzzy>"
  oXmlFile.WriteLine "      <untranslated>0</untranslated>"
  oXmlFile.WriteLine "    </strings>"
  oXmlFile.WriteLine "  </translation>"
  oXmlFile.WriteLine "</translations>"
  oXmlFile.Close
End Sub

''
' ...
Sub CreateTranslatorsListFile(ByVal sHtmlPath, ByVal oTranslationsStatus)
  Dim oHtmlFile, sLanguage, oLanguageStatus, i
  Dim sLastUpdated, sMaintainer, sTranslators
  
  Set oHtmlFile = oFSO.CreateTextFile(sHtmlPath, True)
  
  oHtmlFile.WriteLine "<!DOCTYPE HTML PUBLIC ""-//W3C//DTD HTML 4.01 Transitional//EN"""
  oHtmlFile.WriteLine "  ""http://www.w3.org/TR/html4/loose.dtd"">"
  oHtmlFile.WriteLine "<html>"
  oHtmlFile.WriteLine "<head>"
  oHtmlFile.WriteLine "  <title>WinMerge Translators</title>"
  oHtmlFile.WriteLine "  <meta http-equiv=""content-type"" content=""text/html; charset=ISO-8859-1"">"
  oHtmlFile.WriteLine "  <style type=""text/css"">"
  oHtmlFile.WriteLine "  <!--"
  oHtmlFile.WriteLine "    body {"
  oHtmlFile.WriteLine "      font-family: Verdana,Helvetica,Arial,sans-serif;"
  oHtmlFile.WriteLine "      font-size: small;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    dl {"
  oHtmlFile.WriteLine "      width: 30em;"
  oHtmlFile.WriteLine "      margin: 1em;"
  oHtmlFile.WriteLine "      padding: .5em;"
  oHtmlFile.WriteLine "      background: #F9F9F9;"
  oHtmlFile.WriteLine "      border: 1px solid #AAAAAA;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    dt {"
  oHtmlFile.WriteLine "      margin-top: .5em;"
  oHtmlFile.WriteLine "      font-weight: bold;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "  -->"
  oHtmlFile.WriteLine "  </style>"
  oHtmlFile.WriteLine "</head>"
  oHtmlFile.WriteLine "<body>"
  oHtmlFile.WriteLine "<h1>WinMerge Translators</h1>"
  oHtmlFile.WriteLine "<ul>"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      oHtmlFile.WriteLine "  <li><a href=""#" & sLanguage & """>" & sLanguage & "</a></li>"
    End If
  Next
  oHtmlFile.WriteLine "</ul>"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      sMaintainer = ""
      sTranslators = ""
      
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oHtmlFile.WriteLine "<h2><a name=""" & sLanguage & """>" & sLanguage & "</a></h2>"
      oHtmlFile.WriteLine "<dl>"
      For i = 0 To oLanguageStatus.Translators.Count - 1 'For all translators...
        If (oLanguageStatus.Translators(i).Maintainer = True) Then 'If maintainer...
          If (oLanguageStatus.Translators(i).Mail <> "") Then 'If mail address exists...
            sMaintainer = "<dd><strong><a href=""mailto:" & oLanguageStatus.Translators(i).Mail & """>" & oLanguageStatus.Translators(i).Name & "</a></strong></dd>"
          Else 'If mail address NOT exists...
            sMaintainer = "<dd><strong>" & oLanguageStatus.Translators(i).Name & "</strong></dd>"
          End If
        Else 'If NO maintainer...
          If (oLanguageStatus.Translators(i).Mail <> "") Then 'If mail address exists...
            sTranslators = sTranslators & "<dd><a href=""mailto:" & oLanguageStatus.Translators(i).Mail & """>" & oLanguageStatus.Translators(i).Name & "</a></dd>"
          Else 'If mail address NOT exists...
            sTranslators = sTranslators & "<dd>" & oLanguageStatus.Translators(i).Name & "</dd>"
          End If
        End If
      Next
      If (sMaintainer <> "") Then 'If maintainer exists...
        oHtmlFile.WriteLine "  <dt>Maintainer:</dt>"
        oHtmlFile.WriteLine "  " & sMaintainer
      End If
      If (sTranslators <> "") Then 'If translators exists...
        oHtmlFile.WriteLine "  <dt>Translators:</dt>"
        oHtmlFile.WriteLine "  " & sTranslators
      End If
      sLastUpdated = Left(oLanguageStatus.PoRevisionDate, 10)
      If (sLastUpdated <> "") Then 'If PO revision date exists...
        oHtmlFile.WriteLine "  <dt>Last Update:</dt>"
        oHtmlFile.WriteLine "  <dd>" & Left(oLanguageStatus.PoRevisionDate, 10) & "</dd>"
      End If
      oHtmlFile.WriteLine "  <dt>Translation File:</dt>"
      oHtmlFile.WriteLine "  <dd><a href=""" & SvnWebUrlLanguages & sLanguage & ".po"" rel=""nofollow"">" & sLanguage & ".po</a></dd>"
      oHtmlFile.WriteLine "</dl>"
    End If
  Next
  oHtmlFile.WriteLine "<p>Status from <strong>" & GetCreationDate() & "</strong>. Look at <a href=""http://winmerge.org/translations/"">winmerge.org</a> for updates.</p>"
  oHtmlFile.WriteLine "</body>"
  oHtmlFile.WriteLine "</html>"
  oHtmlFile.Close
End Sub

''
' ...
Function FoundRegExpMatch(ByVal sString, ByVal sPattern, ByRef oMatchReturn)
  Dim oRegExp, oMatches
  
  Set oRegExp = New RegExp
  oRegExp.Pattern = sPattern
  oRegExp.IgnoreCase = True
  
  oMatchReturn = Null
  FoundRegExpMatch = False
  If (oRegExp.Test(sString) = True) Then
    Set oMatches = oRegExp.Execute(sString)
    Set oMatchReturn = oMatches(0)
    FoundRegExpMatch = True
  End If
End Function

''
' ...
Function GetRegExpSubMatch(ByVal sString, ByVal sPattern)
  Dim oMatch
  
  GetRegExpSubMatch = ""
  If (FoundRegExpMatch(sString, sPattern, oMatch)) Then 'If pattern found...
    GetRegExpSubMatch = oMatch.SubMatches(0)
  End If
End Function

''
' ...
Function GetCreationDate()
  Dim oNow, sYear, sMonth, sDay, sHour, sMinute
  
  oNow = Now()
  sYear = Year(oNow)
  sMonth = Month(oNow)
  If (sMonth < 10) Then sMonth = "0" & sMonth
  sDay = Day(oNow)
  If (sDay < 10) Then sDay = "0" & sDay
  
  GetCreationDate = sYear & "-" & sMonth & "-" & sDay
End Function

''
' ...
Function InfoBox(ByVal sText, ByVal iSecondsToWait)
  Dim oShell
  
  If (bRunFromCmd = False) Then 'If run from command line...
    Set oShell = Wscript.CreateObject("WScript.Shell")
    InfoBox = oShell.Popup(sText, iSecondsToWait, Wscript.ScriptName, 64)
  Else 'If NOT run from command line...
    Wscript.Echo sText
  End If
End Function

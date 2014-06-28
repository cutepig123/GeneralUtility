/** 
 * @file  OptionsDef.h
 *
 * @brief Constants for option-names
 */
// RCS ID line follows -- this is updated by CVS
// $Id: OptionsDef.h 6134 2008-11-28 19:58:25Z kimmov $

#ifndef _OPTIONS_DEF_H_
#define _OPTIONS_DEF_H_

#include <tchar.h>

// User's language
const TCHAR OPT_SELECTED_LANGUAGE[] = _T("Locale/LanguageId");

// View-menu
const TCHAR OPT_SHOW_UNIQUE_LEFT[] = _T("Settings/ShowUniqueLeft");
const TCHAR OPT_SHOW_UNIQUE_RIGHT[] = _T("Settings/ShowUniqueRight");
const TCHAR OPT_SHOW_DIFFERENT[] = _T("Settings/ShowDifferent");
const TCHAR OPT_SHOW_IDENTICAL[] = _T("Settings/ShowIdentical");
const TCHAR OPT_SHOW_BINARIES[] = _T("Settings/ShowBinaries");
const TCHAR OPT_SHOW_SKIPPED[] = _T("Settings/ShowSkipped");
const TCHAR OPT_TREE_MODE[] = _T("Settings/TreeMode");

// Show/hide toolbar/statusbar/tabbar
const TCHAR OPT_SHOW_TOOLBAR[] = _T("Settings/ShowToolbar");
const TCHAR OPT_SHOW_STATUSBAR[] = _T("Settings/ShowStatusbar");
const TCHAR OPT_SHOW_TABBAR[] = _T("Settings/ShowTabbar");
const TCHAR OPT_TOOLBAR_SIZE[] = _T("Settings/ToolbarSize");
const TCHAR OPT_RESIZE_PANES[] = _T("Settings/AutoResizePanes");

const TCHAR OPT_SYNTAX_HIGHLIGHT[] = _T("Settings/HiliteSyntax");
const TCHAR OPT_WORDDIFF_HIGHLIGHT[] = _T("Settings/HiliteWordDiff");
const TCHAR OPT_DISABLE_SPLASH[] = _T("Settings/DisableSplash");
const TCHAR OPT_VIEW_WHITESPACE[] =  _T("Settings/ViewWhitespace");
const TCHAR OPT_CONNECT_MOVED_BLOCKS[] = _T("Settings/ConnectMovedBlocks");
const TCHAR OPT_SCROLL_TO_FIRST[] =  _T("Settings/ScrollToFirst");

// Backup options
const TCHAR OPT_BACKUP_FOLDERCMP[] = _T("Backup/EnableFolder");
const TCHAR OPT_BACKUP_FILECMP[] = _T("Backup/EnableFile");
const TCHAR OPT_BACKUP_LOCATION[] = _T("Backup/Location");
const TCHAR OPT_BACKUP_GLOBALFOLDER[] = _T("Backup/GlobalFolder");
const TCHAR OPT_BACKUP_ADD_BAK[] = _T("Backup/NameAddBak");
const TCHAR OPT_BACKUP_ADD_TIME[] = _T("Backup/NameAddTime");

const TCHAR OPT_DIRVIEW_SORT_COLUMN[] =  _T("Settings/DirViewSortCol");
const TCHAR OPT_DIRVIEW_SORT_ASCENDING[] =  _T("Settings/DirViewSortAscending");
const TCHAR OPT_DIRVIEW_ENABLE_SHELL_CONTEXT_MENU[] = _T("Settings/DirViewEnableShellContextMenu");

// File compare
const TCHAR OPT_AUTOMATIC_RESCAN[] = _T("Settings/AutomaticRescan");
const TCHAR OPT_ALLOW_MIXED_EOL[] = _T("Settings/AllowMixedEOL");
const TCHAR OPT_TAB_SIZE[] = _T("Settings/TabSize");
const TCHAR OPT_TAB_TYPE[] = _T("Settings/TabType");
const TCHAR OPT_BREAK_ON_WORDS[] = _T("Settings/BreakOnWords");
const TCHAR OPT_BREAK_TYPE[] = _T("Settings/BreakType");
const TCHAR OPT_WORDWRAP[] = _T("Settings/WordWrap");
const TCHAR OPT_VIEW_LINENUMBERS[] = _T("Settings/ViewLineNumbers");
const TCHAR OPT_VIEW_FILEMARGIN[] = _T("Settings/ViewFileMargin");

const TCHAR OPT_EXT_EDITOR_CMD[] = _T("Settings/ExternalEditor");
const TCHAR OPT_USE_RECYCLE_BIN[] = _T("Settings/UseRecycleBin");
const TCHAR OPT_SINGLE_INSTANCE[] = _T("Settings/SingleInstance");
const TCHAR OPT_MERGE_MODE[] = _T("Settings/MergingMode");
const TCHAR OPT_CLOSE_WITH_ESC[] = _T("Settings/CloseWithEsc");
const TCHAR OPT_LOGGING[] = _T("Settings/Logging");
const TCHAR OPT_VERIFY_OPEN_PATHS[] = _T("Settings/VerifyOpenPaths");
const TCHAR OPT_AUTO_COMPLETE_SOURCE[] = _T("Settings/AutoCompleteSource");
const TCHAR OPT_IGNORE_SMALL_FILETIME[] = _T("Settings/IgnoreSmallFileTime");
const TCHAR OPT_ASK_MULTIWINDOW_CLOSE[] = _T("Settings/AskClosingMultipleWindows");
const TCHAR OPT_PRESERVE_FILETIMES[] = _T("Settings/PreserveFiletimes");

const TCHAR OPT_CP_DEFAULT_MODE[] = _T("Settings/CodepageDefaultMode");
const TCHAR OPT_CP_DEFAULT_CUSTOM[] = _T("Settings/CodepageDefaultCustomValue");
const TCHAR OPT_CP_DETECT[] = _T("Settings/CodepageDetection");

const TCHAR OPT_PROJECTS_PATH[] = _T("Settings/ProjectsPath");
const TCHAR OPT_USE_SYSTEM_TEMP_PATH[] = _T("Settings/UseSystemTempPath");
const TCHAR OPT_CUSTOM_TEMP_PATH[] = _T("Settings/CustomTempPath");

// Color options
// The difference color
const TCHAR OPT_CLR_DIFF[] = _T("Settings/DifferenceColor");
// The selected difference color
const TCHAR OPT_CLR_SELECTED_DIFF[] = _T("Settings/SelectedDifferenceColor");
// The difference deleted color
const TCHAR OPT_CLR_DIFF_DELETED[] = _T("Settings/DifferenceDeletedColor");
// The selected difference deleted color
const TCHAR OPT_CLR_SELECTED_DIFF_DELETED[] = _T("Settings/SelectedDifferenceDeletedColor");
// The difference text color
const TCHAR OPT_CLR_DIFF_TEXT[] = _T("Settings/DifferenceTextColor");
// The selected difference text color
const TCHAR OPT_CLR_SELECTED_DIFF_TEXT[] = _T("Settings/SelectedDifferenceTextColor");
// The ignored lines color
const TCHAR OPT_CLR_TRIVIAL_DIFF[] = _T("Settings/TrivialDifferenceColor");
// The ignored and deleted lines color
const TCHAR OPT_CLR_TRIVIAL_DIFF_DELETED[] = _T("Settings/TrivialDifferenceDeletedColor");
// The ignored text color
const TCHAR OPT_CLR_TRIVIAL_DIFF_TEXT[] = _T("Settings/TrivialDifferenceTextColor");
// The moved block color
const TCHAR OPT_CLR_MOVEDBLOCK[] = _T("Settings/MovedBlockColor");
// The moved block deleted lines color
const TCHAR OPT_CLR_MOVEDBLOCK_DELETED[] = _T("Settings/MovedBlockDeletedColor");
// The moved block text color
const TCHAR OPT_CLR_MOVEDBLOCK_TEXT[] = _T("Settings/MovedBlockTextColor");
// The selected moved block color
const TCHAR OPT_CLR_SELECTED_MOVEDBLOCK[] = _T("Settings/SelectedMovedBlockColor");
// The selected moved block deleted lines
const TCHAR OPT_CLR_SELECTED_MOVEDBLOCK_DELETED[] = _T("Settings/SelectedMovedBlockDeletedColor");
// The selected moved block text color
const TCHAR OPT_CLR_SELECTED_MOVEDBLOCK_TEXT[] = _T("Settings/SelectedMovedBlockTextColor");
// The word difference color
const TCHAR OPT_CLR_WORDDIFF[] = _T("Settings/WordDifferenceColor");
// The selected word difference color
const TCHAR OPT_CLR_SELECTED_WORDDIFF[] = _T("Settings/SelectedWordDifferenceColor");
// The word difference text color
const TCHAR OPT_CLR_WORDDIFF_TEXT[] = _T("Settings/WordDifferenceTextColor");
// The selected word difference text color
const TCHAR OPT_CLR_SELECTED_WORDDIFF_TEXT[] = _T("Settings/SelectedWordDifferenceTextColor");
// Whether to use default (theme) text colors
const TCHAR OPT_CLR_DEFAULT_TEXT_COLORING[] = _T("Settings/DefaultTextColoring");

// Compare options
const TCHAR OPT_CMP_IGNORE_WHITESPACE[] = _T("Settings/IgnoreSpace");
const TCHAR OPT_CMP_IGNORE_BLANKLINES[] = _T("Settings/IgnoreBlankLines");
const TCHAR OPT_CMP_FILTER_COMMENTLINES[] = _T("Settings/FilterCommentsLines");
const TCHAR OPT_CMP_IGNORE_CASE[] = _T("Settings/IgnoreCase");
const TCHAR OPT_CMP_IGNORE_EOL[] = _T("Settings/IgnoreEol");
const TCHAR OPT_CMP_METHOD[] = _T("Settings/CompMethod");
const TCHAR OPT_CMP_MOVED_BLOCKS[] = _T("Settings/MovedBlocks");
const TCHAR OPT_CMP_MATCH_SIMILAR_LINES[] = _T("Settings/MatchSimilarLines");
const TCHAR OPT_CMP_STOP_AFTER_FIRST[] = _T("Settings/StopAfterFirst");
const TCHAR OPT_CMP_QUICK_LIMIT[] = _T("Settings/QuickMethodLimit");

// Multidoc enable/disable per document type
const TCHAR OPT_MULTIDOC_DIRDOCS[] = _T("Settings/MultiDirDocs");
const TCHAR OPT_MULTIDOC_MERGEDOCS[] = _T("Settings/MultiMergeDocs");

/// Are regular expression linefilters enabled?
const TCHAR OPT_LINEFILTER_ENABLED[] = _T("Settings/IgnoreRegExp");
/// Currently selected filefilter
const TCHAR OPT_FILEFILTER_CURRENT[] = _T("Settings/FileFilterCurrent");
const TCHAR OPT_FILTER_USERPATH[] = _T("Settings/UserFilterPath");
const TCHAR OPT_FILEFILTER_SHARED[] = _T("Settings/Filters/Shared");

// Version control
const TCHAR OPT_VCS_SYSTEM[] = _T("Settings/VersionSystem");
const TCHAR OPT_VSS_PATH[] = _T("Settings/VssPath");

// Archive support
const TCHAR OPT_ARCHIVE_ENABLE[] = _T("Merge7z/Enable");
const TCHAR OPT_ARCHIVE_PROBETYPE[] = _T("Merge7z/ProbeSignature");

// Plugins
const TCHAR OPT_PLUGINS_ENABLED[] = _T("Settings/PluginsEnabled");

// Startup options
const TCHAR OPT_SHOW_SELECT_FILES_AT_STARTUP[] = _T("Settings/ShowFileDialog");

// Font options
const TCHAR OPT_FONT_FILECMP_USECUSTOM[] = _T("Font/Specified");
const TCHAR OPT_FONT_DIRCMP_USECUSTOM[] = _T("FontDirCompare/Specified");

const TCHAR OPT_FONT_FILECMP_HEIGHT[] = _T("Font/Height");
const TCHAR OPT_FONT_FILECMP_WIDTH[] = _T("Font/Width");
const TCHAR OPT_FONT_FILECMP_ESCAPEMENT[] = _T("Font/Escapement");
const TCHAR OPT_FONT_FILECMP_ORIENTATION[] = _T("Font/Orientation");
const TCHAR OPT_FONT_FILECMP_WEIGHT[] = _T("Font/Weight");
const TCHAR OPT_FONT_FILECMP_ITALIC[] = _T("Font/Italic");
const TCHAR OPT_FONT_FILECMP_UNDERLINE[] = _T("Font/Underline");
const TCHAR OPT_FONT_FILECMP_STRIKEOUT[] = _T("Font/StrikeOut");
const TCHAR OPT_FONT_FILECMP_CHARSET[] = _T("Font/CharSet");
const TCHAR OPT_FONT_FILECMP_OUTPRECISION[] = _T("Font/OutPrecision");
const TCHAR OPT_FONT_FILECMP_CLIPPRECISION[] = _T("Font/ClipPrecision");
const TCHAR OPT_FONT_FILECMP_QUALITY[] = _T("Font/Quality");
const TCHAR OPT_FONT_FILECMP_PITCHANDFAMILY[] = _T("Font/PitchAndFamily");
const TCHAR OPT_FONT_FILECMP_FACENAME[] = _T("Font/FaceName");

const TCHAR OPT_FONT_DIRCMP_HEIGHT[] = _T("FontDirCompare/Height");
const TCHAR OPT_FONT_DIRCMP_WIDTH[] = _T("FontDirCompare/Width");
const TCHAR OPT_FONT_DIRCMP_ESCAPEMENT[] = _T("FontDirCompare/Escapement");
const TCHAR OPT_FONT_DIRCMP_ORIENTATION[] = _T("FontDirCompare/Orientation");
const TCHAR OPT_FONT_DIRCMP_WEIGHT[] = _T("FontDirCompare/Weight");
const TCHAR OPT_FONT_DIRCMP_ITALIC[] = _T("FontDirCompare/Italic");
const TCHAR OPT_FONT_DIRCMP_UNDERLINE[] = _T("FontDirCompare/Underline");
const TCHAR OPT_FONT_DIRCMP_STRIKEOUT[] = _T("FontDirCompare/StrikeOut");
const TCHAR OPT_FONT_DIRCMP_CHARSET[] = _T("FontDirCompare/CharSet");
const TCHAR OPT_FONT_DIRCMP_OUTPRECISION[] = _T("FontDirCompare/OutPrecision");
const TCHAR OPT_FONT_DIRCMP_CLIPPRECISION[] = _T("FontDirCompare/ClipPrecision");
const TCHAR OPT_FONT_DIRCMP_QUALITY[] = _T("FontDirCompare/Quality");
const TCHAR OPT_FONT_DIRCMP_PITCHANDFAMILY[] = _T("FontDirCompare/PitchAndFamily");
const TCHAR OPT_FONT_DIRCMP_FACENAME[] = _T("FontDirCompare/FaceName");

#endif // _OPTIONS_DEF_H_

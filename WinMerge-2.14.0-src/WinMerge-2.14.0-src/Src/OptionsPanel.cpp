/** 
 * @file  OptionsPanel.cpp
 *
 * @brief Implementation of OptionsPanel class.
 */
// ID line follows -- this is updated by SVN
// $Id: OptionsPanel.cpp 7501 2011-01-03 13:29:00Z gerundt $

#include "stdafx.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

/**
 * @brief Constructor.
 */
OptionsPanel::OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate)
 : CPropertyPage(nIDTemplate)
 , m_pOptionsMgr(optionsMgr)
{
}

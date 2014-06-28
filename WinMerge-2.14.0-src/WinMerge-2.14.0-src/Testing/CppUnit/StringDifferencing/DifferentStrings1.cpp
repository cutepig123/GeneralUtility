/** 
 * @file  DifferentStrings1.h
 *
 * @brief Implementation for simple different strings tests.
 */

#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "CompareOptions.h"
#include "stringdiffs.h"

#include "DifferentStrings1.h"

CPPUNIT_TEST_SUITE_REGISTRATION( DifferentStrings1 );

/** @brief Testcase initialization code. */
void DifferentStrings1::setUp()
{
	// Add possible initializations here
}

/** @brief Testcase cleanup code. */
void DifferentStrings1::tearDown()
{
	// Add possible cleanups here
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-char case difference correctly
 * with different compare options. In this test the difference is
 * the first char in word.
 */
void DifferentStrings1::CasesDiffer1()
{
	wdiffarray diffs;
	CString string1(_T("Test"));
	CString string2(_T("test"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-char case difference correctly
 * with different compare options. In this test the difference is
 * in the middle of word.
 */
void DifferentStrings1::CasesDiffer2()
{
	wdiffarray diffs;
	CString string1(_T("test"));
	CString string2(_T("teSt"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options.
 */
void DifferentStrings1::CasesDiffer3()
{
	wdiffarray diffs;
	CString string1(_T("test case"));
	CString string2(_T("teSt case"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options.
 */
void DifferentStrings1::CasesDiffer4()
{
	wdiffarray diffs;
	CString string1(_T("test case"));
	CString string2(_T("test cAse"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time with two words.
 */
void DifferentStrings1::CasesDiffer5()
{
	wdiffarray diffs;
	CString string1(_T("test case"));
	CString string2(_T("tEst cAse"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time with three words,
 * with one identical word between two different words.
 */
void DifferentStrings1::CasesDiffer6()
{
	wdiffarray diffs;
	CString string1(_T("test case string"));
	CString string2(_T("tEst case String"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
}

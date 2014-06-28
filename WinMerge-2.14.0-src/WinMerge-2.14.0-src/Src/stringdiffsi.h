/** 
 * @file  stringdiffsi.h
 *
 * @brief Declaration file for class stringdiffs
 *
 */
// ID line follows -- this is updated by SVN
// $Id: stringdiffsi.h 5459 2008-06-10 16:49:30Z kimmov $

#ifndef stringdiffsi_h_included
#define stringdiffsi_h_included

#include <vector>

struct wdiff;

/**
 * @brief Class to hold together data needed to implement sd_ComputeWordDiffs
 */
class stringdiffs
{
public:
	stringdiffs(const String & str1, const String & str2,
		bool case_sensitive, int whitespace, int breakType,
		std::vector<wdiff*> * pDiffs);

	~stringdiffs();

	void BuildWordDiffList();
	void PopulateDiffs();

// Implementation types
private:
	struct word {
		int start; // index of first character of word in original string
		int end;   // index of last character of word in original string
		int hash;
		word(int s=0, int e=0, int h=0) : start(s), end(e), hash(h) { }
		int length() const { return end+1-start; }
	};

// Implementation methods
private:

	void BuildWordsArray(const String & str, std::vector<word*> * words);
	bool findSync(int *w1, int *w2) const;
	int FindNextMatchInWords2(const word & needword1, int bw2) const;
	int FindNextMatchInWords1(const word & needword2, int bw1) const;

	int hash(const String & str, int begin, int end) const;
	bool AreWordsSame(const word & word1, const word & word2) const;
	bool caseMatch(TCHAR ch1, TCHAR ch2) const;

// Implementation data
private:
	const String & m_str1;
	const String & m_str2;
	bool m_case_sensitive;
	int m_whitespace;
	int m_breakType;
	std::vector<wdiff*> * m_pDiffs;
	std::vector<word*> m_words1;
	std::vector<word*> m_words2;
	std::vector<wdiff*> m_wdiffs;
};


#endif // stringdiffsi_h_included

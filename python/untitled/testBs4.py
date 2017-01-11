html_doc = """
<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0">
<channel>
<title>cnBeta.COM</title>
<link>http://www.cnbeta.com</link>
<pubDate>Sun, 08 Jan 2017 12:52:18 GMT</pubDate>
<language>zh-cn</language>
<copyright>@cnbeta.com</copyright>
<item>
	<title>T1</title>
	<link>http://www.cnbeta.com/articles/574785.htm</link>
	<pubDate>Sun, 08 Jan 2017 11:25:57 GMT</pubDate>
	<description>D1</description>
</item>
<item>
	<title>T2</title>
	<link>http://www.cnbeta.com/articles/574785.htm</link>
	<pubDate>Sun, 08 Jan 2017 11:25:57 GMT</pubDate>
	<description>D2</description>
</item>
</channel></rss>
"""

from bs4 import BeautifulSoup
soup = BeautifulSoup(html_doc)

#print(soup.prettify())

for link in soup.find_all('item/title'):
#    print(link.get('href'))
    print(link)
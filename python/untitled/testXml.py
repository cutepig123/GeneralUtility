import xml.etree.ElementTree as ET
import urllib2

text = '''
<div>
    <ul>
         <li class="item-0"><a href="link1.html">first item</a></li>
         <li class="item-1"><a href="link2.html">second item</a></li>
         <li class="item-inactive"><a href="link3.html">third item</a></li>
         <li class="item-1"><a href="link4.html">fourth item</a></li>
         <li class="item-0"><a href="link5.html">fifth item</a></li>
     </ul>
 </div>
'''
root = ET.fromstring(text)
for t in root.findall('.//li/a'):
    print t.attrib, t.text

def getData(url):
    try:
        response = urllib2.urlopen(url)
        html = response.read()
        return html
    except:
        return ''

def testUrl():
    url = 'http://rss.cnbeta.com/rss'
    html = getData(url)
    root = ET.fromstring(html)
    allItems = root.findall('.//item')
    i=0
    for t in allItems:
        link = t.find('link').text
        print i, '/', len(allItems), link
        if i>0:break
        data = getData(link)
        if len(data)>0:
            t.find('description').text = data
        i=i+1
    ET.ElementTree(root).write('output.xml')

testUrl()

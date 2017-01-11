from flask import Flask
from flask import Flask, jsonify
import urllib2

app = Flask(__name__)

@app.route("/")
def hello():
    return "Hello World!"

tasks = [
    {
        'id': 1,
        'title': u'Buy groceries',
        'description': u'Milk, Cheese, Pizza, Fruit, Tylenol',
        'done': False
    },
    {
        'id': 2,
        'title': u'Learn Python',
        'description': u'Need to find a good Python tutorial on the web',
        'done': False
    }
]

@app.route('/todo/api/v1.0/tasks', methods=['GET'])
def get_tasks():
    return jsonify({'tasks': tasks})

@app.route('/testurl', methods=['GET'])
def testUrl():
    response = urllib2.urlopen('http://python.org/')
    html = response.read()
    return html

@app.route('/testCnbeta', methods=['GET'])
def testUrl():
    url = 'http://rss.cnbeta.com/rss'
    response = urllib2.urlopen(url)
    html = response.read()
    p1=0
    p2=0
    while 1:
        p1=html.find('<link>', p2)
        p2 = html.find('</link>', p1+1)
        link=

    return html

if __name__ == "__main__":
    app.run()

#http://localhost:5000/todo/api/v1.0/tasks
#http://localhost:5000
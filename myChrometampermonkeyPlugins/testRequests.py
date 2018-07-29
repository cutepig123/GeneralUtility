import requests

def testGet():
	bitcoin_api_url = 'https://api.coinmarketcap.com/v1/ticker/bitcoin/'
	response = requests.get(bitcoin_api_url)
	response_json = response.json()
	print (response_json)

#curl -X POST -H "Content-Type: application/json" -d '{"value1":"1","value2":"2","value3":"3"}' https://maker.ifttt.com/trigger/bankmoney/with/key/feQcXd0QuePnJb23E97bv

ifttt_event_url='https://maker.ifttt.com/trigger/bankmoney/with/key/feQcXd0QuePnJb23E97bv'
data={"value1":"BankAccount: HSBC922 HkdBalance: 20 rmbBalance: 0 creditBalance: -100","value2":"ss2","value3":"3cc"}
print (requests.post(ifttt_event_url, json=data))


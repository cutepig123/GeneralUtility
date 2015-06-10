import socket
import sys,os
import pickle

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('localhost', 21567)
try:
	#connect server
	addr=[]
	ip_list=[]
	while len(ip_list)<2:
		# Send data
		message = 'register_my_ip'
		print >>sys.stderr, 'sending "%s"' % message
		sent = sock.sendto(pickle.dumps(message), server_address)

		# Receive response
		while 1:
			print >>sys.stderr, 'waiting to receive'
			data, server = sock.recvfrom(4096)
			print 'Recv', data
			if not server[1]==server_address[1]:
				continue
			else:
				x =pickle.loads(data)
				assert(len(x)==2,len(x))
				addr=x[0]
				ip_list =x[1]
				break
		print 'Recv:', addr, ip_list
		#print 'Press anykey to continue send msg'
		#os.system('pause')
	
	isEixt =0
	assert(len(addr)==2)
	other_ip =[]
	for i in ip_list:
		assert(len(i)==2)
		if not (i[0]==addr[0] and i[1]==addr[1]):
			other_ip =i
			break
	
	print 'Connect to peer', other_ip
	os.system('pause')
	sock.settimeout(1)
	
	while 1:
		opt =raw_input('send (0) or recv(1)?')
		if opt=='0':
			sent = sock.sendto('hello! this is %s'%pickle.dumps(addr), other_ip)
		else:
			try:
				data, server = sock.recvfrom(4096)
				print 'Recv',data, server
			except:
				print 'recv error'
		print 'Press anykey to continue'
		os.system('pause')
	
finally:
	print >>sys.stderr, 'closing socket'
	sock.close()
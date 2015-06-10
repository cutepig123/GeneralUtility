#http://www.wellho.net/resources/ex.php4?item=y303/udp_server.py
import socket
import pickle
 
# A UDP server

# Set up a UDP server
UDPSock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

# Listen on port 21567
# (to all IP addresses on this system)
listen_addr = ("",21567)
UDPSock.bind(listen_addr)

ip_list=[]
# Report on all data packets received and
# where they came from in each case (as this is
# UDP, each may be from a different source and it's
# up to the server to sort this out!)
while True:
		data,addr = UDPSock.recvfrom(1024)
		data_dec =pickle.loads(data)
		print 'Recv Msg:', data_dec,addr
		if data_dec=='register_my_ip':
			isEixt =0
			assert(len(addr)==2)
			for i in ip_list:
				assert(len(i)==2)
				if (i[0]==addr[0] and i[1]==addr[1]):
					isEixt=1
					break
			if not isEixt:	
				ip_list.append(addr)
			addrip_list_enc =pickle.dumps([addr, ip_list])
			sent = UDPSock.sendto(addrip_list_enc, addr)
			

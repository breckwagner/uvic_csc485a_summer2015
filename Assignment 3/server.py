################################################################################
#	CSC 485A - Assignment 3
#	@author    Richard B. Wagner
#	@author    MacKay McGillivray
#	@author    Jon G. Bowen
#	@version   0.1
#	@date      2015-07-10
#	@copyright GNU Public License.
################################################################################

# Dependencies
################################################################################

try:
	import threading as _threading
except ImportError:
	import dummy_threading as _threading
import socketserver
import time
from urllib.parse import urlparse, parse_qs
import queue


# Global Variables
################################################################################

# global thread safe queue to pipe requests into the MAPE-K loop
PORT = 8080;
request_queue = [];
job_queue = {}

# Utility Functions
################################################################################

##
# @param name The command/job to execute (e.g. 'find X -name "Y" > output.txt &')
# @param id Unique number to give job so user can get job status
# @param runtime The maximum amount of time the command can be allowed to run
# @param priority The priority the user would like the command to have
#def addJob(name, id, runtime, priority):
	#job_queue.append(id:{'name':name, 'id':id, 'runtime':runtime, 'priority':priority})

##
# @param name The command/job to execute (e.g. 'find X -name "Y" > output.txt &')
# @param id Unique number to give job so user can get job status
# @param runtime The maximum amount of time the command can be allowed to run
# @param priority The priority the user would like the command to have
#def modifyJob(name, id, runtime, priority):
#	return {'name':name, 'id':id, 'runtime':runtime, 'priority':priority}

##
# @param id Unique number to give job so user can get job status
#def removeJob(id):
#	return {'name':name, 'id':id, 'runtime':runtime, 'priority':priority}

def dispatcher(request, args):
	if(request=='addJob'):
		job_queue[str(args['id'][0])] = {'name':args['name'][0], 'id':args['id'][0], 'runtime':args['runtime'][0], 'priority':args['priority'][0], 'start':time.time()};
	elif(request=='modifyJob'):
		modifyJob(name, id, runtime, priority)
	elif(request=='removeJob'):
		removeJob(id)

# Classes
################################################################################

class MyTCPHandler(socketserver.BaseRequestHandler):
	
	def handle(self):
		while(True):
			self.data = self.request.recv(1024)
			if not self.data:
				print('DISCONNECTED')
				break
			
			# Do data decode and format
			tmp_data = self.data.decode('utf-8').split('\r\n')
			_data = {}
			for i in range(0, len(tmp_data)):
				tmp = tmp_data[i].split();
				if(len(tmp)>1):
					_data[tmp[0]] = tmp[1:]
			url = urlparse('http://' + _data['Host:'][0] + _data['GET'][0])
			
			
			if(len(url[2])>1 and url[2][1:] == 'status'):
				content = "<html><head><title>Status</title></head><body><h1>Status</h1>"
				for i in range(0, len(request_queue)):
					content += str(request_queue[i]) + "<br \>"
				
				content += "</body></html>"
				
			elif(len(url[2])>1 and url[2][1:] != 'favicon.ico' ):
				request_queue.append([url[2][1:], parse_qs(url[4],keep_blank_values=True)])
				content ="<html><body><h1>Your Request Has Been Recieved</h1></body></html>"
			
			
			
			header = "HTTP/1.1 200 OK\nContent-length: " + str(len(content)) + "\nContent-Type: text/html\n\n"
			
			self.request.sendall(str(header+content).encode('utf-8'))
			
			
# Main
################################################################################

if __name__ == "__main__":
	server = socketserver.TCPServer(('localhost', PORT), MyTCPHandler)
	
	# put actual listener on different thread for asynchronous job retrieval
	_threading.Thread(target=server.serve_forever).start()
	
	# MAPE-K loop
	while(True):
		
		# pause loop
		time.sleep(0.5)
		
		# Monitor
		########################################################################
		
		# if request_queue is not empty
		if(request_queue):
			# fetch request from queue
			request = request_queue.pop(0)
			print(request)
		else:
			request = None
		
		# Analyze/Plan
		########################################################################
		
		# if a job has gone past its max runtime, scheduale it for removal
		r_queue = []
		for i in job_queue:
			if(float(job_queue[i]['runtime']) > time.time() - job_queue[i]['start']):
				r_queue.append(job_queue[i])
				
		# if there is "room" in the execution list
		room = (len(job_queue)<=3)
			
		
		
		# Execute
		########################################################################
		
		# for
		while r_queue:
			del job_queue[r_queue.pop()['id']]
		
		if(request and room):
			dispatcher(request[0], request[1])

# Output targets are binary files 'Client' and 'Server'
Client Server: Client.obj Server.obj
	g++ Client.obj -o Client -pthread
	g++ Server.obj -o Server -pthread

# Intermediate object files.
Client.obj: Client.cpp
	g++ -c Client.cpp -o Client.obj -pthread
Server.obj: Server.cpp
	g++ -c Server.cpp -o Server.obj -pthread

runS: Server
	./Server $(ARG)
runC: Client
	./Client $(ARG)

# Cleanup temporary files.
clean:
	rm -rf *.obj Client Server

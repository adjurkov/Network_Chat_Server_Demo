INFO-6016 Network Programming
Server/Client Chat
Name: Ana Djurkovic 
StudentID: 1036572

(Worked on project solo)

-----------------------------------------------------------------------------------------
-                               ABOUT THIS PROGRAM                                      -
-----------------------------------------------------------------------------------------

- Build project: build Debug x64

- Run project:

-----------------------------------------------------------------------------------------
-                                 MARKING BREAKDOWN                                     -
-----------------------------------------------------------------------------------------

SERVER:

[5 marks]: Handle connections and messages without blocking.

- kbhit was used for asynchronous user input
- 

[10 marks]: Deserialize messages properly.

- 

[1 mark] Ability for connections to join a room.

-

[1 mark] Ability for connections to leave a room.

 - 

[2 marks] Ability for connections to send a message to a room and broadcast to peers.

-

[1 mark] Ability for connections to join multiple rooms.

-

[1 mark] When a connection joins a room, the server should broadcast: "[name] has joined the room."

-

[1 mark] When a connection leaves a room, the server should broadcast "[name] has left the room."

-	

-----------------------------------------------------------------------------------------

Client:

[1 mark] Able to join a room.

-

[1 mark] Able to leave a room.

-

[1 mark] Able to send a message to a room.

-

[1 mark] Able to receive messages from a room.

-

-----------------------------------------------------------------------------------------

Buffer:

[1 mark] Initialized with a size of N.

-

[2 marks] Should grow when serializing past the write index.

-

[2 marks] Serialize, and deserialize int.

-

[2 marks] Serialize, and deserialize short.

-

[2 marks] Serialize, and deserialize string.

-

-----------------------------------------------------------------------------------------

Protocol:

[1 mark] Must be binary.

- 

[2 marks] Must use length prefix message framing.

-

[1 mark] Big Endian must be used for the protocol.

-
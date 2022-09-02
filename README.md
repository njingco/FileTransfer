# FileTransfer

# Build

Build the application using the makefile.

1. `cd src/`
2. `make`

# Run Application

## Server

`./server`

## Client

`./client <ip> <port>`

# Commands

After running client it will prompt the user to enter a command.

- GET: Request File from Server
- SND: Send File to Server
- EXT: Close application

## GET

For a client to request a file from the Server, use the GET command. It will ask the user to enter the file name.

If the file is found in the server it will send it to the client and the file will be saved in the `/files` folder.

If the file is not in the server it will send an error message.

## SND

For a client to send a file from the Server, use the SND command. It will ask the user to enter the file name to send.

If the file is found in `/files` folder, it will send it to the server and save it in its `/files` folder.

If the file is not in the `/files` folder it will send an error message.

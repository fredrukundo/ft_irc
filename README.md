# 🖥️ IRC Server from Scratch – Group Project 🚀

## Overview
This project is an IRC (Internet Relay Chat) server developed from the ground up using **C++**. It simulates a fully functioning IRC system where users can connect, join channels, communicate, and manage access privileges.

## Project Contributions
This was a group project where our efforts were split into two major sections:

### 🔧 **My Focus – Commands and Channel Management:**
- **Channel Creation**: Implemented functionality to create channels and manage client memberships.
- **Commands:**
  - `KICK` – Remove clients from a channel.
  - `INVITE` – Send invites to clients to join a specific channel.
  - `TOPIC` – Set or view the current topic of the channel.
  - `MODE` – Adjust the mode of channels with specific options:
    - `i` – Toggle invite-only access.
    - `t` – Restrict topic changes to operators.
    - `k` – Manage channel keys (passwords).
    - `o` – Assign or remove operator privileges.
    - `l` – Set user limits for the channel.
  - **Core IRC Commands**:
    - `JOIN` – Join a specific channel.
    - `PART` – Leave a channel.
    - `PRIVMSG` – Send a private message to a client or a channel.
    - `QUIT` – Disconnect from the server.
    - **User Authentication**:
      - `PASS` – Provide a password to connect to the server.
      - `NICK` – Set or change the nickname of a client.
      - `USER` – Register a username and real name for the connection.

### 🔌 **Teammate’s Focus – Server & Networking:**
- Built the underlying server architecture.
- Managed socket creation, client connection handling, and communication protocols.

## Key Features
- Multi-channel support
- Client-to-client messaging
- Real-time channel and user management
- Full implementation of IRC commands including user authentication

## Technologies Used
- **C++**
- **Socket programming**

## Getting Started
### How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/irc-server-project.git
2. Navigate to the project directory:
   ```bash
   cd ft_irc
3. Compile and run the server:
     ```bash
     make
     ./ircserv port pass
## How to Connect
You can use any IRC client (such as LimeChat) to connect to the server:

1. Open your IRC client.
2. Enter the server address and port.
## Contributing
If you'd like to contribute or suggest improvements, feel free to submit a pull request or open an issue.

## Acknowledgements
This project was a collaboration between:

[FREDRUKUNDO's GitHub] (https://github.com/fredrukundo)  
[FRUIT-DU-DEMON's GitHub]

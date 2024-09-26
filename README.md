# WhisperNet
Stay Hidden, Stay Secure

**WhisperNet** is a decentralized, privacy-focused communication platform that allows users to securely connect, chat, and share files without relying on centralized servers. WhisperNet combines **peer-to-peer networking**, **Decentralized Identifiers (DIDs)**, and **end-to-end encryption** to ensure users can interact anonymously while maintaining full control over their data and connections.

## Key Features

- **Decentralized Authentication**:
  - Users create unique usernames linked to Decentralized Identifiers (DIDs), backed by cryptographic public-private key pairs.
  - Your identity is verifiable across the network without revealing sensitive data.

- **Peer-to-Peer Networking**:
  - WhisperNet uses advanced P2P protocols to enable direct, encrypted communication between peers.
  - No central servers mean more privacy and no single point of failure.

- **DHT-Based Username Lookup and Peer Discovery**:
  - A **Distributed Hash Table (DHT)** is used for decentralized username lookup **and peer discovery**.
  - Find and connect to other users by their unique usernames in a fully decentralized manner.
  - Dynamically discover peers in the network without relying on hardcoded or preconfigured peer lists.

- **Friend Request and Connection Approval**:
  - WhisperNet features a **social-network-like system** where users can send and receive friend requests.
  - A user must **approve** or **deny** the connection request before a P2P link is established, ensuring control over who can communicate with you.

- **Anonymous Chatrooms**:
  - Create and join anonymous chatrooms with customizable privacy settings.
  - Chat in groups without revealing your real identity.

- **Secure File Sharing**:
  - Upload and share files securely using decentralized storage solutions like **IPFS**.
  - Files are encrypted before sharing to ensure only intended recipients can access them.

- **End-to-End Encryption**:
  - All messages, file transfers, and connections are encrypted using modern cryptographic algorithms (OpenSSL / libsodium).
  - Your data stays private, and no intermediaries can read or intercept your communications.

## Future Features

- **Decentralized Storage**: Further integration with decentralized file storage systems (e.g., IPFS) to store and retrieve shared data more efficiently.
- **Group Chat and Channels**: Enhanced support for chatrooms and group-based communications, allowing for persistent, anonymous conversations.

## Technologies Used

- **C++**: Core language for backend logic and P2P networking.
- **OpenSSL / libsodium**: Provides encryption and cryptographic functionality for secure communication.

---

## Getting Started

### 1. Clone the repository:

```bash
git clone https://github.com/mush1e/WhisperNet.git
cd WhisperNet
```

### 2. Build the application:

Make sure all dependencies (like OpenSSL and IPFS) are installed on your system.

```bash
make
```

### 3. Run the application:

```bash
./whispernet
```

### 4. Setting up your identity:

Upon the first run, WhisperNet will generate a unique key pair and DID for your node. You'll then be able to choose a **unique username**, which will be registered via the DHT. If the username is already taken, you will be prompted to select a different one.

### 5. Connecting to Peers:

- **Discover Peers**:
  - Use the DHT-based peer discovery to find other users in the network.
  - Search for users by their username without any central server.

- **Send Friend Requests**:
  - Initiate a connection request to another user, who can then accept or deny it.

- **Start Messaging**:
  - Once a connection is established, you can start messaging or sharing files with the connected user.

---

## Contributing

We welcome contributions from the community! Feel free to fork the repository, submit pull requests, or open issues for any bugs or feature suggestions.

To contribute:

1. Fork the repository.
2. Create a feature branch.
3. Commit your changes.
4. Submit a pull request.

Please ensure that your code follows the project’s coding standards and is thoroughly tested.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---


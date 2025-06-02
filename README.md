# WhisperNet (currently abandoned) 
**Stay Hidden, Stay Secure**
*A decentralized, privacy-focused social network built from scratch in Go.*

## Table of Contents

1. [Overview](#overview)
2. [Key Features](#key-features)
3. [Planned Roadmap](#planned-roadmap)
4. [Architecture](#architecture)
   - [Peer-to-Peer Layer](#peer-to-peer-layer)
   - [Decentralized Identifiers (DIDs)](#decentralized-identifiers-dids)
   - [DHT-based Discovery](#dht-based-discovery)
   - [Secure Communications](#secure-communications)
   - [Social Media Layer](#social-media-layer)
5. [Getting Started](#getting-started)
   - [Prerequisites](#prerequisites)
   - [Build and Run](#build-and-run)
   - [Initial Setup](#initial-setup)
6. [Usage](#usage)
   - [Creating a Profile](#creating-a-profile)
   - [Finding and Adding Friends](#finding-and-adding-friends)
   - [Posting to Your Feed](#posting-to-your-feed)
   - [Anonymous Chatrooms](#anonymous-chatrooms)
   - [File Sharing](#file-sharing)
7. [Project Scope & Goals](#project-scope--goals)
8. [Contributing](#contributing)
9. [License](#license)

---

## Overview

**WhisperNet** is an **experimental** decentralized social network with built-in anonymity, E2E encryption, and a permissioned “friend request” system. Its primary goal is to **learn** the internals of peer-to-peer protocols, decentralized identity, cryptography, and distributed storage—**all implemented in Go from scratch** where possible.

This is *not* intended to be a production-ready system—rather, a playground for exploring how privacy-preserving, censorship-resistant social media can work when there are **no central servers** and **no single point of failure**.

---

## Key Features

- **Full Decentralization**
  All network traffic is peer-to-peer. WhisperNet does not depend on central servers or cloud APIs.

- **DID-Based Usernames**
  Each user has a unique Decentralized Identifier (DID) backed by a cryptographic key pair. Usernames are mapped to these DIDs.

- **DHT-Driven Peer Discovery**
  No central directories. Peers discover one another and exchange data via a **Distributed Hash Table**.

- **Friend Requests & Connection Approval**
  Users must *approve* connection requests before peers can exchange private messages or see each other’s “friends-only” content.

- **End-to-End Encryption**
  All messages are encrypted so that only sender and recipient can view them, leveraging Go’s standard crypto libraries (e.g., `crypto/ecdsa`, `crypto/ed25519`, or `golang.org/x/crypto`).

- **Social Feed & Ephemeral Posts** *(in progress)*
  - Post text or media to your personal feed.
  - Choose visibility: public, friends-only, or ephemeral (auto-expire).
  - “Likes” and “comments” stored in a decentralized manner, referencing your DID.

- **Anonymous Chatrooms**
  Create group chats with ephemeral or persistent content. No real-world identities required.

- **Secure File Sharing**
  Leveraging a combination of custom P2P transfers and optional integration with IPFS. Files are encrypted before leaving your machine.

---

## Planned Roadmap

1. **Core P2P Protocol**
   - Build a custom handshake, messaging, and encryption layer in Go.
   - Implement NAT traversal (if possible) for direct peer connections.

2. **Improved DHT & Gossip**
   - Enhance lookup performance.
   - Implement ephemeral or partial indexing for ephemeral posts.

3. **Social Layer**
   - Expand feed functionality with “likes,” “comments,” and “tags.”
   - Add user-defined groups or “communities” for shared posting.

4. **Privacy & UX Enhancements**
   - Onion routing (Tor/I2P) integration for stronger anonymity.
   - Basic spam prevention, blocking, and local content moderation.

5. **Refinements & Testing**
   - Unit tests across all core modules.
   - Performance benchmarks for large user counts.

---

## Architecture

### Peer-to-Peer Layer
- Built in pure Go for a deep understanding of networking.
- Uses **TCP** (and possibly **UDP** for certain features) with custom message framing.
- Employs a custom handshake protocol to establish E2E encryption and verify DIDs.

### Decentralized Identifiers (DIDs)
- Each user generates a key pair (e.g., **Ed25519** or **ECDSA**).
- The public key is hashed or encoded in a DID string (e.g., `did:whispernet:<hash>`).
- Users can rotate or revoke keys, but you need the old key to sign the event that points to the new key.

### DHT-based Discovery
- A Kademlia-like DHT implemented in Go.
- Stores mappings of “username -> DID,” “DID -> IP address & port,” and ephemeral references to posted content (e.g., “hashtags” or “topics”).

### Secure Communications
- **Initial Handshake**: Diffie-Hellman or ECDH to derive a session key.
- **Message Encryption**: AES-256-GCM or ChaCha20-Poly1305 for symmetric encryption once a session key is established.
- **Signatures**: All messages optionally signed by your private key, so recipients can authenticate the sender.

### Social Media Layer
- **Feeds**: A list of content objects (posts, images, references) stored in the network, each identified by a content hash.
- **Permissions**:
  - Public posts are unencrypted or encrypted with a well-known key.
  - Friends-only posts encrypted with a random symmetric key, sealed by each friend’s public key.
  - Ephemeral posts are the same but come with an expiration signal that can be honored by the network and/or your own node’s GC.

---

## Getting Started

### Prerequisites
1. **Go** (1.19+ recommended).
2. **OpenSSL** or **libsodium** (optional; some bindings may be used for cryptographic routines).
3. **Git** for version control.

*(IPFS is optional if you choose to integrate it for file storage.)*

### Build and Run

```bash
git clone https://github.com/mush1e/WhisperNet.git
cd WhisperNet

# Build the project
go build -o whispernet cmd/whispernet/main.go

# Run the node
./whispernet
```

### Initial Setup
- On first run, WhisperNet generates a new key pair and a corresponding DID.
- You can choose a username, which the node will attempt to register via the DHT.
- If the username is taken, you’ll be prompted to pick another.

---

## Usage

### Creating a Profile
- Your “profile” is basically a small JSON (or CBOR) structure stored in the network (encrypted or not, your choice).
- By default, it might hold a display name, short bio, or a small avatar.

### Finding and Adding Friends
1. **Search** by username or DID via the DHT.
2. **Send Friend Request**: The target must accept before you can see each other’s feeds or chat privately.
3. **Approve/Deny**: The target user can approve or deny your request.

### Posting to Your Feed
- Create a new post:
  ```bash
  whispernet post --visibility friends --message "Hello decentralized world!"
  ```
- This post is encrypted (if not public) and stored in the network under a content hash.
- Peers who follow you can retrieve it via that hash once they see the reference in your “feed index.”

### Anonymous Chatrooms
- Start an ephemeral chatroom:
  ```bash
  whispernet chat --create --anonymous
  ```
- Share the invite link (an ephemeral token or room ID).
- All messages in that room are end-to-end encrypted. *Optional: room-based ephemeral timer for messages.*

### File Sharing
- Encrypt your file locally, then distribute via your P2P channel or store the chunks on IPFS.
- Recipients can retrieve the file using the provided content hash or direct link, and decrypt using the shared key.

---

## Project Scope & Goals

1. **Learn Go Thoroughly**
   - Low-level networking (net package), concurrency (goroutines, channels), and standard library crypto.

2. **Understand Decentralized Systems**
   - DHTs (Kademlia), advanced peer discovery, self-sovereign identity (DIDs), content-addressed storage.

3. **Implement a P2P Social Network**
   - Explore feed mechanics, “likes,” “comments,” ephemeral content, and fine-grained access control.
   - Keep everything fully decentralized and privacy-preserving.

4. **Experiment & Iterate**
   - This code is a sandbox: break things, refactor, optimize, and document your learning.
   - Nothing is set in stone—contributions and alternative protocols are welcome!

---

## Contributing

**WhisperNet** is a personal learning project with an open invitation for collaborators interested in decentralized tech, security, and Go. Here’s how you can help:

1. **Fork and Clone** the Repository.
2. **Create a Feature Branch** for your contribution.
3. **Write Tests** where possible (unit tests, integration tests, fuzz tests!).
4. **Submit a Pull Request** with a clear explanation of what your changes do.

### Ideas for Contributions
- Improve the DHT implementation (e.g., better routing table logic).
- Add new cryptographic ciphers or handshake variants.
- Build out the social features (e.g., “notifications,” user blocklists, group moderation).
- Performance tuning on concurrency or NAT traversal.

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

**Happy Hacking!**
*Stay Hidden, Stay Secure.*

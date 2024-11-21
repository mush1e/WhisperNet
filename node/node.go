package node

import (
	"fmt"
	"sync"
)

type Node struct {
	ID      string
	Peers   map[string]string
	Storage map[string]string
	mtx     sync.Mutex
}

func InitializeNode(id string) *Node {
	return &Node{
		ID:      id,
		Peers:   make(map[string]string),
		Storage: make(map[string]string),
	}
}

func (n *Node) AddPeer(peerID, addr string) {
	n.mtx.Lock()
	defer n.mtx.Unlock()
	n.Peers[peerID] = addr
}

func (n *Node) Store(key, value string) error {
	n.mtx.Lock()
	defer n.mtx.Unlock()

	if _, exists := n.Storage[key]; exists {
		return fmt.Errorf("key %q already exists in storage", key)
	}

	n.Storage[key] = value
	return nil
}

func (n *Node) Reterieve(key string) (value string) {
	n.mtx.Lock()
	defer n.mtx.Unlock()
	value = n.Storage[key]
	return
}

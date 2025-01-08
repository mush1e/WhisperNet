package core

import (
	"crypto/ed25519"
	"crypto/rand"
	"encoding/hex"
	"fmt"
)

type KeyPair struct {
	PrivateKey ed25519.PrivateKey
	PublicKey  ed25519.PublicKey
}

type DID struct {
	Method string
	ID     string
}

func GenerateKeyPair() (*KeyPair, error) {
	pub, priv, err := ed25519.GenerateKey(rand.Reader)

	if err != nil {
		return nil, err
	}

	return &KeyPair{
		PrivateKey: priv,
		PublicKey:  pub,
	}, nil
}

func CreateDID(pub ed25519.PublicKey) DID {
	encoded := hex.EncodeToString(pub)
	return DID{
		Method: "whispernet",
		ID:     encoded,
	}
}

func FormatDID(d DID) string {
	return fmt.Sprintf("did:%s:%s", d.Method, d.ID)
}

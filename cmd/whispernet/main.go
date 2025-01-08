package main

import (
	"fmt"

	"github.com/whispernet/internal/core"
)

func main() {
	keyPair, err := core.GenerateKeyPair()

	if err != nil {
		panic(err)
	}

	did := core.CreateDID(keyPair.PublicKey)
	didString := core.FormatDID(did)

	fmt.Println("Welcome to WhisperNet (alpha)!")
	fmt.Printf("Your DID: %s\n", didString)
	fmt.Printf("Public Key (hex): %x\n", keyPair.PublicKey)
	fmt.Println("Your keys are currently in memory only. Next step: store them securely.")

}

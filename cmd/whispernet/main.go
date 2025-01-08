package main

import (
	"fmt"
	"log"
	"os"

	"github.com/whispernet/internal/core"
)

func main() {
	configPath := core.DefaultConfigPath()
	cfg, err := core.LoadConfig(configPath)
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("No config found, generating new key pair")
			kp, err := core.GenerateKeyPair()
			if err != nil {
				log.Fatalf("failed to generate key pair: %v", err)
			}

			cfg = core.ConfigFromKeyPair(kp)
			if err := core.SaveConfig(configPath, cfg); err != nil {
				log.Fatalf("failed to save config: %v\n", err)
			}
			fmt.Printf("new config saved to: %s\n", configPath)
		} else {
			log.Fatalf("could not load config: %v\n", err)
		}
	} else {
		fmt.Printf("loaded config from: %s\n", configPath)
	}

	keyPair, err := core.KeyPairFromConfig(cfg)
	if err != nil {
		log.Fatalf("Failed to parse keys from config: %v\n", err)
	}

	did := core.CreateDID(keyPair.PublicKey)
	didString := core.FormatDID(did)
	fmt.Println("Welcome to WhisperNet (alpha)!")
	fmt.Printf("Your DID: %s\n", didString)
	fmt.Printf("Public Key (hex): %x\n", keyPair.PublicKey)
}

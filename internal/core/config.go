package core

import (
	"crypto/ed25519"
	"encoding/hex"
	"encoding/json"
	"errors"
	"os"
	"path/filepath"
)

const defaultConfigFile = "whispernet_config.json"

type Config struct {
	PrivateKeyHex string `json:"private_key"`
	PublicKeyHex  string `json:"public_key"`
}

func LoadConfig(configPath string) (*Config, error) {
	data, err := os.ReadFile(configPath)
	if err != nil {
		return nil, err
	}

	var cfg Config
	if err := json.Unmarshal(data, &cfg); err != nil {
		return nil, errors.New("corrupt config: missing keys")
	}

	return &cfg, nil
}

func SaveConfig(configPath string, cfg *Config) error {
	data, err := json.MarshalIndent(cfg, "", " ")
	if err != nil {
		return err
	}

	if err := os.MkdirAll(filepath.Dir(configPath), 0o700); err != nil {
		return err
	}
	return os.WriteFile(configPath, data, 0o600)
}

func KeyPairFromConfig(cfg *Config) (*KeyPair, error) {
	privBytes, err := hex.DecodeString(cfg.PrivateKeyHex)
	if err != nil {
		return nil, err
	}

	pubBytes, err := hex.DecodeString(cfg.PublicKeyHex)
	if err != nil {
		return nil, err
	}

	publicKey := ed25519.PublicKey(pubBytes)
	privateKey := ed25519.PrivateKey(privBytes)

	return &KeyPair{
		PrivateKey: privateKey,
		PublicKey:  publicKey,
	}, nil
}

func ConfigFromKeyPair(kp *KeyPair) *Config {
	return &Config{
		PrivateKeyHex: hex.EncodeToString(kp.PrivateKey),
		PublicKeyHex:  hex.EncodeToString(kp.PublicKey),
	}
}

func DefaultConfigPath() string {
	home, err := os.UserHomeDir()
	if err != nil {
		return defaultConfigFile
	}
	return filepath.Join(home, ".whispernet", defaultConfigFile)
}

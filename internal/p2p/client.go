package p2p

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
)

func ConnectToServer(host string, port int, message string) error {
	addr := fmt.Sprintf("%s:%d", host, port)
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return fmt.Errorf("failed to connect to %s: %v", addr, err)
	}
	defer conn.Close()
	_, err = conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("failed to write to server: %w", err)
	}

	reader := bufio.NewReader(conn)
	response, err := reader.ReadString('\n')
	if err != nil && err != io.EOF {
		return fmt.Errorf("failed to read server response: %w", err)
	}
	log.Printf("Server responded with: %s", response)
	return nil
}

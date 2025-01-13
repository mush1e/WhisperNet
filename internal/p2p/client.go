package p2p

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

func startChat(conn net.Conn) {
	go func() {
		reader := bufio.NewReader(conn)
		for {
			line, err := reader.ReadString('\n')
			if err != nil {
				fmt.Println("Peer disconnected.")
				return
			}
			fmt.Printf("Peer: %s", line)
		}
	}()
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "exit" {
			fmt.Println("Exiting chat session.")
			conn.Close()
			return
		}
		_, err := conn.Write([]byte(line + "\n"))
		if err != nil {
			fmt.Println("Error writing to peer:", err)
			return
		}
	}
}

func Connect(addr string) error {
	addr = strings.TrimSpace(addr)
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return err
	}
	startChat(conn)
	return nil
}

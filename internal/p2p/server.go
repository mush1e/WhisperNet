package p2p

import (
	"bufio"
	"fmt"
	"log"
	"net"
)

func StartServer(port int) error {
	addr := fmt.Sprintf(":%d", port)
	listener, err := net.Listen("tcp", addr)
	if err != nil {
		return fmt.Errorf("failed to start server : %v", err)
	}
	log.Printf("server listening on %s\n", addr)

	for {
		conn, err := listener.Accept()

		if err != nil {
			log.Printf("failed to accept connection : %v\n", err)
			continue
		}
		handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()
	defer log.Printf("Connection from %s closed.\n", conn.RemoteAddr())
	log.Printf("new client connected from %v\n", conn.RemoteAddr())
	reader := bufio.NewReader(conn)
	line, err := reader.ReadString('\n')

	if err != nil {
		log.Printf("read error : %v\n", err)
		return
	}

	if line != "" {
		log.Printf("Recieved from client : %q\n", line)
	}

	response := "hello from the WhisperNet Server!"
	if _, err := conn.Write([]byte(response)); err != nil {
		log.Printf("write error : %q\n", err)
	}
}

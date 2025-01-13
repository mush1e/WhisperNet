package p2p

import (
	"fmt"
	"log"
	"net"
	"strings"
)

var IsListening bool

func Listen(port int) (net.Listener, error) {
	addr := fmt.Sprintf(":%d", port)
	listener, err := net.Listen("tcp", addr)

	if err != nil {
		IsListening = false
		return nil, err
	}

	log.Printf("server listening on %q", addr)
	IsListening = true

	go func() {
		for {
			conn, err := listener.Accept()
			if err != nil {
				if strings.Contains(err.Error(), "use of closed network connection") {
					log.Println("Listener closed, stopping accept loop.")
					return
				}
				log.Printf("failed to accept connection : %v\n", err)
				continue
			}
			go startChat(conn)
		}
	}()

	return listener, nil
}

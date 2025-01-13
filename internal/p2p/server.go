package p2p

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
)

var IsListening bool
var ConnChan = make(chan net.Conn)

func RecvChat(conn net.Conn) error {
	reader := bufio.NewReader(os.Stdin)
	fmt.Printf("\n recieving chat inv from %q accept? (y/n)\n", conn.RemoteAddr().String())
	line, err := reader.ReadString('\n')
	if err != nil {
		return err
	}
	line = strings.TrimSpace(line)
	switch line {
	case "y":
		startChat(conn)
	case "n":
		conn.Write([]byte("chat request denied from " + conn.LocalAddr().String() + "\n"))
		return nil
	}
	return nil
}

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
			ConnChan <- conn
		}
	}()

	return listener, nil
}

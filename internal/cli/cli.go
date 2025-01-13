package cli

import (
	"bufio"
	"errors"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"

	"github.com/whispernet/internal/p2p"
)

var Port int
var listener net.Listener

func configurePort() (err error) {
	reader := bufio.NewReader(os.Stdin)
	fmt.Printf("enter port : ")
	portStr, _ := reader.ReadString('\n')
	portStr = strings.TrimSpace(portStr)
	Port, err = strconv.Atoi(portStr)

	if err != nil {
		return errors.New("invalid port input")
	}

	if Port < 1 || Port > 65535 {
		return errors.New("port out of range [1 <= port <= 65535]")
	}

	return nil
}

func StartInteractiveCLI() {
	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Println()
		fmt.Println("WhisperNet Menu:")
		fmt.Println("[1] Configure port")
		fmt.Println("[2] Join network")
		fmt.Println("[3] Start chat session")
		fmt.Println("[q] Quit")
		fmt.Print("Choose an option: ")

		choice, _ := reader.ReadString('\n')
		choice = strings.TrimSpace(choice)

		switch choice {

		case "1":
			if err := configurePort(); err != nil {
				fmt.Printf("invalid port : %q\n", err)
				continue
			}

		case "2":
			if !p2p.IsListening {
				ln, err := p2p.Listen(Port)
				if err != nil {
					log.Fatalf("could not begin listening on port %v : %q", Port, err)
				}
				listener = ln
			} else {
				fmt.Println("you are already listening for connections on port : ", Port)
			}

		case "q":
			fmt.Println("gracefully shutting down whispernet!")
			if listener != nil {
				log.Println("terminating TCP listener")
				listener.Close()
			}
			return
		}
	}
}

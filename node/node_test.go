package node

import (
	"reflect"
	"testing"
)

func TestAddPeer(t *testing.T) {
	t.Run("testing add peer", func(t *testing.T) {
		node := InitializeNode("Mu5h1e")
		node.AddPeer("Peer1", "Home")
		want := map[string]string{"Peer1": "Home"}
		got := node.Peers
		if !reflect.DeepEqual(got, want) {
			t.Errorf("Expected %v but got %v", want, got)
		}
	})
}

func TestStore(t *testing.T) {
	t.Run("testing store data -basic", func(t *testing.T) {
		node := InitializeNode("")
		err := node.Store("Pet", "Dog")
		want := "Dog"
		got := node.Storage["Pet"]
		assertEqual(t, got, want)
		assertError(t, err, nil)
	})

	t.Run("testing store data -overwrite", func(t *testing.T) {
		node := InitializeNode("")
		node.Storage["Pet"] = "Dog"
		err := node.Store("Pet", "Cat")
		got := node.Storage["Pet"]
		want := "Dog"
		assertEqual(t, got, want)
		assertError(t, err, ErrPeerAlreadyExists)
	})
}

func assertEqual(t testing.TB, got, want string) {
	t.Helper()
	if got != want {
		t.Errorf("expected %v but got %v", want, got)
	}
}

func assertError(t testing.TB, got, want error) {
	t.Helper()
	if got != want {
		t.Errorf("expected error: %v | recieved error: %v", got, want)
	}
}

/*
	TODO : Work on more tests for storage functions, check obsidian notes for error comparisions and stuff
		 - Incrementally develop whispernet to be well tested and stuff
		 - Study concurrency in go lang and keep up the progress with the book
*/

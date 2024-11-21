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
		node.Store("Pet", "Dog")
		want := "Dog"
		got := node.Storage["Pet"]
		assertEqual(t, got, want)
	})
}

func assertEqual(t testing.TB, got, want string) {
	t.Helper()
	if got != want {
		t.Errorf("expected %v but got %v", want, got)
	}
}

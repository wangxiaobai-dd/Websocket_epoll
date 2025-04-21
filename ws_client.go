package main

import (
	"fmt"
	"log"
	"net/url"
	"time"

	"github.com/gorilla/websocket"
)

func main() {
	// 解析 WebSocket 服务器地址
	u := url.URL{Scheme: "ws", Host: "122.51.106.92:8500", Path: "/ws"}
	fmt.Printf("connecting to %s\n", u.String())

	// 连接到 WebSocket 服务器
	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Fatal("dial:", err)
	}
	defer c.Close()

	done := make(chan struct{})

	// 启动一个 goroutine 来接收服务器发送的消息
	go func() {
		defer close(done)
		for {
			_, message, err := c.ReadMessage()
			if err != nil {
				log.Println("read:", err)
				return
			}
			log.Printf("recv: %d", len(message))

			//log.Printf("recv: %s", message)
		}
	}()

	// 向服务器发送消息
	t := time.NewTicker(time.Second)
	defer t.Stop()

	var msg string
	for i := 0; i < 165000; i++ {
		msg += "h"
	}
	c.WriteMessage(websocket.TextMessage, []byte(msg))

	for {
		select {
		case <-done:
			return
		case _ = <-t.C:
			//err := c.WriteMessage(websocket.TextMessage, []byte(msg))
			//if err != nil {
			//	log.Println("write:", err)
			//	return
			//}
		}
	}
}

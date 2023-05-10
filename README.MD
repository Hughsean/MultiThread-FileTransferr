### 传输时序

```mermaid
sequenceDiagram
participant s as Server
participant c as Client
c->>s:访问控制端口
c-->>s:访问数据端口

c->>s:file[name, length](json)
note over c:创建文件发送线程
note over s:创建文件接收线程
s->>c:ready to accpet
note over c:创建多线程实体
	loop Client每一个线程实体
		c-->>s:Socket.Connect
	end
note over s:接受Client.Socket
	loop Server每一个线程实体
		c-->>s:传送字节流
	end
s->>c:接收无误
```

### 开发手册

error::connection_reset(远程主机强迫关闭了一个现有的连接)
error::connection_aborted(你的主机中的软件中止了一个已建立的连接)

`DownWork`==>`UpWork`:

```json
{
	"progress":"int: 接收进度",
}
```

`Scan`

```json
{
    "TYPE":"SCAN",
}
```

`Respond`:

```json
{
    "TYPE":"RESPOND",
}
```

`send`:

```json
{
    "FILENAME":"string",
    "FILESIZE":int
}
```

`tcplisten`:

```json
[
    {
       "ID":int,
       "PORT":int
    }
    ...
]
```



`App CMD`:

> * `help`: 获取帮助
> * `scan`: 扫描局域网, 获得局域网内设备`ip`
> * ~~connect [ip]: 连接到ip~~
> * `send [file] [ip]`: 发送文件`[file]`到`[ip]`
> * `exit`: 程序退出


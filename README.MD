# 多线程网络文件传输

### 传输时序

```mermaid
sequenceDiagram
participant s as Server
participant c as Client
c->>s:访问控制端口<br>file[name, length](json)
note over s:创建N个线程, 随机分配数据端口, 向任务池提交任务
s->>c:文件块id对应port<br>array[id:port]
note over c:根据id:port映射, 创建N线程, 访问服务端对应端口
	loop Client每一个线程实体
		c-->>s:Socket.Connect
	end
note over s:接受Client.Socket
	loop Server每一个线程实体
		c-->>s:传送字节流
	end
s->>c:接收完毕
note over s,c:释放资源
```

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
    "FILENAME":"string, 文件名称",
    "FILESIZE":"uint64, 文件大小"
}
```

`tcplisten`:

```json
[
    {
       "ID":"int, 文件块id",
       "PORT":"int, 端口号"
    }
    // ...
]
```



`App CMD`:

> * ~~`help`: 获取帮助~~
> * `scan`: 扫描局域网, 获得局域网内设备`ip`
> * ~~connect [ip]: 连接到ip~~
> * `send [file] [ip]`: 发送文件`[file]`到`[ip]`
> * `exit`: 程序退出

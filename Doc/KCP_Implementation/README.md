# Simple KCP Implementation for Unity

这是一个在Unity C#环境下的简易KCP协议实现，包含完整的客户端和服务器代码。

## 项目结构

```
KCP_Implementation/
├── KcpSegment.cs      # KCP数据段定义和编解码
├── Kcp.cs             # KCP核心协议实现
├── KcpClient.cs       # KCP客户端（Unity MonoBehaviour）
├── KcpServer.cs       # KCP服务器（Unity MonoBehaviour，用于测试）
├── KcpClientDemo.cs   # 客户端演示UI
└── README.md          # 本文档
```

## KCP 协议核心特性

### 1. 数据段结构 (KcpSegment)
- **conv**: 会话ID，用于区分不同的连接
- **cmd**: 命令类型 (PUSH/ACK/WASK/WINS)
- **frg**: 分片序号，支持大数据包分片传输
- **wnd**: 接收窗口大小，用于流量控制
- **ts**: 时间戳，用于RTT计算
- **sn**: 序列号，保证数据有序
- **una**: 确认号，表示已收到的最大连续序列号

### 2. 核心算法

#### ARQ自动重传请求
- 超时重传：RTO时间后自动重传未确认的包
- 快速重传：收到3个重复ACK立即重传
- 选择重传：只重传丢失的数据包

#### 滑动窗口
- 发送窗口：控制同时发送的数据包数量
- 接收窗口：控制可接收的数据包缓冲区大小
- 拥塞控制：动态调整发送速率

#### RTT计算和RTO调整
- 动态计算往返时间(RTT)
- 根据RTT调整重传超时时间(RTO)
- 平滑RTT算法减少抖动影响

## 使用方法

### 1. 基本使用

#### 客户端
```csharp
// 创建KCP客户端组件
var kcpClient = gameObject.AddComponent<KcpClient>();
kcpClient.serverHost = "127.0.0.1";
kcpClient.serverPort = 4567;
kcpClient.conversation = 123456;

// 监听消息
kcpClient.OnMessageReceived += (message) => {
    Debug.Log($"收到消息: {message}");
};

// 发送消息
kcpClient.SendMessage("Hello KCP!");
```

#### 服务器
```csharp
// 创建KCP服务器组件
var kcpServer = gameObject.AddComponent<KcpServer>();
kcpServer.listenPort = 4567;
kcpServer.maxClients = 10;

// 广播消息
kcpServer.BroadcastMessage("服务器消息");
```

### 2. 演示场景设置

1. **创建服务器场景**
   - 创建空GameObject，命名为"KCP Server"
   - 添加`KcpServer`组件
   - 设置监听端口（默认4567）

2. **创建客户端场景**
   - 创建空GameObject，命名为"KCP Client"
   - 添加`KcpClient`组件
   - 设置服务器地址和端口

3. **创建UI演示场景**
   - 创建Canvas和UI元素
   - 添加`KcpClientDemo`组件
   - 连接UI组件引用

### 3. Hello World 测试

#### 步骤1：启动服务器
```csharp
// 在Unity中运行服务器场景
// 或者在代码中直接启动
var server = new GameObject("KCP Server").AddComponent<KcpServer>();
```

#### 步骤2：连接客户端
```csharp
// 在另一个Unity实例或同一实例中启动客户端
var client = new GameObject("KCP Client").AddComponent<KcpClient>();
client.SendMessage("Hello KCP World!");
```

#### 步骤3：验证通信
- 客户端发送消息
- 服务器接收并回显消息
- 客户端接收回显消息

## 核心实现详解

### 1. 数据包编解码
```csharp
// 编码：将KCP段转换为字节数组
public byte[] Encode()
{
    // 24字节固定头部 + 可变长度数据
    // 使用小端序编码，保证跨平台兼容性
}

// 解码：从字节数组解析KCP段
public static KcpSegment Decode(byte[] buffer, int offset, int length)
{
    // 解析头部字段
    // 提取数据部分
    // 验证数据完整性
}
```

### 2. 可靠传输机制
```csharp
// 发送缓冲区管理
private List<KcpSegment> snd_buf = new List<KcpSegment>();

// 重传逻辑
private void ProcessRetransmission()
{
    foreach (var segment in snd_buf)
    {
        // 超时重传检查
        if (current >= segment.resendts)
        {
            // 重传数据包
            // 更新RTO
        }
        
        // 快速重传检查
        else if (segment.fastack >= 3)
        {
            // 立即重传
        }
    }
}
```

### 3. 流量控制
```csharp
// 窗口大小控制
private void MoveSndQueueToBuf()
{
    // 检查发送窗口是否有空间
    while (snd_queue.Count > 0 && 
           snd_buf.Count < Math.Min(snd_wnd, rmt_wnd))
    {
        // 移动数据到发送缓冲区
        // 分配序列号
        // 设置重传参数
    }
}
```

## 性能优化建议

### 1. 参数调优
```csharp
// 减少延迟的设置
kcp.SetMtu(1400);           // 设置合适的MTU
kcp.interval = 20;          // 减少更新间隔
kcp.rx_minrto = 10;         // 减少最小RTO
```

### 2. 内存管理
- 使用对象池减少GC压力
- 预分配缓冲区避免频繁分配
- 及时清理超时连接

### 3. 网络优化
- 合理设置窗口大小
- 根据网络状况调整参数
- 监控网络质量动态调整

## 测试验证

### 1. 功能测试
- [x] 基本连接建立
- [x] 消息发送接收
- [x] 分片和重组
- [x] 重传机制
- [x] 窗口控制

### 2. 性能测试
- 延迟测试：测量往返时间
- 吞吐量测试：大数据传输
- 丢包恢复：模拟网络丢包
- 并发连接：多客户端同时连接

### 3. 稳定性测试
- 长时间运行测试
- 网络中断恢复测试
- 内存泄漏检查
- 异常情况处理

## 扩展功能

### 1. 加密支持
- AES数据加密
- RSA密钥交换
- 消息完整性验证

### 2. 心跳机制
- 连接保活
- 网络质量监控
- 自动重连

### 3. 负载均衡
- 多服务器支持
- 客户端分流
- 故障转移

## 注意事项

1. **线程安全**：KCP本身不是线程安全的，需要在主线程中调用Update
2. **时间同步**：确保客户端和服务器时间基准一致
3. **防火墙**：UDP可能被防火墙阻挡，需要相应配置
4. **NAT穿透**：复杂网络环境可能需要NAT穿透技术

## 故障排除

### 常见问题
1. **连接失败**：检查IP地址和端口设置
2. **消息丢失**：检查网络环境和重传参数
3. **性能问题**：调整窗口大小和更新频率
4. **内存泄漏**：检查对象生命周期管理

### 调试工具
- Unity Console查看日志
- 网络抓包工具分析数据包
- 性能分析器监控资源使用

这个简易KCP实现提供了完整的可靠UDP传输功能，适合学习KCP原理和在Unity项目中使用。
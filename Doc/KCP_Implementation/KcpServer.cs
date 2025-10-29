using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

namespace SimpleKCP
{
    /// <summary>
    /// KCP 服务器实现（用于测试）
    /// </summary>
    public class KcpServer : MonoBehaviour
    {
        [Header("服务器设置")]
        public int listenPort = 4567;
        public int maxClients = 10;
        
        [Header("调试信息")]
        public bool enableDebugLog = true;
        
        // 网络组件
        private UdpClient udpServer;
        private Thread serverThread;
        private volatile bool isRunning = false;
        
        // 客户端管理
        private Dictionary<IPEndPoint, ClientSession> clients = new Dictionary<IPEndPoint, ClientSession>();
        private object clientsLock = new object();
        
        // 统计信息
        private int totalPacketsReceived = 0;
        private int totalPacketsSent = 0;
        
        /// <summary>
        /// 客户端会话类
        /// </summary>
        private class ClientSession
        {
            public IPEndPoint endPoint;
            public Kcp kcp;
            public uint lastActiveTime;
            public int packetsReceived;
            public int packetsSent;
            
            public ClientSession(IPEndPoint ep, uint conversation, Action<byte[], int> outputCallback)
            {
                endPoint = ep;
                kcp = new Kcp(conversation, outputCallback);
                lastActiveTime = Kcp.GetMilliseconds();
            }
            
            public void UpdateActivity()
            {
                lastActiveTime = Kcp.GetMilliseconds();
            }
            
            public bool IsTimeout(uint currentTime, uint timeoutMs = 30000)
            {
                return currentTime - lastActiveTime > timeoutMs;
            }
        }
        
        void Start()
        {
            StartServer();
        }
        
        void Update()
        {
            UpdateClients();
            ProcessClientMessages();
        }
        
        void OnDestroy()
        {
            StopServer();
        }
        
        /// <summary>
        /// 启动服务器
        /// </summary>
        private void StartServer()
        {
            try
            {
                udpServer = new UdpClient(listenPort);
                isRunning = true;
                
                serverThread = new Thread(ServerThreadFunction);
                serverThread.IsBackground = true;
                serverThread.Start();
                
                Log($"KCP服务器启动成功，监听端口: {listenPort}");
            }
            catch (Exception e)
            {
                LogError($"服务器启动失败: {e.Message}");
            }
        }
        
        /// <summary>
        /// 服务器线程函数
        /// </summary>
        private void ServerThreadFunction()
        {
            while (isRunning)
            {
                try
                {
                    IPEndPoint clientEndPoint = null;
                    byte[] data = udpServer.Receive(ref clientEndPoint);
                    
                    if (data != null && data.Length > 0)
                    {
                        totalPacketsReceived++;
                        ProcessClientPacket(clientEndPoint, data);
                    }
                }
                catch (SocketException se)
                {
                    if (isRunning)
                    {
                        LogError($"Socket异常: {se.Message}");
                    }
                }
                catch (Exception e)
                {
                    if (isRunning)
                    {
                        LogError($"服务器异常: {e.Message}");
                    }
                }
                
                Thread.Sleep(1);
            }
        }
        
        /// <summary>
        /// 处理客户端数据包
        /// </summary>
        private void ProcessClientPacket(IPEndPoint clientEndPoint, byte[] data)
        {
            lock (clientsLock)
            {
                ClientSession session = null;
                
                // 查找或创建客户端会话
                if (!clients.TryGetValue(clientEndPoint, out session))
                {
                    if (clients.Count >= maxClients)
                    {
                        Log($"客户端数量已达上限，拒绝新连接: {clientEndPoint}");
                        return;
                    }
                    
                    // 从数据包中提取conversation ID
                    uint conversation = ExtractConversation(data);
                    if (conversation == 0)
                    {
                        LogError($"无效的conversation ID从 {clientEndPoint}");
                        return;
                    }
                    
                    // 创建新会话
                    session = new ClientSession(clientEndPoint, conversation, 
                        (buffer, length) => SendToClient(clientEndPoint, buffer, length));
                    
                    clients[clientEndPoint] = session;
                    Log($"新客户端连接: {clientEndPoint}, conversation: {conversation}");
                }
                
                // 更新活动时间
                session.UpdateActivity();
                session.packetsReceived++;
                
                // 输入数据到KCP
                session.kcp.Input(data, 0, data.Length);
            }
        }
        
        /// <summary>
        /// 从数据包中提取conversation ID
        /// </summary>
        private uint ExtractConversation(byte[] data)
        {
            if (data.Length < 4) return 0;
            
            return (uint)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
        }
        
        /// <summary>
        /// 发送数据到客户端
        /// </summary>
        private void SendToClient(IPEndPoint clientEndPoint, byte[] buffer, int length)
        {
            try
            {
                udpServer.Send(buffer, length, clientEndPoint);
                totalPacketsSent++;
                
                lock (clientsLock)
                {
                    if (clients.TryGetValue(clientEndPoint, out ClientSession session))
                    {
                        session.packetsSent++;
                    }
                }
                
                if (enableDebugLog)
                {
                    Log($"发送到 {clientEndPoint}: {length} 字节");
                }
            }
            catch (Exception e)
            {
                LogError($"发送到客户端失败 {clientEndPoint}: {e.Message}");
            }
        }
        
        /// <summary>
        /// 更新所有客户端
        /// </summary>
        private void UpdateClients()
        {
            uint currentTime = Kcp.GetMilliseconds();
            List<IPEndPoint> timeoutClients = new List<IPEndPoint>();
            
            lock (clientsLock)
            {
                foreach (var kvp in clients)
                {
                    var session = kvp.Value;
                    
                    // 更新KCP状态
                    session.kcp.Update(currentTime);
                    
                    // 检查超时
                    if (session.IsTimeout(currentTime))
                    {
                        timeoutClients.Add(kvp.Key);
                    }
                }
                
                // 移除超时客户端
                foreach (var endPoint in timeoutClients)
                {
                    clients.Remove(endPoint);
                    Log($"客户端超时断开: {endPoint}");
                }
            }
        }
        
        /// <summary>
        /// 处理客户端消息
        /// </summary>
        private void ProcessClientMessages()
        {
            lock (clientsLock)
            {
                foreach (var kvp in clients)
                {
                    var session = kvp.Value;
                    ProcessSessionMessages(session);
                }
            }
        }
        
        /// <summary>
        /// 处理单个会话的消息
        /// </summary>
        private void ProcessSessionMessages(ClientSession session)
        {
            byte[] buffer = new byte[1024];
            int length = session.kcp.Recv(buffer, buffer.Length);
            
            if (length > 0)
            {
                string message = System.Text.Encoding.UTF8.GetString(buffer, 0, length);
                Log($"收到来自 {session.endPoint} 的消息: {message}");
                
                // 回复消息（Echo服务器）
                string reply = $"Echo: {message} (来自服务器)";
                SendMessageToClient(session, reply);
            }
        }
        
        /// <summary>
        /// 发送消息到指定客户端
        /// </summary>
        private void SendMessageToClient(ClientSession session, string message)
        {
            try
            {
                byte[] data = System.Text.Encoding.UTF8.GetBytes(message);
                int result = session.kcp.Send(data, data.Length);
                
                if (result < 0)
                {
                    LogError($"向客户端 {session.endPoint} 发送消息失败，错误码: {result}");
                }
            }
            catch (Exception e)
            {
                LogError($"向客户端 {session.endPoint} 发送消息异常: {e.Message}");
            }
        }
        
        /// <summary>
        /// 广播消息到所有客户端
        /// </summary>
        public void BroadcastMessage(string message)
        {
            lock (clientsLock)
            {
                foreach (var session in clients.Values)
                {
                    SendMessageToClient(session, message);
                }
            }
            
            Log($"广播消息: {message}");
        }
        
        /// <summary>
        /// 停止服务器
        /// </summary>
        private void StopServer()
        {
            isRunning = false;
            
            if (serverThread != null && serverThread.IsAlive)
            {
                serverThread.Join(1000);
                serverThread = null;
            }
            
            if (udpServer != null)
            {
                udpServer.Close();
                udpServer = null;
            }
            
            lock (clientsLock)
            {
                clients.Clear();
            }
            
            Log("服务器已停止");
        }
        
        /// <summary>
        /// 获取服务器统计信息
        /// </summary>
        public string GetStatistics()
        {
            lock (clientsLock)
            {
                return $"在线客户端: {clients.Count}, 总接收包: {totalPacketsReceived}, 总发送包: {totalPacketsSent}";
            }
        }
        
        // 日志方法
        private void Log(string message)
        {
            if (enableDebugLog)
            {
                Debug.Log($"[KCP服务器] {message}");
            }
        }
        
        private void LogError(string message)
        {
            Debug.LogError($"[KCP服务器] {message}");
        }
        
        // Unity Inspector 测试功能
        [Header("测试功能")]
        public string broadcastMessage = "服务器广播消息";
        
        [ContextMenu("广播测试消息")]
        public void SendBroadcastMessage()
        {
            BroadcastMessage(broadcastMessage);
        }
        
        [ContextMenu("显示统计信息")]
        public void ShowStatistics()
        {
            Log(GetStatistics());
        }
        
        [ContextMenu("重启服务器")]
        public void RestartServer()
        {
            StopServer();
            Thread.Sleep(100);
            StartServer();
        }
    }
}
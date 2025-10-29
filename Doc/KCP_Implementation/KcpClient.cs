using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

namespace SimpleKCP
{
    /// <summary>
    /// KCP 客户端实现
    /// </summary>
    public class KcpClient : MonoBehaviour
    {
        [Header("连接设置")]
        public string serverHost = "127.0.0.1";
        public int serverPort = 4567;
        public uint conversation = 123456;
        
        [Header("调试信息")]
        public bool enableDebugLog = true;
        
        // 网络组件
        private UdpClient udpClient;
        private IPEndPoint serverEndPoint;
        private Kcp kcp;
        
        // 线程控制
        private Thread receiveThread;
        private volatile bool isRunning = false;
        
        // 统计信息
        private int packetsSent = 0;
        private int packetsReceived = 0;
        
        void Start()
        {
            InitializeClient();
        }
        
        void Update()
        {
            if (kcp != null)
            {
                // 定期更新KCP状态
                kcp.Update(Kcp.GetMilliseconds());
                
                // 处理接收到的数据
                ProcessReceivedData();
            }
        }
        
        void OnDestroy()
        {
            Disconnect();
        }
        
        /// <summary>
        /// 初始化客户端
        /// </summary>
        private void InitializeClient()
        {
            try
            {
                // 创建UDP客户端
                udpClient = new UdpClient();
                serverEndPoint = new IPEndPoint(IPAddress.Parse(serverHost), serverPort);
                
                // 创建KCP实例
                kcp = new Kcp(conversation, OnKcpOutput);
                
                // 启动接收线程
                isRunning = true;
                receiveThread = new Thread(ReceiveThreadFunction);
                receiveThread.IsBackground = true;
                receiveThread.Start();
                
                Log("KCP客户端初始化成功");
            }
            catch (Exception e)
            {
                LogError($"客户端初始化失败: {e.Message}");
            }
        }
        
        /// <summary>
        /// KCP输出回调（发送数据到网络）
        /// </summary>
        private void OnKcpOutput(byte[] buffer, int length)
        {
            try
            {
                if (udpClient != null && serverEndPoint != null)
                {
                    udpClient.Send(buffer, length, serverEndPoint);
                    packetsSent++;
                    
                    if (enableDebugLog)
                    {
                        Log($"发送KCP包: {length} 字节, 总发送: {packetsSent}");
                    }
                }
            }
            catch (Exception e)
            {
                LogError($"发送数据失败: {e.Message}");
            }
        }
        
        /// <summary>
        /// 接收线程函数
        /// </summary>
        private void ReceiveThreadFunction()
        {
            while (isRunning)
            {
                try
                {
                    if (udpClient != null)
                    {
                        IPEndPoint remoteEP = null;
                        byte[] data = udpClient.Receive(ref remoteEP);
                        
                        if (data != null && data.Length > 0)
                        {
                            packetsReceived++;
                            
                            // 输入数据到KCP
                            kcp?.Input(data, 0, data.Length);
                            
                            if (enableDebugLog)
                            {
                                Log($"接收UDP包: {data.Length} 字节, 总接收: {packetsReceived}");
                            }
                        }
                    }
                }
                catch (SocketException se)
                {
                    if (isRunning) // 只有在运行状态下才报告错误
                    {
                        LogError($"Socket异常: {se.Message}");
                    }
                }
                catch (Exception e)
                {
                    if (isRunning)
                    {
                        LogError($"接收数据异常: {e.Message}");
                    }
                }
                
                // 短暂休眠避免CPU占用过高
                Thread.Sleep(1);
            }
        }
        
        /// <summary>
        /// 处理接收到的数据
        /// </summary>
        private void ProcessReceivedData()
        {
            if (kcp == null) return;
            
            byte[] buffer = new byte[1024];
            int length = kcp.Recv(buffer, buffer.Length);
            
            if (length > 0)
            {
                string message = System.Text.Encoding.UTF8.GetString(buffer, 0, length);
                Log($"收到消息: {message}");
                
                // 触发消息接收事件
                OnMessageReceived?.Invoke(message);
            }
        }
        
        /// <summary>
        /// 发送消息
        /// </summary>
        public void SendMessage(string message)
        {
            if (kcp == null)
            {
                LogError("KCP未初始化，无法发送消息");
                return;
            }
            
            try
            {
                byte[] data = System.Text.Encoding.UTF8.GetBytes(message);
                int result = kcp.Send(data, data.Length);
                
                if (result < 0)
                {
                    LogError($"发送消息失败，错误码: {result}");
                }
                else
                {
                    Log($"发送消息: {message}");
                }
            }
            catch (Exception e)
            {
                LogError($"发送消息异常: {e.Message}");
            }
        }
        
        /// <summary>
        /// 断开连接
        /// </summary>
        public void Disconnect()
        {
            isRunning = false;
            
            // 等待接收线程结束
            if (receiveThread != null && receiveThread.IsAlive)
            {
                receiveThread.Join(1000); // 最多等待1秒
                receiveThread = null;
            }
            
            // 关闭UDP客户端
            if (udpClient != null)
            {
                udpClient.Close();
                udpClient = null;
            }
            
            kcp = null;
            Log("客户端已断开连接");
        }
        
        /// <summary>
        /// 获取连接状态
        /// </summary>
        public bool IsConnected()
        {
            return kcp != null && udpClient != null && isRunning;
        }
        
        /// <summary>
        /// 获取统计信息
        /// </summary>
        public string GetStatistics()
        {
            return $"发送包数: {packetsSent}, 接收包数: {packetsReceived}, 待发送: {kcp?.WaitSnd() ?? 0}";
        }
        
        // 事件定义
        public event Action<string> OnMessageReceived;
        
        // 日志方法
        private void Log(string message)
        {
            if (enableDebugLog)
            {
                Debug.Log($"[KCP客户端] {message}");
            }
        }
        
        private void LogError(string message)
        {
            Debug.LogError($"[KCP客户端] {message}");
        }
        
        // Unity Inspector 测试按钮
        [Header("测试功能")]
        public string testMessage = "Hello KCP!";
        
        [ContextMenu("发送测试消息")]
        public void SendTestMessage()
        {
            SendMessage(testMessage);
        }
        
        [ContextMenu("显示统计信息")]
        public void ShowStatistics()
        {
            Log(GetStatistics());
        }
        
        [ContextMenu("重新连接")]
        public void Reconnect()
        {
            Disconnect();
            Thread.Sleep(100);
            InitializeClient();
        }
    }
}
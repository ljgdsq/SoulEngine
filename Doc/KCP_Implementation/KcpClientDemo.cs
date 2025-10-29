using UnityEngine;
using UnityEngine.UI;
using SimpleKCP;

namespace SimpleKCP.Demo
{
    /// <summary>
    /// KCP 客户端演示UI
    /// </summary>
    public class KcpClientDemo : MonoBehaviour
    {
        [Header("UI组件")]
        public InputField hostInput;
        public InputField portInput;
        public InputField messageInput;
        public Button connectButton;
        public Button disconnectButton;
        public Button sendButton;
        public Text statusText;
        public Text logText;
        public ScrollRect logScrollRect;
        
        [Header("KCP组件")]
        public KcpClient kcpClient;
        
        private void Start()
        {
            InitializeUI();
            SetupEventHandlers();
        }
        
        private void InitializeUI()
        {
            // 设置默认值
            if (hostInput != null)
                hostInput.text = "127.0.0.1";
            
            if (portInput != null)
                portInput.text = "4567";
            
            if (messageInput != null)
                messageInput.text = "Hello KCP World!";
            
            // 初始状态
            UpdateConnectionStatus(false);
            
            // 清空日志
            if (logText != null)
                logText.text = "";
        }
        
        private void SetupEventHandlers()
        {
            // 按钮事件
            if (connectButton != null)
                connectButton.onClick.AddListener(OnConnectClicked);
            
            if (disconnectButton != null)
                disconnectButton.onClick.AddListener(OnDisconnectClicked);
            
            if (sendButton != null)
                sendButton.onClick.AddListener(OnSendClicked);
            
            // 输入框回车事件
            if (messageInput != null)
            {
                messageInput.onEndEdit.AddListener((text) => {
                    if (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.KeypadEnter))
                    {
                        OnSendClicked();
                    }
                });
            }
            
            // KCP客户端事件
            if (kcpClient != null)
            {
                kcpClient.OnMessageReceived += OnMessageReceived;
            }
        }
        
        private void Update()
        {
            // 更新连接状态显示
            if (kcpClient != null)
            {
                UpdateConnectionStatus(kcpClient.IsConnected());
            }
        }
        
        private void OnConnectClicked()
        {
            if (kcpClient == null)
            {
                AddLog("错误: KCP客户端组件未设置");
                return;
            }
            
            try
            {
                // 更新连接参数
                if (hostInput != null && !string.IsNullOrEmpty(hostInput.text))
                    kcpClient.serverHost = hostInput.text;
                
                if (portInput != null && int.TryParse(portInput.text, out int port))
                    kcpClient.serverPort = port;
                
                // 如果已连接，先断开
                if (kcpClient.IsConnected())
                {
                    kcpClient.Disconnect();
                }
                
                // 重新初始化连接
                kcpClient.enabled = false;
                kcpClient.enabled = true;
                
                AddLog($"正在连接到 {kcpClient.serverHost}:{kcpClient.serverPort}...");
            }
            catch (System.Exception e)
            {
                AddLog($"连接失败: {e.Message}");
            }
        }
        
        private void OnDisconnectClicked()
        {
            if (kcpClient != null)
            {
                kcpClient.Disconnect();
                AddLog("已断开连接");
            }
        }
        
        private void OnSendClicked()
        {
            if (kcpClient == null)
            {
                AddLog("错误: KCP客户端组件未设置");
                return;
            }
            
            if (!kcpClient.IsConnected())
            {
                AddLog("错误: 未连接到服务器");
                return;
            }
            
            if (messageInput == null || string.IsNullOrEmpty(messageInput.text))
            {
                AddLog("错误: 消息内容为空");
                return;
            }
            
            try
            {
                string message = messageInput.text;
                kcpClient.SendMessage(message);
                AddLog($"发送: {message}");
                
                // 清空输入框
                messageInput.text = "";
                messageInput.ActivateInputField();
            }
            catch (System.Exception e)
            {
                AddLog($"发送失败: {e.Message}");
            }
        }
        
        private void OnMessageReceived(string message)
        {
            // 这个方法在主线程中调用，因为KcpClient已经处理了线程同步
            AddLog($"接收: {message}");
        }
        
        private void UpdateConnectionStatus(bool connected)
        {
            if (statusText != null)
            {
                statusText.text = connected ? "已连接" : "未连接";
                statusText.color = connected ? Color.green : Color.red;
            }
            
            if (connectButton != null)
                connectButton.interactable = !connected;
            
            if (disconnectButton != null)
                disconnectButton.interactable = connected;
            
            if (sendButton != null)
                sendButton.interactable = connected;
        }
        
        private void AddLog(string message)
        {
            if (logText == null) return;
            
            string timestamp = System.DateTime.Now.ToString("HH:mm:ss");
            string logEntry = $"[{timestamp}] {message}\n";
            
            logText.text += logEntry;
            
            // 限制日志长度
            if (logText.text.Length > 5000)
            {
                int cutIndex = logText.text.IndexOf('\n', 1000);
                if (cutIndex > 0)
                {
                    logText.text = logText.text.Substring(cutIndex + 1);
                }
            }
            
            // 自动滚动到底部
            if (logScrollRect != null)
            {
                Canvas.ForceUpdateCanvases();
                logScrollRect.verticalNormalizedPosition = 0f;
            }
        }
        
        // Unity Inspector 测试方法
        [ContextMenu("发送Hello World")]
        public void SendHelloWorld()
        {
            if (messageInput != null)
            {
                messageInput.text = "Hello KCP World!";
            }
            OnSendClicked();
        }
        
        [ContextMenu("发送长消息测试")]
        public void SendLongMessage()
        {
            if (messageInput != null)
            {
                messageInput.text = "这是一个长消息测试，用来验证KCP的分片和重组功能是否正常工作。" +
                                  "消息内容包含中文字符，测试编码解码功能。" +
                                  "This is a long message test to verify KCP fragmentation and reassembly. " +
                                  "The message contains both Chinese and English characters to test encoding/decoding.";
            }
            OnSendClicked();
        }
        
        [ContextMenu("清空日志")]
        public void ClearLog()
        {
            if (logText != null)
            {
                logText.text = "";
            }
        }
        
        [ContextMenu("显示统计信息")]
        public void ShowStatistics()
        {
            if (kcpClient != null)
            {
                AddLog($"统计信息: {kcpClient.GetStatistics()}");
            }
        }
    }
}
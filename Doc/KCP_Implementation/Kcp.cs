using System;
using System.Collections.Generic;

namespace SimpleKCP
{
    /// <summary>
    /// KCP 核心实现类
    /// </summary>
    public class Kcp
    {
        // 基本参数
        private uint conv;          // 会话ID
        private uint mtu;           // 最大传输单元
        private uint mss;           // 最大段大小
        private uint state;         // 连接状态
        
        // 发送相关
        private uint snd_una;       // 发送未确认序列号
        private uint snd_nxt;       // 发送下一个序列号
        private uint rcv_nxt;       // 接收下一个序列号
        private uint ssthresh;      // 慢开始阈值
        
        // 拥塞控制
        private uint cwnd;          // 拥塞窗口
        private uint probe;         // 探测标志
        private uint current;       // 当前时间
        private uint interval;      // 内部更新间隔
        private uint ts_recent;     // 最近时间戳
        private uint ts_lastack;    // 最后确认时间戳
        
        // 窗口控制
        private uint snd_wnd;       // 发送窗口
        private uint rcv_wnd;       // 接收窗口
        private uint rmt_wnd;       // 远程窗口
        
        // RTT 相关
        private uint rx_rttval;     // RTT方差
        private uint rx_srtt;       // 平滑RTT
        private uint rx_rto;        // 重传超时时间
        private uint rx_minrto;     // 最小重传超时时间
        
        // 缓冲区
        private List<KcpSegment> snd_queue = new List<KcpSegment>(); // 发送队列
        private List<KcpSegment> rcv_queue = new List<KcpSegment>(); // 接收队列
        private List<KcpSegment> snd_buf = new List<KcpSegment>();   // 发送缓冲区
        private List<KcpSegment> rcv_buf = new List<KcpSegment>();   // 接收缓冲区
        
        // 确认列表
        private List<uint> acklist = new List<uint>(); // ACK列表
        
        // 回调函数
        private Action<byte[], int> output; // 输出回调
        
        // 常量定义
        private const uint IKCP_RTO_NDL = 30;        // 最小重传超时时间
        private const uint IKCP_RTO_MIN = 100;       // 默认最小重传超时时间
        private const uint IKCP_RTO_DEF = 200;       // 默认重传超时时间
        private const uint IKCP_RTO_MAX = 60000;     // 最大重传超时时间
        private const uint IKCP_WND_SND = 32;        // 默认发送窗口
        private const uint IKCP_WND_RCV = 32;        // 默认接收窗口
        private const uint IKCP_MTU_DEF = 1400;      // 默认MTU
        private const uint IKCP_INTERVAL = 100;      // 默认更新间隔
        private const uint IKCP_OVERHEAD = 24;       // KCP头部开销
        private const uint IKCP_THRESH_INIT = 2;     // 初始慢开始阈值
        private const uint IKCP_THRESH_MIN = 2;      // 最小慢开始阈值
        
        public Kcp(uint conversation, Action<byte[], int> outputCallback)
        {
            conv = conversation;
            output = outputCallback;
            
            // 初始化参数
            mtu = IKCP_MTU_DEF;
            mss = mtu - IKCP_OVERHEAD;
            
            snd_wnd = IKCP_WND_SND;
            rcv_wnd = IKCP_WND_RCV;
            rmt_wnd = IKCP_WND_RCV;
            
            cwnd = 1;
            ssthresh = IKCP_THRESH_INIT;
            
            rx_rto = IKCP_RTO_DEF;
            rx_minrto = IKCP_RTO_MIN;
            interval = IKCP_INTERVAL;
            
            snd_nxt = 1;
            rcv_nxt = 1;
        }
        
        /// <summary>
        /// 发送数据
        /// </summary>
        public int Send(byte[] buffer, int len)
        {
            if (len <= 0) return -1;
            
            int count = 0;
            int offset = 0;
            
            // 如果数据长度超过MSS，需要分片
            if (len <= mss)
            {
                count = 1;
            }
            else
            {
                count = (int)((len + mss - 1) / mss);
            }
            
            if (count > 255) return -2; // 分片数量超过限制
            
            // 创建数据段
            for (int i = 0; i < count; i++)
            {
                int size = len - offset;
                if (size > mss) size = (int)mss;
                
                var segment = new KcpSegment(size);
                Array.Copy(buffer, offset, segment.data, 0, size);
                
                segment.conv = conv;
                segment.cmd = KcpSegment.IKCP_CMD_PUSH;
                segment.frg = (byte)(count - i - 1); // 倒序分片号
                segment.len = (uint)size;
                
                snd_queue.Add(segment);
                offset += size;
            }
            
            return 0;
        }
        
        /// <summary>
        /// 接收数据
        /// </summary>
        public int Recv(byte[] buffer, int len)
        {
            if (rcv_queue.Count == 0) return -1;
            
            int peeksize = PeekSize();
            if (peeksize < 0) return -2;
            if (peeksize > len) return -3;
            
            bool recover = rcv_queue.Count >= rcv_wnd;
            
            // 合并分片
            int mergedLen = 0;
            foreach (var segment in rcv_queue)
            {
                Array.Copy(segment.data, 0, buffer, mergedLen, segment.data.Length);
                mergedLen += segment.data.Length;
                
                if (segment.frg == 0) break;
            }
            
            // 移除已接收的分片
            int removeCount = 0;
            foreach (var segment in rcv_queue)
            {
                removeCount++;
                if (segment.frg == 0) break;
            }
            
            rcv_queue.RemoveRange(0, removeCount);
            
            // 移动接收缓冲区数据到接收队列
            MoveRcvBufToQueue();
            
            // 窗口恢复检查
            if (rcv_queue.Count < rcv_wnd && recover)
            {
                probe |= 1; // 请求远程窗口大小
            }
            
            return mergedLen;
        }
        
        /// <summary>
        /// 查看可接收数据大小
        /// </summary>
        public int PeekSize()
        {
            if (rcv_queue.Count == 0) return -1;
            
            var segment = rcv_queue[0];
            if (segment.frg == 0) return segment.data.Length;
            
            if (rcv_queue.Count < segment.frg + 1) return -1;
            
            int length = 0;
            foreach (var seg in rcv_queue)
            {
                length += seg.data.Length;
                if (seg.frg == 0) break;
            }
            
            return length;
        }
        
        /// <summary>
        /// 输入数据包（从网络接收）
        /// </summary>
        public int Input(byte[] data, int offset, int size)
        {
            uint prev_una = snd_una;
            uint maxack = 0;
            bool flag = false;
            
            if (size < 24) return -1; // 头部大小检查
            
            while (offset < size)
            {
                if (size - offset < 24) break;
                
                var segment = KcpSegment.Decode(data, offset, size - offset);
                if (segment == null) break;
                
                if (segment.conv != conv) return -1;
                
                offset += 24 + (int)segment.len;
                
                switch (segment.cmd)
                {
                    case KcpSegment.IKCP_CMD_ACK:
                        ProcessAck(segment.sn);
                        if (segment.sn > maxack)
                        {
                            maxack = segment.sn;
                            flag = true;
                        }
                        break;
                        
                    case KcpSegment.IKCP_CMD_PUSH:
                        ProcessData(segment);
                        break;
                        
                    case KcpSegment.IKCP_CMD_WASK:
                        probe |= 2; // 回应窗口大小
                        break;
                        
                    case KcpSegment.IKCP_CMD_WINS:
                        // 处理窗口大小响应
                        break;
                }
            }
            
            // 快速重传检查
            if (flag)
            {
                ProcessFastAck(maxack);
            }
            
            // 更新拥塞窗口
            if (snd_una > prev_una && cwnd < rmt_wnd)
            {
                uint mss_cwnd = cwnd + 1;
                if (mss_cwnd > rmt_wnd) mss_cwnd = rmt_wnd;
                cwnd = mss_cwnd;
            }
            
            return 0;
        }
        
        /// <summary>
        /// 处理ACK确认
        /// </summary>
        private void ProcessAck(uint sn)
        {
            for (int i = 0; i < snd_buf.Count; i++)
            {
                var segment = snd_buf[i];
                if (segment.sn == sn)
                {
                    snd_buf.RemoveAt(i);
                    break;
                }
            }
        }
        
        /// <summary>
        /// 处理数据包
        /// </summary>
        private void ProcessData(KcpSegment segment)
        {
            uint sn = segment.sn;
            
            // 序列号检查
            if (sn < rcv_nxt || sn >= rcv_nxt + rcv_wnd)
                return;
            
            // 添加到ACK列表
            acklist.Add(sn);
            
            // 如果是期望的下一个包
            if (sn == rcv_nxt)
            {
                rcv_buf.Add(segment);
                rcv_nxt++;
                
                // 移动连续的包到接收队列
                MoveRcvBufToQueue();
            }
            else
            {
                // 乱序包，插入到适当位置
                InsertSegment(rcv_buf, segment);
            }
        }
        
        /// <summary>
        /// 快速重传处理
        /// </summary>
        private void ProcessFastAck(uint sn)
        {
            foreach (var segment in snd_buf)
            {
                if (segment.sn < sn)
                {
                    segment.fastack++;
                }
            }
        }
        
        /// <summary>
        /// 移动接收缓冲区数据到接收队列
        /// </summary>
        private void MoveRcvBufToQueue()
        {
            while (rcv_buf.Count > 0)
            {
                var segment = rcv_buf[0];
                if (segment.sn == rcv_nxt && rcv_queue.Count < rcv_wnd)
                {
                    rcv_buf.RemoveAt(0);
                    rcv_queue.Add(segment);
                    rcv_nxt++;
                }
                else
                {
                    break;
                }
            }
        }
        
        /// <summary>
        /// 按序列号插入数据段
        /// </summary>
        private void InsertSegment(List<KcpSegment> list, KcpSegment segment)
        {
            int index = list.Count;
            for (int i = list.Count - 1; i >= 0; i--)
            {
                if (segment.sn >= list[i].sn)
                {
                    index = i + 1;
                    break;
                }
            }
            list.Insert(index, segment);
        }
        
        /// <summary>
        /// 更新KCP状态（需要定期调用）
        /// </summary>
        public void Update(uint current_time)
        {
            current = current_time;
            
            // 移动发送队列到发送缓冲区
            MoveSndQueueToBuf();
            
            // 处理重传
            ProcessRetransmission();
            
            // 发送ACK
            FlushAck();
            
            // 发送窗口询问
            FlushProbe();
            
            // 发送数据
            FlushData();
        }
        
        /// <summary>
        /// 移动发送队列到发送缓冲区
        /// </summary>
        private void MoveSndQueueToBuf()
        {
            while (snd_queue.Count > 0 && snd_buf.Count < Math.Min(snd_wnd, rmt_wnd))
            {
                var segment = snd_queue[0];
                snd_queue.RemoveAt(0);
                
                segment.sn = snd_nxt++;
                segment.una = rcv_nxt;
                segment.ts = current;
                segment.wnd = (ushort)Math.Min(rcv_wnd - rcv_queue.Count, 65535);
                segment.rto = rx_rto;
                segment.resendts = current + segment.rto;
                
                snd_buf.Add(segment);
            }
        }
        
        /// <summary>
        /// 处理重传
        /// </summary>
        private void ProcessRetransmission()
        {
            foreach (var segment in snd_buf)
            {
                bool needsend = false;
                
                // 超时重传
                if (current >= segment.resendts)
                {
                    needsend = true;
                    segment.rto = Math.Min(segment.rto * 2, IKCP_RTO_MAX);
                    segment.resendts = current + segment.rto;
                    segment.xmit++;
                }
                // 快速重传
                else if (segment.fastack >= 3)
                {
                    needsend = true;
                    segment.fastack = 0;
                    segment.resendts = current + segment.rto;
                    segment.xmit++;
                }
                
                if (needsend)
                {
                    segment.ts = current;
                    segment.wnd = (ushort)Math.Min(rcv_wnd - rcv_queue.Count, 65535);
                    segment.una = rcv_nxt;
                    
                    byte[] buffer = segment.Encode();
                    output?.Invoke(buffer, buffer.Length);
                }
            }
        }
        
        /// <summary>
        /// 发送ACK确认
        /// </summary>
        private void FlushAck()
        {
            foreach (uint sn in acklist)
            {
                var ack = new KcpSegment();
                ack.conv = conv;
                ack.cmd = KcpSegment.IKCP_CMD_ACK;
                ack.sn = sn;
                ack.una = rcv_nxt;
                ack.wnd = (ushort)Math.Min(rcv_wnd - rcv_queue.Count, 65535);
                ack.ts = current;
                
                byte[] buffer = ack.Encode();
                output?.Invoke(buffer, buffer.Length);
            }
            acklist.Clear();
        }
        
        /// <summary>
        /// 处理窗口探测
        /// </summary>
        private void FlushProbe()
        {
            if (probe != 0)
            {
                var segment = new KcpSegment();
                segment.conv = conv;
                segment.una = rcv_nxt;
                segment.wnd = (ushort)Math.Min(rcv_wnd - rcv_queue.Count, 65535);
                
                if ((probe & 1) != 0)
                {
                    segment.cmd = KcpSegment.IKCP_CMD_WASK;
                }
                else if ((probe & 2) != 0)
                {
                    segment.cmd = KcpSegment.IKCP_CMD_WINS;
                }
                
                byte[] buffer = segment.Encode();
                output?.Invoke(buffer, buffer.Length);
                
                probe = 0;
            }
        }
        
        /// <summary>
        /// 发送数据
        /// </summary>
        private void FlushData()
        {
            // 这个方法在 MoveSndQueueToBuf 和 ProcessRetransmission 中已经处理了数据发送
            // 这里可以添加额外的数据发送逻辑
        }
        
        /// <summary>
        /// 检查是否可以发送数据
        /// </summary>
        public int WaitSnd()
        {
            return snd_buf.Count + snd_queue.Count;
        }
        
        /// <summary>
        /// 设置最大传输单元
        /// </summary>
        public void SetMtu(uint mtu)
        {
            if (mtu < 50 || mtu < IKCP_OVERHEAD) return;
            
            this.mtu = mtu;
            this.mss = mtu - IKCP_OVERHEAD;
        }
        
        /// <summary>
        /// 获取当前时间（毫秒）
        /// </summary>
        public static uint GetMilliseconds()
        {
            return (uint)Environment.TickCount;
        }
    }
}
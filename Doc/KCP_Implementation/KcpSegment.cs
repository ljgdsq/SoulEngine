using System;

namespace SimpleKCP
{
    /// <summary>
    /// KCP 数据段结构
    /// </summary>
    public class KcpSegment
    {
        // KCP 头部字段
        public uint conv;      // 会话ID
        public byte cmd;       // 命令类型
        public byte frg;       // 分片数量
        public ushort wnd;     // 接收窗口大小
        public uint ts;        // 时间戳
        public uint sn;        // 序列号
        public uint una;       // 确认序列号
        public uint len;       // 数据长度
        
        // 重传控制
        public uint resendts;  // 重传时间戳
        public uint rto;       // 重传超时时间
        public uint fastack;   // 快速重传计数
        public uint xmit;      // 发送次数
        
        // 数据
        public byte[] data;
        
        // 命令类型常量
        public const byte IKCP_CMD_PUSH = 81; // 数据推送命令
        public const byte IKCP_CMD_ACK  = 82; // 确认命令
        public const byte IKCP_CMD_WASK = 83; // 窗口询问命令
        public const byte IKCP_CMD_WINS = 84; // 窗口大小命令
        
        public KcpSegment(int size = 0)
        {
            if (size > 0)
            {
                data = new byte[size];
            }
        }
        
        /// <summary>
        /// 将数据段编码为字节数组
        /// </summary>
        public byte[] Encode()
        {
            int headerSize = 24; // KCP头部固定24字节
            byte[] buffer = new byte[headerSize + (data?.Length ?? 0)];
            int offset = 0;
            
            // 编码头部
            WriteUInt32(buffer, ref offset, conv);
            buffer[offset++] = cmd;
            buffer[offset++] = frg;
            WriteUInt16(buffer, ref offset, wnd);
            WriteUInt32(buffer, ref offset, ts);
            WriteUInt32(buffer, ref offset, sn);
            WriteUInt32(buffer, ref offset, una);
            WriteUInt32(buffer, ref offset, len);
            
            // 编码数据
            if (data != null && data.Length > 0)
            {
                Array.Copy(data, 0, buffer, offset, data.Length);
            }
            
            return buffer;
        }
        
        /// <summary>
        /// 从字节数组解码数据段
        /// </summary>
        public static KcpSegment Decode(byte[] buffer, int offset, int length)
        {
            if (length < 24) return null; // 头部至少24字节
            
            var segment = new KcpSegment();
            int pos = offset;
            
            // 解码头部
            segment.conv = ReadUInt32(buffer, ref pos);
            segment.cmd = buffer[pos++];
            segment.frg = buffer[pos++];
            segment.wnd = ReadUInt16(buffer, ref pos);
            segment.ts = ReadUInt32(buffer, ref pos);
            segment.sn = ReadUInt32(buffer, ref pos);
            segment.una = ReadUInt32(buffer, ref pos);
            segment.len = ReadUInt32(buffer, ref pos);
            
            // 解码数据
            int dataLength = (int)segment.len;
            if (dataLength > 0 && pos + dataLength <= offset + length)
            {
                segment.data = new byte[dataLength];
                Array.Copy(buffer, pos, segment.data, 0, dataLength);
            }
            
            return segment;
        }
        
        // 辅助方法：写入32位整数(小端序)
        private static void WriteUInt32(byte[] buffer, ref int offset, uint value)
        {
            buffer[offset++] = (byte)(value & 0xFF);
            buffer[offset++] = (byte)((value >> 8) & 0xFF);
            buffer[offset++] = (byte)((value >> 16) & 0xFF);
            buffer[offset++] = (byte)((value >> 24) & 0xFF);
        }
        
        // 辅助方法：写入16位整数(小端序)
        private static void WriteUInt16(byte[] buffer, ref int offset, ushort value)
        {
            buffer[offset++] = (byte)(value & 0xFF);
            buffer[offset++] = (byte)((value >> 8) & 0xFF);
        }
        
        // 辅助方法：读取32位整数(小端序)
        private static uint ReadUInt32(byte[] buffer, ref int offset)
        {
            uint value = (uint)(buffer[offset] | 
                               (buffer[offset + 1] << 8) | 
                               (buffer[offset + 2] << 16) | 
                               (buffer[offset + 3] << 24));
            offset += 4;
            return value;
        }
        
        // 辅助方法：读取16位整数(小端序)
        private static ushort ReadUInt16(byte[] buffer, ref int offset)
        {
            ushort value = (ushort)(buffer[offset] | (buffer[offset + 1] << 8));
            offset += 2;
            return value;
        }
        
        public override string ToString()
        {
            return $"KcpSegment[cmd={cmd}, sn={sn}, una={una}, len={len}, frg={frg}]";
        }
    }
}
package com.iqiyi.inqnetworkutil;

import java.util.ArrayList;
import java.util.List;

public class JNativeWrapper {

    public static class PingStatus
    {
        public double loss_rate;
        public double minrtt;  // ms
        public double avgrtt;  // ms
        public double maxrtt;  // ms
        public String ip;
        public PingStatus(double loss_rate, double minrtt, double avgrtt, double maxrtt, String ip)
        {
            this.loss_rate = loss_rate;
            this.minrtt = minrtt;
            this.avgrtt = avgrtt;
            this.maxrtt = maxrtt;
            this.ip = ip;
        }
    }

    public static interface Callback
    {
        public void DnsQuery(ArrayList<String> ips);
        public void ping( String ip, double loss_rate, double minrtt, double avgrtt, double maxrtt);
    };

    //测试接口
    public native static int NetworkDiagnosis(String host, Callback callback);

    /**
     @timeout 查询超时时间
     @return ip列表逗号分割
     */
    public native static String NativeQueryDNS(String host, String dnsSer, int timeout);

    public native static String NativePing(String host, int times, int package_size, int interval/*S*/, int timeout/*S*/);

    public native static String NativeTraceRouter(String host);
}
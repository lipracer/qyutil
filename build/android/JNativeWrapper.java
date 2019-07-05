public class JNativeUtil
{
    //测试接口
    public native int NetworkDiagnosis(String host);

    /**
    @timeout 查询超时时间
    @return ip列表逗号分割 
     */
    public native String NativeQueryDNS(String host, String dnsSer, int timeout);

    public native String NativePing(String host, int times, int package_size, int interval/*S*/, int timeout/*S*/);

    public native String NativeTraceRouter(String host);
}
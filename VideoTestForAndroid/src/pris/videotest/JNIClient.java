package pris.videotest;

public class JNIClient {
	public static native boolean detect(int[] pixels, int width, int height);

	public static native boolean init(int[] pixels, int width, int height);

	public static native int helloAndroid(int[] pixels, int width, int height);
	
	public static native int[] detectWithReturn(int[] pixels, int width, int height);

	public static native boolean initVIBE(int[] pixels, int width, int height);

	public static native int[] detectWithVIBE(int[] pixels, int width, int height);
}

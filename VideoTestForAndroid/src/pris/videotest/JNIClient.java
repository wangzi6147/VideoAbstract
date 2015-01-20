package pris.videotest;

public class JNIClient {
	public static native boolean detect(byte[] data, int width, int height);

	public static native boolean init(byte[] data, int width, int height);

	public static native int helloAndroid(int[] pixels, int width, int height);
	
	public static native int[] detectWithReturn(int[] pixels, int width, int height);

	public static native boolean initVIBE(int[] pixels, int width, int height);

	public static native int[] detectWithVIBE(int[] pixels, int width, int height);

	public static native boolean initShift(int[] pixels, int width, int height);

	public static native boolean detectWithShift(int[] pixels, int width, int height);

	public static native boolean initWithByte(byte[] data, int width, int height);

	public static native boolean detectWithByte(byte[] data, int width, int height);

	public static native boolean detectWithDiff(byte[] data, int width, int height);
}

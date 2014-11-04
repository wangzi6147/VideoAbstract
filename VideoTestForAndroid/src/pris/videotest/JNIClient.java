package pris.videotest;

public class JNIClient {
	public static native boolean detect(int[] pixes, int width, int height);

	public static native boolean init(int[] pixels, int width, int height);

	public static native int helloAndroid(int[] pixels, int width, int height);
}

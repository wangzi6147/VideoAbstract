package pris.videotest;

import java.io.ByteArrayOutputStream;

import pris.videotest.MainActivity.AutoFocusCallback;
import pris.videotest.MainActivity.priviewCallBack;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

@SuppressLint("NewApi")
public class DetectionProcess implements Camera.ShutterCallback{

	//可以把例如检测有无摄像头、摄像头有无被占用、检测到入侵之后的一些操作放在这个类中
	
	private Camera mCamera;// Camera对象
	private String strCaptureFilePath = Environment.getExternalStorageDirectory() + "/DCIM/Camera/";// 保存图像的路径
	private AutoFocusCallback mAutoFocusCallback = new AutoFocusCallback();// AutoFocusCallback自动对焦的回调对象
	private boolean ifInit;
	private int screenWidth;
	private int screenHeight;
	private SurfaceTexture surfaceTexture;

	//检测有无摄像头的异常机制
	public class NoCameraException extends Exception {}
	public boolean checkCameraHandle() throws NoCameraException//throws NoCameraException{
	{
		if (Camera.getNumberOfCameras()!=0) {
			//Log.e("============", "摄像头存在");// 验证摄像头是否存在
			return true;
		}
		else{
			throw new NoCameraException();
		}
	}
	
	public void init() {
		surfaceTexture = new SurfaceTexture(10);
		ifInit = true;
		initCamera();
	}
	
	
	/* 拍照的method */
	private void takePicture() {
		if (mCamera != null) {
			//mCamera.takePicture(shutterCallback, rawCallback, jpegCallback);
		}
	}

	private ShutterCallback shutterCallback = new ShutterCallback() {
		public void onShutter() {
			/* 按下快门瞬间会调用这里的程序 */
		}
	};

	private PictureCallback rawCallback = new PictureCallback() {
		public void onPictureTaken(byte[] _data, Camera _camera) {
			/* 要处理raw data?写?否 */
			
		}
	};
	
	private PictureCallback jpegCallback = new PictureCallback() {
		public void onPictureTaken(byte[] _data, Camera _camera) {

			
			 if (Environment.getExternalStorageState().equals(
			 Environment.MEDIA_MOUNTED)) // 判断SD卡是否存在，并且可以可以读写 
				 {
				 System.out.println("sd卡存在且可读写");
			  } else { 
				  System.out.println("sd卡错误");
			  }
			 
			// Log.w("============", _data[55] + "");

			try {
				/* 取得相片 */
				Bitmap bm = BitmapFactory.decodeByteArray(_data, 0,
						_data.length);
//				/* 创建文件 */
//				File myCaptureFile = new File(strCaptureFilePath, "1.jpg");
//				BufferedOutputStream bos = new BufferedOutputStream(
//						new FileOutputStream(myCaptureFile));
//				/* 采用压缩转档方法 */
//				bm.compress(Bitmap.CompressFormat.JPEG, 100, bos);
//
//				/* 调用flush()方法，更新BufferStream */
//				bos.flush();
//
//				/* 结束OutputStream */
//				bos.close();

				/* 让相片显示3秒后圳重设相机 */
				// Thread.sleep(2000);
				/* 重新设定Camera */
				stopCamera();
				initCamera();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	};
	
	public boolean ifRefresh = true;
	private boolean isPreview = false; // 是否在预览中
	private int i = 0;
	private Bitmap bitmapForshow;
	
	
	/* 自定义class AutoFocusCallback */
	public final class AutoFocusCallback implements
			android.hardware.Camera.AutoFocusCallback {
		public void onAutoFocus(boolean focused, Camera camera) {

			/* 对到焦点拍照 */
			if (focused) {
//				takePicture();
			}
		}
	};
	
	/* 相机初始化的method */
	private void initCamera() {
//		if (mCamera != null) {
//			try {
//				Camera.Parameters parameters = mCamera.getParameters();
//				/*
//				 * 设定相片大小为1024*768， 格式为JPG
//				 */
//				// parameters.setPictureFormat(PixelFormat.JPEG);
//				parameters.setPictureSize(1024, 768);
//				mCamera.setParameters(parameters);
//				/* 打开预览画面 */
//				mCamera.startPreview();
//			} catch (Exception e) {
//				e.printStackTrace();
//			}
//		}
		if (!isPreview ) {  
            mCamera = Camera.open();  
        }  
        if (mCamera != null && !isPreview) {  
            try {  
                Camera.Parameters parameters = mCamera.getParameters();  
                parameters.setPreviewSize(screenWidth, screenHeight); // 设置预览照片的大小  
                parameters.setPreviewFpsRange(20, 30); // 每秒显示20~30帧  
                parameters.setPictureFormat(ImageFormat.NV21); // 设置图片格式  
                parameters.setPictureSize(screenWidth, screenHeight); // 设置照片的大小  
                //camera.setParameters(parameters); // android2.3.3以后不需要此行代码  
                //mCamera.setPreviewDisplay(null); // 通过SurfaceView显示取景画面  
                mCamera.setPreviewTexture(surfaceTexture);
                mCamera.setPreviewCallback(new priviewCallBack()); // 设置回调的类  
                mCamera.startPreview(); // 开始预览  
                mCamera.autoFocus(null); // 自动对焦  
            } catch (Exception e) {  
                e.printStackTrace();  
            }  
            isPreview = true;  
        } 
	}
	
	/* 停止相机的method */
	private void stopCamera() {
		if (mCamera != null) {
			try {
				/* 停止预览 */
				mCamera.stopPreview();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	// 每次cam采集到新图像时调用的回调方法，前提是必须开启预览
	class priviewCallBack implements Camera.PreviewCallback {

		@Override
		public void onPreviewFrame(byte[] data, Camera camera) {
			// Log.w("wwwwwwwww", data[5] + "");
			// Log.w("支持格式", mCamera.getParameters().getPreviewFormat()+"");
			if (ifRefresh) {
				ifRefresh = false;
				decodeToBitMap(data, camera);
				ifRefresh = true;
			}
		}
	}
	
	
	public void decodeToBitMap(byte[] data, Camera _camera) {
		Size size = mCamera.getParameters().getPreviewSize();
		try {
			long currentTimeMillis = System.currentTimeMillis();
//			YuvImage image = new YuvImage(data, ImageFormat.NV21, size.width,
//					size.height, null);
//			// Log.w("wwwwwwwww", size.width + " " + size.height);
//			if (image != null) {
//				ByteArrayOutputStream stream = new ByteArrayOutputStream();
//				image.compressToJpeg(new Rect(0, 0, size.width, size.height),
//						80, stream);
//				Bitmap bmp = BitmapFactory.decodeByteArray(
//						stream.toByteArray(), 0, stream.size());
//				sendImageIv.setImageBitmap(bmp);
				int width = size.width;
				int height = size.height;
//				int[] pixels = new int[width * height];
//				bmp.getPixels(pixels, 0, width, 0, 0, width, height);
//				System.out.println("red " + Color.red(pixels[0]));
//				System.out.println("green " + Color.green(pixels[0]));
//				System.out.println("blue " + Color.blue(pixels[0]));
//				System.out.println("alpha " + Color.alpha(pixels[0]));
				
//				int i = JNIClient.helloAndroid(pixels, width, height);
//				System.out.println(i);
//				if (ifInit) {
//					//if (JNIClient.init(data, width, height))
////					if (JNIClient.initShift(pixels, width, height))
//					//if(JNIClient.initVIBE(pixels, width, height))
//					if(JNIClient.initWithByte(data, width, height))
//						ifInit = false;
//				}
				
//				if (JNIClient.detect(data, width, height) && !ifInit) {
//					sendImageIv.setBackgroundColor(Color.RED);
//					System.out.println(System.currentTimeMillis()-currentTimeMillis);
//					i++;
//				}else {
//					sendImageIv.setBackgroundColor(Color.BLUE);
//				}
//				if (JNIClient.detectWithShift(pixels, width, height) && !ifInit) {
//				if (JNIClient.detectWithByte(data, width, height) && !ifInit) {
				if (JNIClient.detectWithDiff(data, width, height)){
					//socketThread.write(data);
					i++;
					System.out.println("time:"+(System.currentTimeMillis()-currentTimeMillis)+" num:"+i);
					//sendImageIv.setBackgroundColor(Color.RED);
				}else {
					//sendImageIv.setBackgroundColor(Color.BLUE);
				}
				//int[] result = JNIClient.detectWithReturn(pixels, width, height);
				//int[] result = JNIClient.detectWithVIBE(pixels, width, height);
				//bitmapForshow = Bitmap.createBitmap(result, 40, 30, Config.RGB_565);
				//sendImageIv.setImageBitmap(bitmapForshow);
				
				// Log.w("wwwwwwwww", bmp.getWidth() + " " + bmp.getHeight());
				// Log.w("wwwwwwwww",
				// (bmp.getPixel(100, 100) & 0xff) + "  "
				// + ((bmp.getPixel(100, 100) >> 8) & 0xff) + "  "
				// + ((bmp.getPixel(100, 100) >> 16) & 0xff));
//				image=null;
//				bmp.recycle();
//				pixels =null;
//				stream.close();
				//System.gc();
//			}
		} catch (Exception ex) {
			Log.e("Sys", "Error:" + ex.getMessage());
		}
	}

	@Override
	public void onShutter() {
		
	}
	
	static {
		System.loadLibrary("VideoTest");
	}
}


package pris.videotest;

import java.io.ByteArrayOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

	private Camera mCamera;// Camera对象
	private Button mButton;// 右侧条框，点击出发保存图像（拍照）的事件
	private SurfaceView mSurfaceView;// 显示图像的surfaceView
	private SurfaceHolder holder;// SurfaceView的控制器
	private AutoFocusCallback mAutoFocusCallback = new AutoFocusCallback();// AutoFocusCallback自动对焦的回调对象
	private ImageView sendImageIv;// 发送图片的imageview，位于右侧条框

	private String strCaptureFilePath = Environment
			.getExternalStorageDirectory() + "/DCIM/Camera/";// 保存图像的路径
	private boolean ifInit;
	private int screenWidth;
	private int screenHeight;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (checkCameraHardware(this)) {
			Log.e("============", "摄像头存在");// 验证摄像头是否存在
		}
		/* 隐藏状态栏 */
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		//屏幕常亮
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		/* 隐藏标题栏 */
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		/* 设定屏幕显示为横向 */
		// this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

		setContentView(R.layout.activity_main);// ----------------------

        screenWidth = 640;  
        screenHeight = 480; 
		/* SurfaceHolder设置 */
		mSurfaceView = (SurfaceView) findViewById(R.id.surface);
		holder = mSurfaceView.getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		/* 设置拍照Button的OnClick事件处理 */

		mButton = (Button) findViewById(R.id.btn);
		mButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				/* 自动对焦后拍照 */
				// mCamera.autoFocus(mAutoFocusCallback);// 调用mCamera的
				 //takePicture();
				ifRefresh = true;
				ifInit = true;
			}
		});

		sendImageIv = (ImageView) findViewById(R.id.imageView);

		Button socketTestBtn = (Button) findViewById(R.id.socketTestBtn);
		socketThread = new SocketThread();
		socketTestBtn.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				new Thread(socketThread).start();
			}
		});

	}

	// ///////----------重写SurfaceHolder.Callback接口的方法，
	// 在创建面板的时候调用的方法
	@Override
	public void surfaceCreated(SurfaceHolder surfaceholder) {

		ifInit = true;
		initCamera();
//		try {
//			mCamera = null;
//			try {
//				mCamera = Camera.open(0);// 打开相机；在低版本里，只有open（）方法；高级版本加入此方法的意义是具有打开多个
//				// 摄像机的能力，其中输入参数为摄像机的编号
//				// 在manifest中设定的最小版本会影响这里方法的调用，如果最小版本设定有误（版本过低），在ide里将不允许调用有参的
//				// open方法;
//				// 如果模拟器版本较高的话，无参的open方法将会获得null值!所以尽量使用通用版本的模拟器和API；
//			} catch (Exception e) {
//				Log.e("============", "摄像头被占用");
//			}
//			if (mCamera == null) {
//				Log.e("============", "摄像机为空");
//				System.exit(0);
//			}
//			mCamera.setPreviewDisplay(holder);// 设置显示面板控制器
//			priviewCallBack pre = new priviewCallBack();// 建立预览回调对象
//			mCamera.setPreviewCallback(pre); // 设置预览回调对象
//			// mCamera.getParameters().setPreviewFormat(ImageFormat.JPEG);
//			mCamera.startPreview();// 开始预览，这步操作很重要
//		} catch (IOException exception) {
//			mCamera.release();
//			mCamera = null;
//		}

		// 不添加显示面板的代码：
		/*
		 * 打开相机， mCamera = null; try { mCamera = Camera.open(0); } catch
		 * (Exception e) { Log.e("============", "摄像头被占用"); } if (mCamera ==
		 * null) { Log.e("============", "返回结果为空"); System.exit(0); } //
		 * mCamera.setPreviewDisplay(holder); priviewCallBack pre = new
		 * priviewCallBack(); mCamera.setPreviewCallback(pre); Log.w("wwwwwwww",
		 * mCamera.getParameters().getPreviewFormat() + "");
		 * mCamera.startPreview();
		 */
	}

	// 在面板改变的时候调用的方法
	@Override
	public void surfaceChanged(SurfaceHolder surfaceholder, int format, int w,
			int h) {
		/* 相机初始化 */
//		initCamera();
	}

	// 销毁面板时的方法
	@Override
	public void surfaceDestroyed(SurfaceHolder surfaceholder) {
//		stopCamera();
//		mCamera.release();
//		mCamera = null;
		 // 如果camera不为null ,释放摄像头  
        if (mCamera != null) {  
            if (isPreview)  
                mCamera.stopPreview();  
            mCamera.release();  
            mCamera = null;  
        }  
        System.exit(0); 
	}

	/* 拍照的method */
	private void takePicture() {
		if (mCamera != null) {
			mCamera.takePicture(shutterCallback, rawCallback, jpegCallback);
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

	// 在takepicture中调用的回调方法之一，接收jpeg格式的图像
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
				sendImageIv.setImageBitmap(bm);
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
	public boolean ifRefresh = false;
	private boolean isPreview = false; // 是否在预览中
	private int i = 0;
	private Bitmap bitmapForshow;
	private SocketThread socketThread;

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
            mCamera = Camera.open(0);  
        }  
        if (mCamera != null && !isPreview) {  
            try {  
                Camera.Parameters parameters = mCamera.getParameters();  
                parameters.setPreviewSize(screenWidth, screenHeight); // 设置预览照片的大小  
                parameters.setPreviewFpsRange(20, 30); // 每秒显示20~30帧  
                parameters.setPictureFormat(ImageFormat.NV21); // 设置图片格式  
                parameters.setPictureSize(screenWidth, screenHeight); // 设置照片的大小  
                // camera.setParameters(parameters); // android2.3.3以后不需要此行代码  
                mCamera.setPreviewDisplay(holder); // 通过SurfaceView显示取景画面  
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

	// 检测摄像头是否存在的私有方法
	private boolean checkCameraHardware(Context context) {
		if (context.getPackageManager().hasSystemFeature(
				PackageManager.FEATURE_CAMERA)) {
			// 摄像头存在
			return true;
		} else {
			// 摄像头不存在
			return false;
		}
	}

	// 每次cam采集到新图像时调用的回调方法，前提是必须开启预览
	class priviewCallBack implements Camera.PreviewCallback {

		@Override
		public void onPreviewFrame(byte[] data, Camera camera) {
			// TODO Auto-generated method stub
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
				if (ifInit) {
					//if (JNIClient.init(data, width, height))
//					if (JNIClient.initShift(pixels, width, height))
					//if(JNIClient.initVIBE(pixels, width, height))
					if(JNIClient.initWithByte(data, width, height))
						ifInit = false;
				}
				
//				if (JNIClient.detect(data, width, height) && !ifInit) {
//					sendImageIv.setBackgroundColor(Color.RED);
//					System.out.println(System.currentTimeMillis()-currentTimeMillis);
//					i++;
//				}else {
//					sendImageIv.setBackgroundColor(Color.BLUE);
//				}
//				if (JNIClient.detectWithShift(pixels, width, height) && !ifInit) {
				if (JNIClient.detectWithByte(data, width, height) && !ifInit) {
					sendImageIv.setBackgroundColor(Color.RED);
					socketThread.write(data);
					i++;
					System.out.println(System.currentTimeMillis()-currentTimeMillis);
				}else {
					sendImageIv.setBackgroundColor(Color.BLUE);
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
				System.gc();
//			}
		} catch (Exception ex) {
			Log.e("Sys", "Error:" + ex.getMessage());
		}
	}

	static {
		System.loadLibrary("VideoTest");
	}

}

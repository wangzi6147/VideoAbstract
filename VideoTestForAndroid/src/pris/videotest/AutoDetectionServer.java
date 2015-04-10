package pris.videotest;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;


import pris.videotest.MainActivity.priviewCallBack;
import android.annotation.SuppressLint;
import android.app.Service;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.os.Environment;
import android.os.IBinder;
import android.util.Log;
import android.view.SurfaceView;

@SuppressLint("NewApi") public class AutoDetectionServer extends Service {

	private String strCaptureFilePath = Environment.getExternalStorageDirectory() + "/tmp/housekeeping/";// 保存图像的路径
	protected Context mContext;
	private Camera mCamera;
	private int screenWidth = 640;  
	private int screenHeight = 480; 
	private int count = 0;
	//private DetectionProcess MyDetection=new DetectionProcess();
	
	
	public boolean ifRefresh = false;
	private boolean isPreview = false; // 是否在预览中
	private int i = 0;
	private Bitmap bitmapForshow;

	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		//LogUtil.d("自动侦测Auto Detection Server 启动");
		bindBroadcast();
		
		System.out.println("service_create");
	}
	private void bindBroadcast() {
		// TODO Auto-generated method stub
		//LogUtil.d("bindBroadcast()");
		IntentFilter filter = new IntentFilter();
		filter.addAction(Constants.HK_OPEN_AUTO_DETRCT);//设置属性
		this.registerReceiver(mReceiver, filter);
	}

	private BroadcastReceiver mReceiver = new BroadcastReceiver() {

		private SurfaceTexture surfaceTexture;

		@Override
		public void onReceive(Context arg0, Intent intent) {
			// TODO Auto-generated method stub
			//LogUtil.d("mReceiver onReceive(),action = "+intent.getAction());
			System.out.println("receive");
			mContext = arg0;
			String action = intent.getAction();
			if(Constants.HK_OPEN_AUTO_DETRCT.equals(action))//处理属性设置
			{
				String status = intent.getStringExtra("open_status");//获取打开/关闭状态
				//LogUtil.d("open_status = "+status);
				if("1".equals(status))//打开自动侦测功能
				{
					//==========================
					//此处进行打开自动侦测操作
					System.out.println("open");
					mCamera = openFacingBackCamera();
					System.out.println("open successfully");
					surfaceTexture = new SurfaceTexture(10);
					try {
						mCamera.setPreviewTexture(surfaceTexture);
					} catch (IOException e) {
						// TODO 自动生成的 catch 块
						e.printStackTrace();
					}
//					SurfaceView dummy = new SurfaceView(getBaseContext());
//					try {
//						mCamera.setPreviewDisplay(dummy.getHolder());
//					} catch (IOException e) {
//						e.printStackTrace();
//					}
					Camera.Parameters parameters = mCamera.getParameters();  
	                parameters.setPreviewSize(screenWidth, screenHeight); // 设置预览照片的大小  
	                parameters.setPreviewFpsRange(20, 30); // 每秒显示20~30帧  
	                parameters.setPictureFormat(ImageFormat.NV21); // 设置图片格式  
	                parameters.setPictureSize(screenWidth, screenHeight); // 设置照片的大小  
	                // camera.setParameters(parameters); // android2.3.3以后不需要此行代码   
	                mCamera.setPreviewCallback(new priviewCallBack()); // 设置回调的类  
	                mCamera.startPreview(); // 开始预览  
	                isPreview=true;
	                mCamera.autoFocus(null); // 自动对焦  
	                ifRefresh=true;
					//===============================
					//打开成功返回结果
					//LogUtil.d("打开自动侦测功能成功 ");
					Intent detectionOpenCommandACK = new Intent();
					detectionOpenCommandACK.setAction(Constants.HK_OPEN_AUTO_DETRCT_ACK);
					detectionOpenCommandACK.putExtra("open_status", "1");
					detectionOpenCommandACK.putExtra("result", "1");//result=1表示成功，result=0表示失败
					//LogUtil.d("返回广播给Interface Manager，open_status =1,result = 1");
					sendBroadcast(detectionOpenCommandACK);
				}
				else if("0".equals(status))
				{
					
					//==========================
					//此处进行关闭自动侦测功能
					System.out.println("close");
					//mCamera.cancelAutoFocus();
					if (mCamera != null)  
	                {
	                    if (isPreview)
	                    {
//		                    mCamera.stopPreview();  
		                    mCamera.release();  
		                    mCamera = null;
	                    }
	                }  
					ifRefresh=false;
//					stopCamera();
					//===============================
					//关闭成功返回结果
					//LogUtil.d("关闭自动侦测功能成功");
					Intent detectionClosecommandACK = new Intent();
					detectionClosecommandACK.setAction(Constants.HK_OPEN_AUTO_DETRCT_ACK);
					detectionClosecommandACK.putExtra("open_status", "0");
					detectionClosecommandACK.putExtra("result", "1");//result=1表示成功，result=0表示失败
					//LogUtil.d("返回广播给Interface Manager，open_status =0,result = 1");
					sendBroadcast(detectionClosecommandACK);
				}
			}
			
			

		}

	};
	
	/**
	 * 发送侦测异常消息
	 */
	public void SendWarnMsg()
	{
		//LogUtil.d("自动侦测发现侦测");
		Intent intent = new Intent();
		intent.setAction(Constants.HK_AUTO_DETRCT_WARN);
		//HouseKeepingApplication.getContext().sendBroadcast(intent);这里
	}

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		this.unregisterReceiver(mReceiver);
		super.onDestroy();
		System.out.println("service_destory");
	}
//	private void stopCamera() {
//		if (mCamera != null) {
//			try {
//				mCamera.stopPreview();
//			} catch (Exception e) {
//				e.printStackTrace();
//			}
//		}
//	}
	
	 private Camera openFacingBackCamera() {
		  Camera cam = null;
		  Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
		  ;
		  for (int camIdx = 0, cameraCount = Camera.getNumberOfCameras(); camIdx < cameraCount; camIdx++) {
		   Camera.getCameraInfo(camIdx, cameraInfo);
		   if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
		    try {
		     cam = Camera.open(camIdx);
		    } catch (RuntimeException e) {
		     e.printStackTrace();
		    }
		   }
		  }

		  return cam;
		 }

		class priviewCallBack implements Camera.PreviewCallback {

			@Override
			public void onPreviewFrame(byte[] data, Camera camera) {
				
				
				//System.out.println("OK");

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
					int width = size.width;
					int height = size.height;

					if (JNIClient.detectWithDiff(data, width, height)){
						//socketThread.write(data);
						count++;
						System.out.println("time:"+(System.currentTimeMillis()-currentTimeMillis)+" num:");
						if(count%5==0){
							System.out.println("save");
							YuvImage image = new YuvImage(data,ImageFormat.NV21,width,height,null); 
							/* 创建文件 */
							File myCaptureFile = new File(strCaptureFilePath, System.currentTimeMillis()+".jpg");
							BufferedOutputStream bos = new BufferedOutputStream(
									new FileOutputStream(myCaptureFile));
							/* 采用压缩转档方法 */
							image.compressToJpeg(new Rect(0, 0, width, height), 50, bos);
							/* 调用flush()方法，更新BufferStream */
							bos.flush();
							/* 结束OutputStream */
							bos.close();
							count = 0;
							
							Intent detectedWarn = new Intent();
							detectedWarn.setAction(Constants.HK_AUTO_DETRCT_WARN);
							detectedWarn.putExtra("type ","picture");
							//LogUtil.d("返回广播给Interface Manager，open_status =0,result = 1");
							sendBroadcast(detectedWarn);
						}
					}else {
						//System.out.println("ABC");
					}

			} catch (Exception ex) {
				Log.e("Sys", "Error:" + ex.getMessage());
			}
		}
		static {
			System.loadLibrary("VideoTest");
		}
}
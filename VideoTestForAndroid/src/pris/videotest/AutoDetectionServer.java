package pris.videotest;
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
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.os.IBinder;
import android.util.Log;
import android.view.SurfaceView;

@SuppressLint("NewApi") public class AutoDetectionServer extends Service {


	protected Context mContext;
	private Camera mCamera;
	private int screenWidth = 640;  
	private int screenHeight = 480; 
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
	                mCamera.autoFocus(null); // 自动对焦  
	                ifRefresh=true;
					//===============================
					//打开成功返回结果
					//LogUtil.d("打开自动侦测功能成功 ");
					Intent result = new Intent();
					result.setAction(Constants.START_AUTO_DETECT_SERVICE);
					result.putExtra("open_status", "1");
					result.putExtra("result", "1");//result=1表示成功，result=0表示失败
					//LogUtil.d("返回广播给Interface Manager，open_status =1,result = 1");
					mContext.sendBroadcast(result);
				}
				else if("0".equals(status))
				{
					
					//==========================
					//此处进行关闭自动侦测功能
					System.out.println("close");
					//===============================
					//关闭成功返回结果
					//LogUtil.d("关闭自动侦测功能成功");
					Intent result = new Intent();
					result.setAction(Constants.HK_OPEN_AUTO_DETRCT_ACK);
					result.putExtra("open_status", "0");
					
					
					
					result.putExtra("result", "1");//result=1表示成功，result=0表示失败
					//LogUtil.d("返回广播给Interface Manager，open_status =0,result = 1");
					mContext.sendBroadcast(result);
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
	
	 private Camera openFacingBackCamera() {
		  Camera cam = null;
		  Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
		  ;
		  for (int camIdx = 0, cameraCount = Camera.getNumberOfCameras(); camIdx < cameraCount; camIdx++) {
		   Camera.getCameraInfo(camIdx, cameraInfo);
		   if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK) {
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
						//i++;
						System.out.println("time:"+(System.currentTimeMillis()-currentTimeMillis)+" num:");
						
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

package pris.videotest;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import com.google.gson.Gson;

import pris.videotest.file.FileManager;
import android.annotation.SuppressLint;
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
import android.os.StatFs;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.Toast;

@SuppressLint("NewApi") public class AutoDetectionServer extends Service {

	private String strCaptureFilePath = Environment.getExternalStorageDirectory() + "/tmp/HouseKeeping/";// 保存图像的路径
	private File houseKeepingFile = new File(strCaptureFilePath);
	protected Context mContext;
	private Camera mCamera;
	private int screenWidth = 640;  
	private int screenHeight = 480; 
	//private int count = 0;
	private int boxsize=3;
	private int imageCount=0;
	String dateStr = null;
	String timeStr = null;
	//private DetectionProcess MyDetection=new DetectionProcess();
	
	
	public boolean ifRefresh = false;
	private boolean isPreview = false; // 是否在预览中
	private int i = 0;
	private Bitmap bitmapForshow;
	
	private int imageQuality=100;
	private int broadcastFrequency=300;//默认一个广播段300秒，即5分钟
	private long thisTurnStartTimeMillis=0;//控制在检测到目标时，一秒钟保存一次图片
	private long thisBroadCastStartTimeMillis=0;//五分钟为一个广播段，记录广播段开始的时间
	private long thisBroadCastDetectTimeMillis=0;//一秒钟判断一次是否需要发广播
	private boolean checkSendWarning=false;//五分钟更新一次标志位
	private long startTimeMillis = 0;//前5秒不检测
	private ArrayList<ImageInfo> ImagesInfo;//存该广播段内前五张图片信息

	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		//LogUtil.d("自动侦测Auto Detection Server 启动");
		bindBroadcast();
		if (!houseKeepingFile.exists()) {
			houseKeepingFile.mkdirs();
		}
		fileManager = new FileManager();
        Toast.makeText(this, "service start", Toast.LENGTH_LONG).show();
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
				String extendedInfo = intent.getStringExtra("extendedInfo");
				if(extendedInfo!=null){
					Gson gson = new Gson();
					Info info = gson.fromJson(extendedInfo, Info.class);
					
					screenWidth=info.getScreenWidth();
					screenHeight=info.getScreenHeight();
					fileManager.setMaxSize(info.getStoreMax());
					broadcastFrequency=info.getBroadcastFrequency();
					imageQuality=info.getImageQuality();
					boxsize=info.getBoxSize();
				}

				
//				System.out.println("lalala"+info.getBoxThreshold());
				//LogUtil.d("open_status = "+status);
				if("1".equals(status))//打开自动侦测功能
				{
					if(mCamera==null){
						fileManager.initLog(strCaptureFilePath);
						startTimeMillis = System.currentTimeMillis();
						//==========================
						//此处进行打开自动侦测操作
						System.out.println("open");
						mCamera = openFacingBackCamera();
						System.out.println("open successfully");
						surfaceTexture = new SurfaceTexture(10);
						try {
							mCamera.setPreviewTexture(surfaceTexture);
						} catch (IOException e) {
							e.printStackTrace();
						}
						Camera.Parameters parameters = mCamera.getParameters();  
						// 选择合适的预览尺寸
						List<Camera.Size> sizeList = parameters
								.getSupportedPreviewSizes();
						// 如果sizeList只有一个我们也没有必要做什么了，因为就他一个别无选择
						if (sizeList.size() > 1) {
							Iterator<Camera.Size> itor = sizeList.iterator();
							while (itor.hasNext()) {
								Camera.Size cur = itor.next();
								if (cur.width >= screenWidth
										&& cur.height >= screenHeight) {
									screenWidth = cur.width;
									screenHeight = cur.height;
									break;
								}
							}
						}
		                parameters.setPreviewSize(screenWidth, screenHeight); // 设置预览照片的大小  
		                parameters.setPreviewFpsRange(20, 30); // 每秒显示20~30帧  
		                //parameters.setPictureFormat(ImageFormat.NV21); // 设置图片格式  
		                parameters.setPictureSize(screenWidth, screenHeight); // 设置照片的大小  
		                mCamera.setParameters(parameters); 
		                mCamera.setPreviewCallback(new priviewCallBack()); // 设置回调的类  
		                mCamera.startPreview(); // 开始预览  
		                isPreview=true;
		                mCamera.autoFocus(null); // 自动对焦  
		                ifRefresh=true;
					}
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
					try{
						if (mCamera != null)  
		                {
		                    if (isPreview)
		                    {
			                    mCamera.stopPreview();
			                    mCamera.setPreviewCallback(null);
			                    mCamera.release();  
			                    mCamera = null;
		                    }
		                }  
						ifRefresh=false;
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
					catch(Exception e) {
						Intent detectionClosecommandACK = new Intent();
						detectionClosecommandACK.setAction(Constants.HK_OPEN_AUTO_DETRCT_ACK);
						detectionClosecommandACK.putExtra("open_status", "0");
						detectionClosecommandACK.putExtra("result", "0");//result=1表示成功，result=0表示失败
						//LogUtil.d("返回广播给Interface Manager，open_status =0,result = 1");
						sendBroadcast(detectionClosecommandACK);
					}
				}

			}
		}
	};
	private FileManager fileManager;
	
	/**
	 * 发送侦测异常消息
	 */
	public void SendWarnMsg()
	{
		//LogUtil.d("自动侦测发现侦测");
		Gson imagesGson = new Gson();
		Intent intent = new Intent();
		intent.setAction(Constants.HK_AUTO_DETRCT_WARN);
		intent.putExtra("type ","picture");
		intent.putExtra("imgs",imagesGson.toJson(ImagesInfo));
		sendBroadcast(intent);
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
		for (int camIdx = 0, cameraCount = Camera.getNumberOfCameras(); camIdx < cameraCount; camIdx++) {
			Camera.getCameraInfo(camIdx, cameraInfo);
			if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
				try {
					cam = Camera.open(camIdx);
				} catch (RuntimeException e) {
					e.printStackTrace();
					Intent detectionOpenCommandACK = new Intent();
					detectionOpenCommandACK.setAction(Constants.HK_OPEN_AUTO_DETRCT_ACK);
					detectionOpenCommandACK.putExtra("open_status", "1");
					detectionOpenCommandACK.putExtra("result", "0");//result=1表示成功，result=0表示失败
					//LogUtil.d("返回广播给Interface Manager，open_status =1,result = 1");
					sendBroadcast(detectionOpenCommandACK);
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
					if((currentTimeMillis-startTimeMillis)/1000<=5){//跳过前5秒
						checkSendWarning=true;
						return;
					}
					
					if((currentTimeMillis-thisBroadCastStartTimeMillis)/1000>=broadcastFrequency){//5分钟为一个广播段(300秒)
						checkSendWarning=true;
					}
					if((currentTimeMillis-thisBroadCastDetectTimeMillis)>=1000){//隔1秒判断一次是否发广播
						thisBroadCastDetectTimeMillis=currentTimeMillis;
						if((currentTimeMillis-thisBroadCastStartTimeMillis)/1000==5 && checkSendWarning==false){
							int curSize = ImagesInfo.size();
							//如果不足5张 重复添加最后一张
							if(curSize<5){
								ImageInfo imageInfo = ImagesInfo.get(curSize-1);
								String lastTimeStr = imageInfo.getPic_time();
								String lastNameStr = imageInfo.getPic_name();
								String strs[] = lastTimeStr.split("_");
								String timeHHStr = strs[0];
								String timeMMStr = strs[1];
								int timeSSInt = Integer.parseInt(strs[2]);
								for(int i = ImagesInfo.size();i<5;i++){
									timeSSInt = timeSSInt+1;
									String curTimeStr = timeHHStr+"_"+timeMMStr+"_"+timeSSInt;
									ImageInfo newInfo = new ImageInfo(lastNameStr, curTimeStr);
									ImagesInfo.add(newInfo);
								}
							}
							SendWarnMsg();
						}
					}
						
					
					if (JNIClient.detectWithDiff(data, width, height)){
//						System.out.println("time:"+(System.currentTimeMillis()-currentTimeMillis)+" num:");
						
						//long temp11=(System.currentTimeMillis()-thisTurnStartTimeMillis)/1000;
						if(checkSendWarning){//更新参数
							thisBroadCastStartTimeMillis = currentTimeMillis;
							checkSendWarning=false;
							
							SimpleDateFormat dateFormatter = new SimpleDateFormat("yyyy-MM-dd/HH_mm_ss");
							Date curDate = new Date(currentTimeMillis);//获取当前时间     
							String str = dateFormatter.format(curDate);
							String strs[] = str.split("/");
							dateStr = strCaptureFilePath+strs[0]+'/';
							timeStr = strs[1];
							
							File dateFile=new File(dateStr);
							ImagesInfo=new ArrayList<ImageInfo>();
							
							if (!dateFile.exists()) {
								dateFile.mkdirs();
							}
							imageCount=0;
						}
						if((currentTimeMillis-thisTurnStartTimeMillis)>=1000){//运动段内，每隔1秒进一次，保存图片
							thisTurnStartTimeMillis=currentTimeMillis;
							System.out.println("save"+imageCount);
							YuvImage image = new YuvImage(data,ImageFormat.NV21,width,height,null); 
							/* 创建文件 */
							String fileStr = dateStr+timeStr+"_"+imageCount+".jpg";
							File myCaptureFile = new File(fileStr);
							BufferedOutputStream bos = new BufferedOutputStream(
									new FileOutputStream(myCaptureFile));
							
							SimpleDateFormat dateFormatter = new SimpleDateFormat("yyyy-MM-dd/HH_mm_ss");
							Date curDate = new Date(currentTimeMillis);//获取当前时间     
							String str = dateFormatter.format(curDate);
							String strs[] = str.split("/");
							String imageTimeStr = strs[1];
							
							if ((currentTimeMillis-thisBroadCastStartTimeMillis)/1000<5) {//保存前5张图片信息
								ImagesInfo.add(new ImageInfo(timeStr+"_"+imageCount+".jpg",imageTimeStr));
							}
							imageCount+=1;
							/* 采用压缩转档方法 */
							if(getSDRemain()>data.length){
								//640*360
								image.compressToJpeg(new Rect(0, 60, width, height-60), imageQuality, bos);
							}
							/* 调用flush()方法，更新BufferStream */
							bos.flush();
							/* 结束OutputStream */
							bos.close();
//							count = 0;
							fileManager.updateLog(fileStr);
							
						}
						//count++;
					}else {
//						//System.out.println("ABC");
//						if(count!=0){
//							count = 0;
//						}
					}

			} catch (Exception ex) {
				Log.e("Sys", "Error:" + ex.getMessage());
			}
		}
		private long getSDRemain() {
			String sdcard = Environment.getExternalStorageDirectory().getPath();
			StatFs statFs = new StatFs(sdcard);
			long blockSize = statFs.getBlockSize();
			long blocks = statFs.getAvailableBlocks();
			return blockSize*blocks;
		}
		static {
			System.loadLibrary("VideoTest");
		}
}

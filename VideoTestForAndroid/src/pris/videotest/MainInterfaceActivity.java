package pris.videotest;

import com.google.gson.Gson;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;
import android.view.View.OnClickListener;

public class MainInterfaceActivity extends Activity implements OnClickListener{

	private Button detectionOpenBtn;
	private Button detectionCloseBtn;
	
	private Intent serviceIntent;
	
	
	protected Context mContext;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		/* 隐藏状态栏 */
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		//屏幕常亮
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		/* 隐藏标题栏 */
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		//捆绑xml
		setContentView(R.layout.main_interface);
		
		//设置监听器
		detectionOpenBtn=(Button)findViewById(R.id.detectionOpenBtn);
		detectionCloseBtn=(Button)findViewById(R.id.detectionCloseBtn);
		
		detectionOpenBtn.setOnClickListener(this); 
		detectionCloseBtn.setOnClickListener(this); 

		serviceIntent=new Intent(this,AutoDetectionServer.class);
		startService(serviceIntent);
        Toast.makeText(this, "service start by activity", Toast.LENGTH_LONG).show();
		
		bindBroadcast();
//		Gson gson = new Gson();
//		String json = gson.toJson(MainInfo);
	}
	private void bindBroadcast() {
		// TODO Auto-generated method stub
		//LogUtil.d("bindBroadcast()");
		IntentFilter filter = new IntentFilter();
		filter.addAction(Constants.HK_OPEN_AUTO_DETRCT_ACK);//设置属性
		filter.addAction(Constants.HK_AUTO_DETRCT_WARN);
		this.registerReceiver(mReceiver, filter);
	}
	
	private BroadcastReceiver mReceiver = new BroadcastReceiver() {
		
		@Override
		public void onReceive(Context context, Intent intent) {
			mContext = context;
			String action = intent.getAction();
			if(Constants.HK_OPEN_AUTO_DETRCT_ACK.equals(action))
			{
				String status = intent.getStringExtra("open_status");//获取打开/关闭状态
				String result = intent.getStringExtra("result");
				if("1".equals(status))//打开自动侦测功能
				{
					System.out.println("receive_open_ack");
					if("1".equals(result))
					{
						//得到service的开启ACK，并且打开成功
					}
				}
				else if("0".equals(status))
				{
					System.out.println("receive_close_ack");
					if("1".equals(result))
					{
						//得到service的关闭ACK，并且关闭成功
					}
				}
			}
			if(Constants.HK_AUTO_DETRCT_WARN.equals(action))
			{
				System.out.println("receive_warning_massage");
				System.out.println(intent.getStringExtra("imgs"));
			}
		}
	};
	
	
	
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.detectionOpenBtn:
//			//启动服务
			Intent detectionOpenCommand = new Intent();
			Gson openGson = new Gson();
			Info openInfo = new Info();
			//openInfo.setBroadcastFrequency(300);
			String openJson= openGson.toJson(openInfo);
			detectionOpenCommand.setAction(Constants.HK_OPEN_AUTO_DETRCT);
			detectionOpenCommand.putExtra("open_status", "1");
			detectionOpenCommand.putExtra("extendedInfo", openJson);
			sendBroadcast(detectionOpenCommand);
			break;
			
		case R.id.detectionCloseBtn:
			Intent detectionClosecommand = new Intent();
			Gson closeGson = new Gson();
//			Info closeInfo = new Info();
//			closeInfo.setBoxThreshold(0.0003f);
//			String closejson = closeGson.toJson(closeInfo);
			detectionClosecommand.setAction(Constants.HK_OPEN_AUTO_DETRCT);
			detectionClosecommand.putExtra("open_status", "0");
//			detectionClosecommand.putExtra("extendedInfo", closejson);
			sendBroadcast(detectionClosecommand);
			break;
			
		default:
			break;
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		stopService(serviceIntent);
		this.unregisterReceiver(mReceiver);
	}
}

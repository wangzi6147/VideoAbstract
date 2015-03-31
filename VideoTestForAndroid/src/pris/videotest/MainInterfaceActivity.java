package pris.videotest;

import com.google.gson.Gson;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.view.View.OnClickListener;

public class MainInterfaceActivity extends Activity implements OnClickListener{

	private Button detectionOpenBtn;
	private Button detectionCloseBtn;
	
	private Intent serviceIntent;
	
	private Info MainInfo;
	
	//protected Context mContext;
	
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
		
//		Gson gson = new Gson();
//		String json = gson.toJson(MainInfo);
}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.detectionOpenBtn:
//			//启动服务
			Intent detectionOpenCommand = new Intent();
			detectionOpenCommand.setAction(Constants.HK_OPEN_AUTO_DETRCT);
			detectionOpenCommand.putExtra("open_status", "1");
			sendBroadcast(detectionOpenCommand);
			break;
			
		case R.id.detectionCloseBtn:
			Intent detectionClosecommand = new Intent();
			detectionClosecommand.setAction(Constants.HK_OPEN_AUTO_DETRCT);
			detectionClosecommand.putExtra("open_status", "0");
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
	}
}

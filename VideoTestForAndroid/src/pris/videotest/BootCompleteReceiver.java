package pris.videotest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class BootCompleteReceiver extends BroadcastReceiver{

	@Override
	public void onReceive(Context context, Intent intent) {
		Intent serviceIntent = new Intent(context,AutoDetectionServer.class);  
        serviceIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
        context.startService(serviceIntent);
        System.out.println("service start by boot");
	}
}

package pris.videotest.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.content.Context;

public class FileManager {

	private String totalStr = "Total:\n";
	private String latestStr = "Latest:\n";
	private int totalSize;
	private String strLogFile;	
	
	public FileManager() {
	}

	public void updateLog(String fileStr) {
		File file = new File(fileStr);
		int length = 0;
		if (file.exists()) {
			FileInputStream fis = null;
			try {
				fis = new FileInputStream(file);
				length = fis.available();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		totalSize = totalSize+length;
		write(fileStr, length);
	}

	public void initLog(String strCaptureFilePath) {
		// TODO Auto-generated method stub
		strLogFile = strCaptureFilePath + "log.txt";
		File logFile = new File(strLogFile);
		if(!logFile.exists()){
			try {
				logFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			totalSize = 0;
			write("", 0);
		}
	}

	private void write(String latestFileStr, int latestSize) {
		// TODO Auto-generated method stub
		String writeStr = totalStr+totalSize+"\n"+latestStr+latestFileStr+" "+latestSize;
		byte[] bytes = writeStr.getBytes();
		try {
			FileOutputStream fileOutputStream = new FileOutputStream(strLogFile);
			fileOutputStream.write(bytes);
			fileOutputStream.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
}

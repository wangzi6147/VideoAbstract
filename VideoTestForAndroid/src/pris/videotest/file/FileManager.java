package pris.videotest.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

import android.os.Environment;

public class FileManager {

	private String totalStr = "Total:\n";
	private String latestStr = "Latest:\n";
	private int totalSize;
	private String strLogFile;
	private int maxSize = 100*1024*1024;// 200M
	private String strCaptureFilePath = Environment
			.getExternalStorageDirectory() + "/tmp/HouseKeeping/";// 保存图像的路径

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
				fis.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		totalSize = totalSize + length;
		if (totalSize > maxSize)
			deleteImg();
		write(fileStr, length);
	}

	private void deleteImg() {
		// TODO Auto-generated method stub
		File dir = new File(strCaptureFilePath);
		File[] dirFiles = dir.listFiles();
		ArrayList<String> dirList = new ArrayList<String>();
		for (File dirFile : dirFiles) {
			dirList.add(dirFile.getName());
		}
		Collections.sort(dirList);
		File imgDir = new File(strCaptureFilePath + dirList.get(0) + '/');
		File[] imgFiles = imgDir.listFiles();
		ArrayList<String> imgList = new ArrayList<String>();
		HashMap<String, File> imgMap = new HashMap<String, File>();
		for (File imgFile : imgFiles) {
			imgList.add(imgFile.getName());
			imgMap.put(imgFile.getName(), imgFile);
		}
		Collections.sort(imgList);
		int deleteCount = 0;
		String currentTimeStr = "";
		for (String imgStr : imgList) {
			System.out.println(deleteCount);
			String[] strings = imgMap.get(imgStr).getName().split("_");
			String timeStr = strings[0] + strings[1] + strings[2];
			if (!currentTimeStr.equals(timeStr)) {
				currentTimeStr = timeStr;
				deleteCount++;
				if (deleteCount > 10)
					break;
			}
			int length = 0;
			try {
				FileInputStream fis = new FileInputStream(imgMap.get(imgStr));
				length = fis.available();
				fis.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.println(totalSize + " " + length);
			totalSize = totalSize - length;
			System.out.println(imgMap.get(imgStr).getName());
			imgMap.get(imgStr).delete();
		}
		imgFiles = imgDir.listFiles();
		if (imgFiles.length == 0) {
			imgDir.delete();
		}
	}

	public void initLog(String strCaptureFilePath) {
		// TODO Auto-generated method stub
		strLogFile = strCaptureFilePath + "log.txt";
		File logFile = new File(strLogFile);
		if (!logFile.exists()) {
			try {
				logFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			totalSize = 0;
			write("", 0);
		} else {
			totalSize = 0;
			String latest = "";
			int latestSize = 0;
			File dir = new File(strCaptureFilePath);
			File[] dirFiles = dir.listFiles();
			ArrayList<String> dirList = new ArrayList<String>();
			for (File dirFile : dirFiles) {
				dirList.add(dirFile.getName());
			}
			Collections.sort(dirList);
			for (int i = 0; i < dirList.size()-1; i++) {
				File imgDir = new File(strCaptureFilePath + dirList.get(i)
						+ '/');
				File[] imgFiles = imgDir.listFiles();
				ArrayList<String> imgList = new ArrayList<String>();
				HashMap<String, File> imgMap = new HashMap<String, File>();
				for (File imgFile : imgFiles) {
					imgList.add(imgFile.getName());
					imgMap.put(imgFile.getName(), imgFile);
				}
				Collections.sort(imgList);
				for (String imgStr : imgList) {
					try {
						FileInputStream fis = new FileInputStream(
								imgMap.get(imgStr));
						latestSize = fis.available();
						fis.close();
					} catch (FileNotFoundException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					totalSize = totalSize + latestSize;
					latest = imgMap.get(imgStr).getAbsolutePath();
				}
			}
			System.out.println(totalSize + " " + latest + " " + latestSize);
			write(latest, latestSize);
		}
	}

	private void write(String latestFileStr, int latestSize) {
		// TODO Auto-generated method stub
		String writeStr = totalStr + totalSize + "\n" + latestStr
				+ latestFileStr + " " + latestSize;
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

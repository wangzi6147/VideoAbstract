package pris.videotest;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class SocketThread implements Runnable {

	private DataOutputStream dos;
	private ArrayList<byte[]> datas;

	@Override
	public void run() {
		datas = new ArrayList<byte[]>();
		try {
			Socket socket = new Socket("10.103.14.202", 8000);
			if (socket.isConnected()) {
				System.out.println("连接成功");
			} else {
				System.out.println("连接失败");
			}

			dos = new DataOutputStream(socket.getOutputStream());
			while (true) {
				if (datas.size() != 0) {
					byte[] data = datas.get(0);
					if (data != null) {
						datas.remove(0);
						dos.write(data);
						dos.flush();
					}
				}
			}
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void write(byte[] data) throws IOException {
		datas.add(data);
	}
}

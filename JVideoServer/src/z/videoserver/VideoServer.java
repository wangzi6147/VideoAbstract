package z.videoserver;

import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class VideoServer {
	public static void main(String[] args) throws IOException {
		// FileWriter fw = new FileWriter("E:\\640x480.yuv");
		FileOutputStream fos = new FileOutputStream("E:\\640x480.yuv");
		ServerSocket serverSocket = new ServerSocket(8000);
		System.out.println("服务器开启");
		Socket socket = serverSocket.accept();
		DataInputStream dis = new DataInputStream(socket.getInputStream());
		System.out.println("用户连接");
		byte[] bs = new byte[1024];
		while (true) {
			int count = dis.read(bs);
			if(count!=-1){
				fos.write(bs, 0 , count);
				System.out.println(count);
			}
		}
	}
}

package pris.videotest;

public class ImageInfo {

	String pic_name;
	String pic_time;
	public ImageInfo(String pic_name, String pic_time) {
		super();
		this.pic_name = pic_name;
		this.pic_time = pic_time;
	}
	public String getPic_name() {
		return pic_name;
	}
	public void setPic_name(String pic_name) {
		this.pic_name = pic_name;
	}
	public String getPic_time() {
		return pic_time;
	}
	public void setPic_time(String pic_time) {
		this.pic_time = pic_time;
	}
}

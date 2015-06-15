package pris.videotest;

public class Info {

	private int screenWidth=640;//图片宽度 
	private int screenHeight=480;//图片高度
	private int storeMax=100;//存储上限，默认100M
	private int broadcastFrequency=300;//广播频率，默认5分钟
	private int imageQuality=100;//图片质量0-100，100为无损
	
	//这三个参数与检测的灵敏度有关
	private int boxSize=3;//格子大小，3代表8，长宽可以不一样
	private int singleBoxThreshold=(int)(8*Math.pow((double)2, (double)boxSize));//单个格子判断为活跃的阈值
	private float boxThreshold=((float)screenWidth*(float)screenHeight*(float)0.0002);//判断有运动物体的格子阈值
	
	public int getBoxSize() {
		return boxSize;
	}
	public int getScreenWidth() {
		return screenWidth;
	}
	public void setScreenWidth(int screenWidth) {
		this.screenWidth = screenWidth;
	}
	public int getScreenHeight() {
		return screenHeight;
	}
	public void setScreenHeight(int screenHeight) {
		this.screenHeight = screenHeight;
	}
	public void setBoxSize(int boxSize) {
		this.boxSize = boxSize;
	}
	public int getSingleBoxThreshold() {
		return singleBoxThreshold;
	}
	public void setSingleBoxThreshold(int singleBoxThreshold) {
		this.singleBoxThreshold = singleBoxThreshold;
	}
	public float getBoxThreshold() {
		return boxThreshold;
	}
	public void setBoxThreshold(float boxThreshold) {
		this.boxThreshold = boxThreshold;
	}
	public int getStoreMax() {
		return storeMax;
	}
	public void setStoreMax(int storeMax) {
		this.storeMax = storeMax;
	}
	public int getBroadcastFrequency() {
		return broadcastFrequency;
	}
	public void setBroadcastFrequency(int broadcastFrequency) {
		this.broadcastFrequency = broadcastFrequency;
	}
	public int getImageQuality() {
		return imageQuality;
	}
	public void setImageQuality(int imageQuality) {
		this.imageQuality = imageQuality;
	}
	
}

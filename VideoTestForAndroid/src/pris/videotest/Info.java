package pris.videotest;

public class Info {

	private int boxSize;//格子大小，3代表8，长宽可以不一样
	private int singleBoxThreshold;//单个格子判断为活跃的阈值
	private float boxThreshold;//判断有运动物体的格子阈值
	private int storeMax;//存储上限
	private int storeInterCount;//存储频率
	private int imageQuality;//图片质量
	public int getBoxSize() {
		return boxSize;
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
	public int getStoreInterCount() {
		return storeInterCount;
	}
	public void setStoreInterCount(int storeInterCount) {
		this.storeInterCount = storeInterCount;
	}
	public int getImageQuality() {
		return imageQuality;
	}
	public void setImageQuality(int imageQuality) {
		this.imageQuality = imageQuality;
	}
	
}

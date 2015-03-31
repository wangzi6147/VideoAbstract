package pris.videotest;

public class Constants {
	/********** 看家app广播 **********************/
	// 看家app文件拷贝完毕广播
	public final static String EX_ACTION_FILE_COPY_COMPLETE_HOUSEKEEPING = "EX_ACTION_FILE_COPY_COMPLETE_HOUSEKEEPING";
	// 向看家请求数据广播
	public final static String EX_ACTION_GET_DATA = "EX_ACTION_GET_DATA";
	// 向Launcher发送看家app更新数据广播
	public final static String EX_ACTION_UPDATE_DATA = "EX_ACTION_UPDATE_DATA";
	public final static String BOOT_COMPLETED ="android.intent.action.BOOT_COMPLETED";
	
	public final static String START_HOUSE_KEEPING_SERVICE ="START_HOUSE_KEEPING_SERVICE";
	//自动侦测服务
	public final static String START_AUTO_DETECT_SERVICE ="START_AUTO_DETECT_SERVICE";
	public final static String HK_ATTR_SET ="HK_ATTR_SET";//属性设置
	public final static String HK_ATTR_SET_ACK ="HK_ATTR_SET_ACK";//属性设置返回

	public final static String HK_WARN_MSG ="HK_WARN_MSG";//报警信息
	public final static String HK_WARN_HAPPENED ="HK_WARN_HAPPENED";//Interface Manager向X1 Butel Cms发送侦测异常广播
	public final static String HK_OPEN_AUTO_DETRCT_ACK = "HK_OPEN_AUTO_DETRCT_ACK";//启动自动侦测服务返回广播
	public final static String HK_OPEN_AUTO_DETRCT = "HK_OPEN_AUTO_DETRCT";//启动自动侦测服务广播
	public final static String HK_AUTO_DETRCT_WARN = "HK_AUTO_DETRCT_WARN";//启动自动侦测服务向Interface Manager发送侦测异常广播
	public final static String MSG_APP_HOUSEKEEPING = "HouseKeeping";
	// Launcher卡片点击广播
		public final static String EX_ACTION_CARD_OPERATE = "EX_ACTION_CARD_OPEERATE";
		public final static String BEAN_TYPE_HOUSEKEEPING = "housekeeping";
		public static String SAVE_DIR_NAME = "HouseKeeping";
		 /** Sip 登陆注册时长 */
	    public static String UMENG_KEY_SIPREG_DURATION = "sipRegDuration";
}

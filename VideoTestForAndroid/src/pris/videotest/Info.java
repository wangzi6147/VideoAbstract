package pris.videotest;

import org.json.JSONObject;

public class Info {
	private String msgHead;//消息头
	private String sender;//发送者
	private String msgId;//消息ID
	private JSONObject receivers;//接受者
	private String title;//标题
	private String app;//应用类型
	private String type;//消息类型
	private JSONObject body;//消息体
	private JSONObject extendedInfo;//拓展字段
	public String getMsgHead() {
		return msgHead;
	}
	public void setMsgHead(String msgHead) {
		this.msgHead = msgHead;
	}
	public String getSender() {
		return sender;
	}
	public void setSender(String sender) {
		this.sender = sender;
	}
	public String getMsgId() {
		return msgId;
	}
	public void setMsgId(String msgId) {
		this.msgId = msgId;
	}
	public JSONObject getReceivers() {
		return receivers;
	}
	public void setReceivers(JSONObject receivers) {
		this.receivers = receivers;
	}
	public String getTitle() {
		return title;
	}
	public void setTitle(String title) {
		this.title = title;
	}
	public String getApp() {
		return app;
	}
	public void setApp(String app) {
		this.app = app;
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public JSONObject getBody() {
		return body;
	}
	public void setBody(JSONObject body) {
		this.body = body;
	}
	public JSONObject getExtendedInfo() {
		return extendedInfo;
	}
	public void setExtendedInfo(JSONObject extendedInfo) {
		this.extendedInfo = extendedInfo;
	}
	
	
	
	
}

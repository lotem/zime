package cn.zzsst.client;

import java.util.ArrayList;

import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.event.dom.client.KeyUpHandler;


public interface ZimeModule extends KeyDownHandler, KeyUpHandler {

	public String getPreedit();

	public void setPreedit(String str);

	public CandidateList getCandidateList();

	public void commitString(String str);

	public void pageDown();

	public void pageUp();

	public void showCandidates();

	public void hideCandidates();

	public void updateCandidates(ArrayList<String> list);

}

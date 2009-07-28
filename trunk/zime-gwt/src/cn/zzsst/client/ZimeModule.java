package cn.zzsst.client;

import java.util.ArrayList;


public interface ZimeModule {

	public String getPreedit();

	public void setPreedit(String str);

	public CandidateList getCandidateList();

	public void commitString(String str);

	public void pageDown();

	public void pageUp();

	public void showCandidates();

	public void hideCandidates();

	public void updateCandidates(ArrayList<String> list);

    public void submit();

}

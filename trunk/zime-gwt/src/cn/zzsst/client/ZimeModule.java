package cn.zzsst.client;


public interface ZimeModule {

	public String getPreedit();

	public void setPreedit(String str);

    void updateCandidates(CandidateList candidateList);

	public void showCandidates();

	public void hideCandidates();

    public void commitString(String str);

    public void submit();

}

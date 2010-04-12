package cn.zzsst.zime.client;

import java.util.ArrayList;


public interface Module {

	public String getPreedit();

	public void setPreedit(String str);

    void updateCandidates(CandidateList candidateList);

	public void showCandidates();

	public void hideCandidates();

    public void commitString(String str);

    public void submit();

    public void setSchema(Schema schema);

    public void setSchemaList(ArrayList<String[]> schemas);

}

package cn.zzsst.client;

import java.util.ArrayList;

public class Context {
    private String preedit;
    private int length;
    private int candidateLength;
    private ArrayList<Integer> startIndex;
    private ArrayList<Integer> endIndex;
    
    public Context() {
        preedit = "";
        length = 0;
        candidateLength = 0;
        startIndex = new ArrayList<Integer>();
        endIndex = new ArrayList<Integer>();
    }

    public void clear() {
        preedit = "";
        length = 0;
        candidateLength = 0;
        startIndex.clear();
        endIndex.clear();
    }
    
    public String getWord(int n) {
        return preedit.substring(startIndex.get(n), endIndex.get(n));
    }
    
    public String getPreedit() {
        return preedit;
    }
    public void setPreedit(String preedit) {
        this.preedit = preedit;
    }
    public void setLength(int length) {
        this.length = length;
    }
    public int getLength() {
        return length;
    }
    public int getCandidateLength() {
        return candidateLength;
    }
    public void setCandidateLength(int candidateLength) {
        this.candidateLength = candidateLength;
    }
    public ArrayList<Integer> getStartIndex() {
        return startIndex;
    }
    public void setStartIndex(ArrayList<Integer> startIndex) {
        this.startIndex = startIndex;
    }
    public ArrayList<Integer> getEndIndex() {
        return endIndex;
    }
    public void setEndIndex(ArrayList<Integer> endIndex) {
        this.endIndex = endIndex;
    }

    public String rest() {
        return (candidateLength == length) ? "" : 
            preedit.substring(startIndex.get(candidateLength));
    }

    public void add(int start, int end) {
        startIndex.add(start);
        endIndex.add(end);
        ++length;
    }
}

package cn.zzsst.client;

import java.util.ArrayList;

public class Context {
    private String preedit;
    private int length;
    private int candidateLength;
    private ArrayList<Integer> startIndex;
    private ArrayList<Integer> endIndex;
    private ArrayList<ArrayList<String>> cache;
    
    public Context() {
        preedit = "";
        length = 0;
        candidateLength = 0;
        startIndex = new ArrayList<Integer>();
        endIndex = new ArrayList<Integer>();
        cache = new ArrayList<ArrayList<String>>();
    }

    public void clear() {
        preedit = "";
        length = 0;
        candidateLength = 0;
        startIndex.clear();
        endIndex.clear();
        cache.clear();
    }
    
    public String getWord(int n) {
        return preedit.substring(startIndex.get(n), endIndex.get(n));
    }
    
    public boolean isEmpty() {
        return "".equals(preedit);
    }
    
    public String getPreedit() {
        return preedit;
    }
    public void setPreedit(String preedit) {
        this.preedit = preedit;
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

    public ArrayList<ArrayList<String>> getCache() {
        return cache;
    }

    public String rest() {
        return (candidateLength == length) ? "" : 
            preedit.substring(startIndex.get(candidateLength));
    }

    public void addWordRange(int start, int end) {
        startIndex.add(start);
        endIndex.add(end);
        ++length;
    }

    public ArrayList<String> fetchCache(int incr) {
        ArrayList<String> result;
        int k = candidateLength - 1;
        do {
            k += incr;
            if (k < 0)
                k += 4;
            else if (k >= 4)
                k -= 4;
            result = cache.get(k);
        } 
        while (result == null && k != candidateLength - 1);
        candidateLength = k + 1;
        return result;
    }
}

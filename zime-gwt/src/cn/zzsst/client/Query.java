package cn.zzsst.client;

import java.util.ArrayList;

public class Query {
    
    public static final int PENDING = 0;
    public static final int READY = 1;
    public static final int NOT_FOUND = 2;
    
    private int index;
    private int endPosition;
    private String queryString;
    private ArrayList<String> result;
    private int status;

    public Query(int index, int endPosition, String queryString) {
        this.index = index;
        this.endPosition = endPosition;
        this.queryString = queryString;
        result = null;
        status = PENDING;
    }
    
    @Override
    public boolean equals(Object o) {
        final Query q = (Query) o; 
        return q != null && 
               q.index == index && q.queryString.equals(queryString);
    }

    @Override
    public String toString() {
        String statusText = status == PENDING   ? "PENDING" : 
                            status == READY     ? "READY" : 
                            status == NOT_FOUND ? "NOT_FOUND" : "";
        return "[Query " + index + " " + endPosition + " " + statusText + "] " + queryString;
    }
    
    public int getIndex() {
        return index;
    }
    public void setIndex(int index) {
        this.index = index;
    }
    public String getQueryString() {
        return queryString;
    }
    public void setQueryString(String queryString) {
        this.queryString = queryString;
    }
    public ArrayList<String> getResult() {
        return result;
    }
    public void setResult(ArrayList<String> result) {
        this.result = result;
    }
    
    public void setStatus(int status) {
        this.status = status;
    }

    public int getStatus() {
        return status;
    }

    public int getEndPosition() {
        return endPosition;
    }

    public void setEndPosition(int pos) {
        this.endPosition = pos;
    }

    public boolean isPending() {
        return status == PENDING;
    }

    public boolean isReady() {
        return status == READY;
    }

    public boolean isNotFound() {
        return status == NOT_FOUND;
    }
}

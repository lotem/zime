package cn.zzsst.client;

import java.util.ArrayList;

public class CandidateList {
	
	private int pageSize;
	private int currentPage;
	private ArrayList<String> candidates;
	
	public CandidateList() {
		pageSize = 7;
		currentPage = 0;
		candidates = new ArrayList<String>();
	}

	public void setPageSize(int pageSize) {
		this.pageSize = pageSize;
	}

	public int getPageSize() {
		return pageSize;
	}

	public void setCurrentPage(int currentPage) {
		this.currentPage = currentPage;
	}

	public int getCurrentPage() {
		return currentPage;
	}

	public void clear() {
	    candidates.clear();
	    currentPage = 0;    
	}
	
	public void addCandidates(ArrayList<String> candidates) {
		this.candidates.addAll(candidates);
	}

	public ArrayList<String> getCandidates() {
		return candidates;
	}
	
	public String getSelectedCandidate(int i) {
		if (i < 0 || i >= pageSize)
			return null;
		int pos = currentPage * pageSize + i;
		if (pos >= candidates.size())
			return null;
		return candidates.get(pos);
	}

    public void pageDown() {
        int page = currentPage + 1;
        int start = page * pageSize;
        if (start < candidates.size())
            currentPage = page;
    }

    public void pageUp() {
        int page = currentPage - 1;
        if (page >= 0)
            currentPage = page;
    }

    public boolean isEmpty() {
        return candidates.isEmpty();
    }
}
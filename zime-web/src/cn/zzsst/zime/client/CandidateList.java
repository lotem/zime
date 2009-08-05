package cn.zzsst.zime.client;

import java.util.ArrayList;

public class CandidateList {
	
	private int pageSize;
	private int currentPage;
	private ArrayList<String> candidates;
	
	public CandidateList(ArrayList<String> candidates) {
        pageSize = 7;
        currentPage = 0;
        if (candidates == null)
            this.candidates = new ArrayList<String>();
        else
            this.candidates = candidates;
	}
	
	@Override
	public String toString() {
	    return candidates.toString();
	}

	public void clear() {
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

    public int getLastPage() {
        return candidates.isEmpty() ? 0 : (candidates.size() - 1) / pageSize;
    }
}
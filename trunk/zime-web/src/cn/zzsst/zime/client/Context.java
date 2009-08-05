package cn.zzsst.zime.client;

import java.util.ArrayList;

public class Context {
    private Dict dict;
    // configuration
    private int maxKeywordLength;
    private int maxQueryKeywords;
    private String delimiters;
    private String[][] splitRules;
    // states
    private String preedit;
    private ArrayList<Query> queries;
    private int queryIndex;
    private int lastFetched;
    
    public Context(Schema schema) {
        this.dict = schema.getDict();
        
        maxKeywordLength = schema.getMaxKeywordLength();
        maxQueryKeywords = schema.getMaxQueryKeywords();
        delimiters = schema.getDelimiters();
        ArrayList<String> list = schema.getSplitRules();
        int n = (list == null) ? 0 : list.size();
        splitRules = new String[n][];
        for (int i = 0; i < n; ++i) {
            splitRules[i] = list.get(i).split("\\s+");
        }
        
        preedit = "";
        queries = new ArrayList<Query>();
        for (int i = 0; i < maxQueryKeywords; ++i)
            queries.add(null);
        queryIndex = 0;
        lastFetched = 0;
    }

    public void clear() {
        preedit = "";
        for (int i = 0; i < queries.size(); ++i)
            queries.set(i, null);
        queryIndex = 0;
        lastFetched = 0;
    }
    
    public boolean isEmpty() {
        return "".equals(preedit);
    }
    
    public String getPreedit() {
        return preedit;
    }
    
    public Query getCurrentQuery() {
        return queries.get(queryIndex);
    }

    public void update(String p, LookupCallback callback) {
        //Logger.log("Context.update(): " + p);
        preedit = p;
        StringBuilder sb = new StringBuilder();
        String word = null;
        int start = 0;
        int i;
        for (i = 0; i < maxQueryKeywords; ++i) {
            word = findWord(p, start);
            if (word == null) {
                break;
            }
            start += word.length();
            if (start < p.length() && isDelimiter(p.charAt(start))) {
                ++start;
            }
            if (i > 0)
                sb.append('/');
            sb.append(word);
            Query query = new Query(i, start, sb.toString());
            Query oldQuery = queries.get(i);
            if (query.equals(oldQuery)) {
                //Logger.log("    existing: " + oldQuery);
                oldQuery.setEndPosition(start);
            }
            else {
                //Logger.log("    new: " + query);
                queries.set(i, query);
                queryIndex = i;
                doQuery(query, callback);
            }
        }
        for (int j = i; j < maxQueryKeywords; ++j) {
            queries.set(j, null);
        }
        if (queryIndex >= i) {
            //Logger.log("    shortened.");
            calculateQueryIndex();
            callback.onSuccess(fetchResult());
        }
    }

    private boolean isDelimiter(char c) {
        return delimiters.indexOf(c) != -1;
    }

    private abstract class QueryCallback implements LookupCallback {
        
        protected Query query;
        protected LookupCallback callback;

        public QueryCallback(Query query, LookupCallback callback) {
            this.query = query;
            this.callback = callback;
        }

    }

    private void doQuery(Query query, LookupCallback callback) {
        dict.lookup(query.getQueryString(), new QueryCallback(query, callback) {
            @Override
            public void onSuccess(CandidateList result) {
                query.setResult(result);
                query.setStatus(Query.READY);
                //Logger.log("QueryCallback.onReady(): " + query);
                //Logger.log("    queryIndex: " + queryIndex);
                if (queryIndex == query.getIndex()) {
                    callback.onSuccess(fetchResult());
                }
            }
            @Override
            public void onFailure(Throwable caught) {
                query.setResult(null);
                query.setStatus(Query.NOT_FOUND);
                if (queryIndex == query.getIndex()) {
                    calculateQueryIndex();
                    callback.onSuccess(fetchResult());
                }
            }
        });
    }

    private String findWord(String p, int start) {
        int i = Math.min(start + maxKeywordLength, p.length());
        for ( ; i > start; --i) {
            boolean split = false;
            if (i < p.length())
                for (String[] t: splitRules) {
                    if (t[0].indexOf(p.charAt(i - 1)) != -1 && 
                            t[1].indexOf(p.charAt(i)) != -1) {
                        Logger.log("split: " + p.substring(start, i) + "|" + p.substring(i));
                        //Logger.log("using split rule: " + t[0] + "|" + t[1]);
                        split = true;
                        break;
                    }
                }
            if (split)
                continue;
            final String s = p.substring(start, i);
            if (dict.exist(s))
                return s;
        }
        // not found
        return null;
    }

    public CandidateList fetchCache(int increment) {
        Query query = null;
        int k = queryIndex;
        for(int i = 0; i < maxQueryKeywords; ++i) {
            k += increment;
            if (k < 0)
                k += maxQueryKeywords;
            else if (k >= maxQueryKeywords)
                k -= maxQueryKeywords;
            query = queries.get(k);
            if (query != null && !query.isNotFound())
                break;
        } 
        queryIndex = k;
        return fetchResult();
    }

    private void calculateQueryIndex() {
        while (queryIndex > 0) {
            Query query = getCurrentQuery();
            if (query != null && !query.isNotFound()) {
                break;
            }
            --queryIndex;
        }
        //Logger.log("calculateQueryIndex: " + queryIndex);
        //Logger.log("    " + getCurrentQuery());
    }
    
    private CandidateList fetchResult() {
        Query query = getCurrentQuery();
        //Logger.log("fetchResult: " + query);
        lastFetched = queryIndex;
        if (query == null)
            return null;

        if (query.isNotFound()) {
            return null;
        }
        if (query.isReady())
            return query.getResult();

        if (query.isPending()) {
            int k = queryIndex;
            while (--k >= 0) {
                query = queries.get(k);
                if (query != null && query.isReady()) {
                    //Logger.log("    fallback: " + query);
                    lastFetched = k;
                    return query.getResult();
                }
            }
        }

        return null;
    }
    
    public String rest() {
        Query query = queries.get(lastFetched);
        return (query == null || !query.isReady()) ? "" : 
            preedit.substring(query.getEndPosition());
    }

}

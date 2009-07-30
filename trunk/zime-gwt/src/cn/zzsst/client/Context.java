package cn.zzsst.client;

import java.util.ArrayList;

public class Context {
    private Dict dict;
    private int maxWordLength;
    private int maxQueryWords;
    private String preedit;
    private ArrayList<Query> queries;
    private int queryIndex;
    private int lastFetched;
    
    public Context(Schema schema) {
        this.dict = schema.getDict();
        maxWordLength = schema.getMaxWordLength();
        maxQueryWords = schema.getMaxQueryWords();
        preedit = "";
        queries = new ArrayList<Query>();
        for (int i = 0; i < maxQueryWords; ++i)
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

    public void update(String p, Callback callback) {
        preedit = p;
        StringBuilder sb = new StringBuilder();
        String word = null;
        int start = 0;
        int i;
        for (i = 0; i < maxQueryWords; ++i) {
            word = findWord(p, start);
            if (word == null) {
                break;
            }
            start += word.length();
            if (start < p.length() && p.charAt(start) == '\'') {
                ++start;
            }
            if (i > 0)
                sb.append('-');
            sb.append(word);
            Query query = new Query(i, start, sb.toString());
            if (!query.equals(queries.get(i))) {
                System.err.println("    new: " + query);
                queries.set(i, query);
                queryIndex = i;
                doQuery(query, callback);
            }
            else {
                System.err.println("    existing: " + queries.get(i));
            }
        }
        for (int j = i; j < maxQueryWords; ++j) {
            queries.set(j, null);
        }
        if (queryIndex >= i) {
            System.err.println("    shortened.");
            calculateQueryIndex();
            callback.onReady(fetchResult());
        }
    }

    private void doQuery(Query query, Callback callback) {
        dict.lookup(query.getQueryString(), new QueryCallback(query, callback) {

            @Override
            public void onReady(ArrayList<String> result) {
                query.setResult(result);
                int status = (result == null) ? Query.NOT_FOUND : Query.READY;
                query.setStatus(status);
                System.err.println("QueryCallback.onReady(): " + query);
                System.err.println("    queryIndex: " + queryIndex);
                if (queryIndex == query.getIndex()) {
                    if (status == Query.NOT_FOUND)
                        calculateQueryIndex();
                    callback.onReady(fetchResult());
                }
            }
            
        });
    }

    private String findWord(String p, int start) {
        int i = Math.min(start + maxWordLength, p.length());
        for ( ; i > start; --i) {
            final String s = p.substring(start, i);
            if (dict.exist(s))
                return s;
        }
        // not found
        return null;
    }

    public ArrayList<String> fetchCache(int increment) {
        Query query = null;
        int k = queryIndex;
        for(int i = 0; i < maxQueryWords; ++i) {
            k += increment;
            if (k < 0)
                k += maxQueryWords;
            else if (k >= maxQueryWords)
                k -= maxQueryWords;
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
        System.err.println("calculateQueryIndex: " + queryIndex);
        System.err.println("    " + getCurrentQuery());
    }
    
    private ArrayList<String> fetchResult() {
        Query query = getCurrentQuery();
        System.err.println("fetchResult: " + query);
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
                    System.err.println("    fallback: " + query);
                    lastFetched = k;
                    return query.getResult();
                }
            }
        }

        return null;
    }
    
    public String rest() {
        Query query = queries.get(lastFetched);
        System.err.println("Context.rest(): " + query);
        System.err.println("    preedit: " + preedit);
        return (query == null || !query.isReady()) ? "" : 
            preedit.substring(query.getEndPosition());
    }

}
